#include "Common.hlsli"

// 참고자료
// https://github.com/Nadrin/PBR/blob/master/data/shaders/hlsl/pbr.hlsl
// https://cdn2.unrealengine.com/Resources/files/2013SiggraphPresentationsNotes-26915738.pdf

TextureCube specularIBLTex : register(t0);
TextureCube irradianceIBLTex : register(t1);
Texture2D brdfTex : register(t2);
Texture2D albedoTex : register(t3);
Texture2D normalTex : register(t4);
Texture2D aoTex : register(t5);
Texture2D metallicRoughnessTex : register(t6);
Texture2D emissiveTex : register(t7);

SamplerState linearSampler : register(s0);
SamplerState clampSampler : register(s1);

static const float3 Fdielectric = 0.04; // 비금속(Dielectric) 재질의 F0

static const float lod = 2.0;

cbuffer BasicPixelConstData : register(b0) 
{
    Material material;
    Light lights[MAX_LIGHTS];
    float3 rimColor;
    float rimPower;
    float rimStrength;
    bool useSmoothstep;
    int useAlbedoMap;
    int useNormalMap;
    int useAOMap;       // Ambient Occlusion
    int invertNormalMapY;
    int useMetallicMap;
    int useRoughnessMap;
    int useEmissiveMap;
    float exposure;
    float gamma;
    float mipmapLevel;
};

cbuffer EyeViewProjConstData : register(b1)
{
    matrix viewProj;
    float3 eyeWorld;
    bool isMirror;
    float4 mirrorPlane;
}

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
    float3 normalWorld = input.normalWorld;
    if (useNormalMap) // NormalWorld를 교체
    {
        float3 normal = normalTex.SampleLevel(linearSampler, input.texcoord, lod).rgb;
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
    float3 irradiance = irradianceIBLTex.SampleLevel(linearSampler, normalWorld, 0).rgb;
    
    return kd * albedo * irradiance;
}

float3 SpecularIBL(float3 albedo, float3 normalWorld, float3 pixelToEye, float metallic, float roughness)
{
    float2 specularBRDF = brdfTex.SampleLevel(clampSampler, float2(dot(normalWorld, pixelToEye), 1.0 - roughness), 0.0).rg;
    
    // 앞에서 사용했던 방법과 동일
    float3 specularIrradiance = specularIBLTex.SampleLevel(linearSampler, reflect(-pixelToEye, normalWorld), 2 + roughness * 5.0f).rgb;
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
float NdfGGX(float NdotH, float roughness)
{
    // Note (3)
    float alpha = roughness * roughness;
    float alphaSq = alpha * alpha;
    float denom = (NdotH * NdotH) * (alphaSq - 1.0) + 1.0;
    
    return alphaSq / (3.141592 * denom * denom);
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

PixelShaderOutput main(PixelShaderInput input) {
    
    if (isMirror && dot(input.posWorld.xyz, mirrorPlane.xyz) + mirrorPlane.w < 0.0)
    {
        clip(-1.0);
    }
    
    float3 pixelToEye = normalize(eyeWorld - input.posWorld);
    float3 normalWorld = GetNormal(input);
    
    float3 albedo = useAlbedoMap ? albedoTex.SampleLevel(linearSampler, input.texcoord, lod).rgb : material.albedo;
    float ao = useAOMap ? aoTex.SampleLevel(linearSampler, input.texcoord, lod).r : 1.0f;
    float metallic = useMetallicMap ? metallicRoughnessTex.SampleLevel(linearSampler, input.texcoord, lod).b : material.metallic;
    float roughness = useRoughnessMap ? metallicRoughnessTex.SampleLevel(linearSampler, input.texcoord, lod).g : material.roughness;
    float3 emission = useEmissiveMap ? emissiveTex.SampleLevel(linearSampler, input.texcoord, lod).rgb : material.emission;
    
    // adjust 
    ao = 0.3 + 0.7 * ao;
    emission *= 4.0;
    
    float3 ambientLighting = AmbientLightingByIBL(albedo, normalWorld, pixelToEye, ao, metallic, roughness);
    
    float3 directLighting = float3(0, 0, 0);
    
    /*float dist = length(eyeWorld - input.posWorld);
    float distMin = 5.0;
    float distMax = 20.0;
    float lod = 10.0 * saturate((dist - distMin) / (distMax - distMin));*/

    /*[unroll]
    for (i = 0; i < NUM_DIR_LIGHTS; ++i)
    {
        color += ComputeDirectionalLight(lights[i], material, normalWorld, toEye);
    }*/

    // 포인트 라이트만 먼저 구현
    [unroll]
    for (int i = NUM_DIR_LIGHTS; i < NUM_DIR_LIGHTS + NUM_POINT_LIGHTS; ++i)
    {
        float3 lightVec = lights[i].position - input.posWorld;
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
        
        float D = NdfGGX(NdotH, roughness);
        float3 G = SchlickGGX(NdotI, NdotO, roughness);
        
        // Note (2), 0으로 나누기 방지
        float3 specularBRDF = (F * D * G) / max(1e-5, 4.0 * NdotI * NdotO);
        float att = saturate((lights[i].fallOffEnd - lightDist) / (lights[i].fallOffEnd - lights[i].fallOffStart));
        float3 radiance = lights[i].radiance * att;
        directLighting += (diffuseBRDF + specularBRDF) * radiance * NdotI;
    }
    
    /*[unroll]
    for (i = NUM_DIR_LIGHTS + NUM_POINT_LIGHTS; i < NUM_DIR_LIGHTS + NUM_POINT_LIGHTS + NUM_SPOT_LIGHTS; ++i)
    {
        color += ComputeSpotLight(lights[i], material, input.posWorld, normalWorld, toEye);
    }*/
        
    PixelShaderOutput output;
    output.pixelColor = float4(ambientLighting + directLighting + emission, 1.0);
        
    // Rim Lighting
    float rim = 1.0 - dot(pixelToEye, normalWorld);
    
    if (useSmoothstep)
        rim = smoothstep(0.0, 1.0, rim);
    rim = pow(abs(rim), rimPower);
    
    float3 rimEffect = rim * rimColor * rimStrength;

    output.pixelColor += float4(rimEffect, 1.0);
    output.pixelColor = clamp(output.pixelColor, 0.0, 1000.0);
    
    return output;
}