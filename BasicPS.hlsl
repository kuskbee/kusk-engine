#include "Common.hlsli"

// 참고자료
// https://github.com/Nadrin/PBR/blob/master/data/shaders/hlsl/pbr.hlsl
// https://cdn2.unrealengine.com/Resources/files/2013SiggraphPresentationsNotes-26915738.pdf

Texture2D albedoTex : register(t0);
Texture2D normalTex : register(t1);
Texture2D aoTex : register(t2);
Texture2D metallicRoughnessTex : register(t3);
Texture2D emissiveTex : register(t4);

static const float3 Fdielectric = 0.04; // 비금속(Dielectric) 재질의 F0

cbuffer BasicPixelConstData : register(b0) 
{
    float3 albedoFactor;
    float roughnessFactor;
    float metallicFactor;
    float3 emissionFactor;
    
    int useAlbedoMap;
    int useNormalMap;
    int useAOMap;       // Ambient Occlusion
    int invertNormalMapY;
    int useMetallicMap;
    int useRoughnessMap;
    int useEmissiveMap;
    int isSelected;
};

struct PixelShaderOutput
{
    float4 pixelColor : SV_Target0;
};

float3 SchlickFresnel(float3 F0, float VdotH)
{
    return float3(F0 + (1.0 - F0) * pow(2.0, (-5.55473 * VdotH - 6.98316) * VdotH)); // Note (5)
    // return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

float3 GetNormal(PixelShaderInput input)
{
    float3 normalWorld = normalize(input.normalWorld);
    if (useNormalMap) // NormalWorld를 교체
    {
        float3 normal = normalTex.SampleLevel(linearWrapSampler, input.texcoord, lodBias).rgb;
        normal = 2.0 * normal - 1.0; // 범위 조절 [0.0, 1.0] => [-1.0, 1.0]
        
        // OpenGL용 노멀맵인 경우 y 방향을 뒤집어주기
        normal.y = invertNormalMapY ? -normal.y : normal.y;
        
        float3 N = normalWorld; // normal
        float3 T = normalize(input.tangentWorld - dot(input.tangentWorld, N) * N); // tangent
        float3 B = cross(N, T);
        
        // matrix는 float4x4, 여기서 벡터 변환용이라서 3x3 사용
        float3x3 TBN = float3x3(T, B, N);
        normalWorld = normalize(mul(normal, TBN));
    }
    
    return normalWorld;
}

float3 DiffuseIBL(float3 albedo, float3 normalWorld, float3 pixelToEye, float metallic)
{
    float3 F0 = lerp(Fdielectric, albedo, metallic);
    float3 F = SchlickFresnel(F0, max(0.0, dot(normalWorld, pixelToEye)));
    float3 kd = lerp(1.0 - F, 0.0, metallic);
    
    // 앞에서 사용했던 방법과 동일
    float3 irradiance = irradianceIBLTex.SampleLevel(linearWrapSampler, normalWorld, 0).rgb;
    
    return kd * albedo * irradiance;
}

float3 SpecularIBL(float3 albedo, float3 normalWorld, float3 pixelToEye, float metallic, float roughness)
{
    float2 specularBRDF = brdfTex.SampleLevel(linearClampSampler, float2(dot(normalWorld, pixelToEye), 1.0 - roughness), 0.0).rg;
    
    // 앞에서 사용했던 방법과 동일
    float3 specularIrradiance = specularIBLTex.SampleLevel(linearWrapSampler, reflect(-pixelToEye, normalWorld), 2 + roughness * 5.0f).rgb;
    const float3 Fdielectric = 0.04; // 비금속(Dielectric) 재질의 F0
    float3 F0 = lerp(Fdielectric, albedo, metallic);
    
    return (F0 * specularBRDF.x + specularBRDF.y) * specularIrradiance; // Note (8)
}

float3 AmbientLightingByIBL(float3 albedo, float3 normalW, float3 pixelToEye, float ao, float metallic, float roughness)
{
    float3 diffuseIBL = DiffuseIBL(albedo, normalW, pixelToEye, metallic);
    float3 specularIBL = SpecularIBL(albedo, normalW, pixelToEye, metallic, roughness);
    
    return (diffuseIBL + specularIBL) * ao;
}

// GGX / Towbridge-Reitz normal distribution function
// Uses Disney's reparametrization of alpha = roughness^2.
float NdfGGX(float NdotH, float roughness, float alphaPrime)
{
    // Note (3)
    float alpha = roughness * roughness;
    float alphaSq = alpha * alpha;
    float denom = (NdotH * NdotH) * (alphaSq - 1.0) + 1.0;
    
    return alphaPrime * alphaPrime / (3.141592 * denom * denom);
}

// Single term for separable Schlick-GGX below.
float SchlickG1(float NdotV, float k)
{
    return NdotV / (NdotV * (1.0 - k) + k);
}

// Note (4), Schlick-GGX approximation of geometric attenuation function using Smith's method.
// I : Input = light, O : Output = view
float SchlickGGX(float NdotI, float NdotO, float roughness) 
{
    float r = roughness + 1.0;
    float k = (r * r) / 8.0;
    return SchlickG1(NdotI, k) * SchlickG1(NdotO, k);
}

// NdcDepthToViewDepth
float N2V(float ndcDepth, matrix invProj)
{
    float4 pointView = mul(float4(0, 0, ndcDepth, 1), invProj);
    return pointView.z / pointView.w;
}

#define NEAR_PLANE 0.1
#define LIGHT_FRUSTUM_WIDTH 0.34641 //<- 계산으로 찾은 값

float PCF_Filter(float2 uv, float zReceiverNdc, float filterRadiusUV, Texture2D shadowMap)
{
    float sum = 0.0f;
    for (int i = 0; i < 128; ++i)
    {
        float2 offset = diskSamples128[i] * filterRadiusUV;
        sum += shadowMap.SampleCmpLevelZero(shadowCompareSampler, uv + offset, zReceiverNdc);
    }
    
    return sum / 128;
}

void FindBlocker(out float avgBlockerDepthView, out float numBlockers, float2 uv,
                 float zReceiverView, Texture2D shadowMap, matrix invProj, float lightRadiusWorld)
{
    float lightRadiusUV = lightRadiusWorld / LIGHT_FRUSTUM_WIDTH;
    float searchRadius = lightRadiusUV * (zReceiverView - NEAR_PLANE) / zReceiverView;
    
    float blockerSum = 0.0;
    numBlockers = 0;
    
    for (int i = 0; i < 128; ++i)
    {
        float shadowMapDepth = shadowMap.Sample(shadowPointSampler, uv + diskSamples128[i] * searchRadius).r;

        // Ndc Depth -> View Depth
        shadowMapDepth = N2V(shadowMapDepth, invProj);
        
        if (shadowMapDepth < zReceiverView)
        {
            blockerSum += shadowMapDepth;
            numBlockers++;            
        }
    }
    
    avgBlockerDepthView = blockerSum / numBlockers;
}

float PCSS(float2 uv, float zReceiverNdc, Texture2D shadowMap, matrix invProj, float lightRadiusWorld)
{
    float lightRadiusUV = lightRadiusWorld / LIGHT_FRUSTUM_WIDTH;
    float zReceiverView = N2V(zReceiverNdc, invProj);

    // STEP 1 : blocker Search
    float avgBlockerDepthView = 0;
    float numBlockers = 0;
    
    FindBlocker(avgBlockerDepthView, numBlockers, uv, zReceiverView, shadowMap, invProj, lightRadiusWorld);
    
    if (numBlockers < 1)
    {
        // There are no occluders so early out
        return 1.0;
    }
    else
    {
        // STEP 2 : penumbra size
        float penumbraRatio = (zReceiverView - avgBlockerDepthView) / avgBlockerDepthView;
        float filterRadiusUV = penumbraRatio * lightRadiusUV * (NEAR_PLANE / zReceiverView);

        // STEP 3 : filtering
        return PCF_Filter(uv, zReceiverNdc, filterRadiusUV, shadowMap);
    }
}

float3 LightRadiance(Light light, float3 representativePoint, float3 posWorld, float3 normalWorld, Texture2D shadowMap)
{
    // Directional light
    float3 lightVec = light.type & LIGHT_DIRECTIONAL
                        ? -light.direction 
                        : representativePoint - posWorld;
    
    float lightDist = length(lightVec);
    lightVec /= lightDist;

    // Spot light
    float spotFactor = light.type & LIGHT_SPOT
                        ? pow(max(-dot(lightVec, light.direction), 0.0), light.spotPower)
                        : 1.0;
    
    // Distance attenuation
    float att = saturate((light.fallOffEnd - lightDist) / (light.fallOffEnd - light.fallOffStart));
    
    // Shadow map
    float shadowFactor = 1.0;
    
    if (light.type & LIGHT_SHADOW)
    {
        const float nearZ = 0.01; // 카메라 설정과 동일
        
        // 1. Project posWorld to light screen
        // light.viewProj 사용
        float4 lightScreen = mul(float4(posWorld, 1.0), light.viewProj);
        lightScreen.xyz /= lightScreen.w;
        
        // 2. 카메라(=광원)에서 볼 때의 텍스쳐 좌표 계산
        // [-1, 1]x[-1, 1] -> [0, 1]x[0, 1]
        // 텍스쳐 좌표와 NDC는 y가 반대
        float2 lightTexcoord = float2(lightScreen.x, -lightScreen.y);
        lightTexcoord += 1.0;
        lightTexcoord *= 0.5;
        
        // 3. 쉐도우맵에서 값 가져오기
        //float depth = shadowMap.Sample(shadowPointSampler, lightTexcoord).r;

        // 4. 가려져 있다면 그림자로 표시
        //if (lightScreen.z > depth + 1e-3)
        //{
        //    shadowFactor = 0.0f;
        //}
        
        uint width, height, numMips;
        shadowMap.GetDimensions(0, width, height, numMips);
       
        //float dx = 5.0 / (float) width;
        //shadowFactor = PCF_Filter(lightTexcoord.xy, lightScreen.z - 0.001, dx, shadowMap);
        shadowFactor = PCSS(lightTexcoord.xy, lightScreen.z - 0.001, shadowMap, light.invProj, light.radius);
    }
    
    float radiance = light.radiance * spotFactor * att * shadowFactor;
    
    return radiance;
}

PixelShaderOutput main(PixelShaderInput input) {
    
    if (isMirror && dot(input.posWorld.xyz, mirrorPlane.xyz) + mirrorPlane.w < 0.0)
    {
        clip(-1.0);
    }
    
    float3 pixelToEye = normalize(eyeWorld - input.posWorld);
    float3 normalWorld = GetNormal(input);
    
    float3 albedo = useAlbedoMap ? albedoTex.SampleLevel(linearWrapSampler, input.texcoord, lodBias).rgb * albedoFactor : albedoFactor;
    float ao = useAOMap ? aoTex.SampleLevel(linearWrapSampler, input.texcoord, lodBias).r : 1.0f;
    float metallic = useMetallicMap ? metallicRoughnessTex.SampleLevel(linearWrapSampler, input.texcoord, lodBias).b * metallicFactor : metallicFactor;
    float roughness = useRoughnessMap ? metallicRoughnessTex.SampleLevel(linearWrapSampler, input.texcoord, lodBias).g * roughnessFactor : roughnessFactor;
    float3 emission = useEmissiveMap ? emissiveTex.SampleLevel(linearWrapSampler, input.texcoord, lodBias).rgb : emissionFactor;
    
    // adjust 
    ao = 0.3 + 0.7 * ao;
    emission *= 4.0;
    
    float3 ambientLighting = AmbientLightingByIBL(albedo, normalWorld, pixelToEye, ao, metallic, roughness) * strengthIBL;
    
    float3 directLighting = float3(0, 0, 0);
    
    /*float dist = length(eyeWorld - input.posWorld);
    float distMin = 5.0;
    float distMax = 20.0;
    float lod = 10.0 * saturate((dist - distMin) / (distMax - distMin));*/

    // 임시로 unroll 사용
    [unroll] // warning X3550: sampler array index must be a literal expression, forcing loop to unroll
    for (int i = 0; i < MAX_LIGHTS; ++i)
    {
        if (lights[i].type)
        {
            float3 L = lights[i].position - input.posWorld;
            float3 r = normalize(reflect(eyeWorld - input.posWorld, normalWorld));
            float3 centerToRay = dot(L, r) * r - L;
            float3 representativePoint = L + centerToRay * clamp(lights[i].radius / length(centerToRay), 0.0, 1.0);
            representativePoint += input.posWorld;
            
            float3 lightVec = representativePoint - input.posWorld;
            //float3 lightVec = lights[i].position - input.posWorld;
            
            float lightDist = length(lightVec);
            lightVec /= lightDist;
            float3 halfway = normalize(pixelToEye + lightVec);
            float NdotI = max(0.0, dot(normalWorld, lightVec));
            float NdotO = max(0.0, dot(normalWorld, pixelToEye));
            float NdotH = max(0.0, dot(normalWorld, halfway));
            
            const float3 Fdielectric = 0.04; // 비금속(Dielectric) 재질의 F0
            float3 F0 = lerp(Fdielectric, albedo, metallic);
            float3 F = SchlickFresnel(F0, max(0.0, dot(halfway, pixelToEye)));
            float3 kd = lerp(float3(1, 1, 1) - F, float3(0, 0, 0), metallic);
            float3 diffuseBRDF = kd * albedo;
            
            float alpha = roughness * roughness;
            float alphaPrime = saturate(alpha + lights[i].radius / (2.0 * lightDist));
        
            float D = NdfGGX(NdotH, roughness, alphaPrime);
            float3 G = SchlickGGX(NdotI, NdotO, roughness);
            // Note (2), 0으로 나누기 방지
            float3 specularBRDF = (F * D * G) / max(1e-5, 4.0 * NdotI * NdotO);
            
            float3 radiance = 0.0f;
            
            radiance = LightRadiance(lights[i], representativePoint, input.posWorld, normalWorld, shadowMaps[i]);
            
            if (abs(dot(float3(1, 1, 1), radiance)) > 1e-5) // <- radiance가 (0, 0, 0)일 경우 더하지 않음.
                directLighting += (diffuseBRDF + specularBRDF) * radiance * NdotI;
        }
    }
    
    /*[unroll]
    for (i = NUM_DIR_LIGHTS + NUM_POINT_LIGHTS; i < NUM_DIR_LIGHTS + NUM_POINT_LIGHTS + NUM_SPOT_LIGHTS; ++i)
    {
        color += ComputeSpotLight(lights[i], material, input.posWorld, normalWorld, toEye);
    }*/
        
    PixelShaderOutput output;
    output.pixelColor = float4(ambientLighting + directLighting + emission, 1.0);
        
    // Rim Lighting
    if (isSelected)
    {
        float rim = 1.0 - dot(pixelToEye, normalWorld);
        rim = smoothstep(0.0, 1.0, rim);
        rim = pow(abs(rim), 10.0);
        float3 rimEffect = rim * float3(0.0, 1.0, 1.0) * 10.0;
        output.pixelColor += float4(rimEffect, 1.0);
    }
    
    output.pixelColor = clamp(output.pixelColor, 0.0, 1000.0);
    
    return output;
}