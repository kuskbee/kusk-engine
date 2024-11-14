#include "Common.hlsli"

// �����ڷ�
// https://github.com/Nadrin/PBR/blob/master/data/shaders/hlsl/pbr.hlsl
// https://cdn2.unrealengine.com/Resources/files/2013SiggraphPresentationsNotes-26915738.pdf

TextureCube specularIBLTex : register(t0);
TextureCube irradianceIBLTex : register(t1);
Texture2D brdfTex : register(t2);
Texture2D albedoTex : register(t3);
Texture2D normalTex : register(t4);
Texture2D aoTex : register(t5);
Texture2D metallicTex : register(t6);
Texture2D roughnessTex : register(t7);
Texture2D emissiveTex : register(t8);

SamplerState linearSampler : register(s0);
SamplerState clampSampler : register(s1);

static const float3 Fdielectric = 0.04; // ��ݼ�(Dielectric) ������ F0

cbuffer BasicPixelConstantData : register(b0) 
{
    float3 eyeWorld;
    float mipmapLevel;
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
    float dummy1;
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
    float3 normalWorld = input.normalWorld;
    if (useNormalMap) // NormalWorld�� ��ü
    {
        float3 normal = normalTex.SampleLevel(linearSampler, input.texcoord, 0.0).rgb;
        normal = 2.0 * normal - 1.0; // ���� ���� [0.0, 1.0] => [-1.0, 1.0]
        
        // OpenGL�� ��ָ��� ��� y ������ �������ֱ�
        normal.y = invertNormalMapY ? -normal.y : normal.y;
        
        float3 N = normalWorld; // normal
        float3 T = normalize(input.tangentWorld - dot(input.tangentWorld, N) * N); // tangent
        float3 B = cross(N, T);
        
        // matrix�� float4x4, ���⼭ ���� ��ȯ���̶� 3x3 ���
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
    
    // �տ��� ����ߴ� ����� ����
    float3 irradiance = irradianceIBLTex.Sample(linearSampler, normalWorld).rgb;
    
    return kd * albedo * irradiance;
}

float3 SpecularIBL(float3 albedo, float3 normalWorld, float3 pixelToEye, float metallic, float roughness)
{
    float2 specularBRDF = brdfTex.Sample(clampSampler, float2(dot(normalWorld, pixelToEye), 1.0 - roughness)).rg;
    
    // �տ��� ����ߴ� ����� ����
    float3 specularIrradiance = specularIBLTex.SampleLevel(linearSampler, reflect(-pixelToEye, normalWorld), roughness * 10.0f).rgb;
    const float3 Fdielectric = 0.04; // ��ݼ�(Dielectric) ������ F0
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

// Single term for separable Schilck-GGX below.
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

PixelShaderOutput main(PixelShaderInput input) : SV_TARGET {
    
    float3 pixelToEye = normalize(eyeWorld - input.posWorld);
    float3 normalWorld = GetNormal(input);
    
    float3 albedo = useAlbedoMap ? albedoTex.Sample(linearSampler, input.texcoord).rgb : material.albedo;
    float ao = useAOMap ? aoTex.SampleLevel(linearSampler, input.texcoord, 0.0).r : 1.0f;
    //float metallic = useMetallicMap ? metallicTex.Sample(linearSampler, input.texcoord).r : material.metallic;
    float metallic = useMetallicMap ? metallicTex.Sample(linearSampler, input.texcoord).b : material.metallic;
    //float roughness = useRoughnessMap ? roughnessTex.Sample(linearSampler, input.texcoord).r : material.roughness;
    float roughness = useRoughnessMap ? roughnessTex.Sample(linearSampler, input.texcoord).g : material.roughness;
    float3 emission = useEmissiveMap ? emissiveTex.Sample(linearSampler, input.texcoord).rgb : float3(0, 0, 0);
    
    // adjust 
    ao = 0.3 + 0.7 * ao;
    emission *= 4.0;
    
    float3 ambientLighting = AmbientLightingByIBL(albedo, normalWorld, pixelToEye, ao, metallic, roughness);
    
    float3 directLighting = float3(0, 0, 0);
    
    /*float dist = length(eyeWorld - input.posWorld);
    float distMin = 5.0;
    float distMax = 20.0;
    float lod = 10.0 * saturate((dist - distMin) / (distMax - distMin));*/
    
    int i = 0;

    /*[unroll]
    for (i = 0; i < NUM_DIR_LIGHTS; ++i)
    {
        color += ComputeDirectionalLight(lights[i], material, normalWorld, toEye);
    }*/

    // ����Ʈ ����Ʈ�� ���� ����
    [unroll]
    for (i = NUM_DIR_LIGHTS; i < NUM_DIR_LIGHTS + NUM_POINT_LIGHTS; ++i)
    {
        float3 lightVec = lights[i].position - input.posWorld;
        float3 halfway = normalize(pixelToEye + lightVec);
        float NdotI = max(0.0, dot(normalWorld, lightVec));
        float NdotO = max(0.0, dot(normalWorld, pixelToEye));
        float NdotH = max(0.0, dot(normalWorld, halfway));

        const float3 Fdielectric = 0.04; // ��ݼ�(Dielectric) ������ F0
        float3 F0 = lerp(Fdielectric, albedo, metallic);
        float3 F = SchlickFresnel(F0, max(0.0, dot(halfway, pixelToEye)));
        float3 kd = lerp(float3(1, 1, 1) - F, float3(0, 0, 0), metallic);
        float3 diffuseBRDF = kd * albedo;
        
        float D = NdfGGX(NdotH, roughness);
        float3 G = SchlickGGX(NdotI, NdotO, roughness);
        
        // Note (2), 0���� ������ ����
        float3 specularBRDF = (F * D * G) / max(1e-5, 4.0 * NdotI * NdotO);
        float3 radiance = lights[i].radiance * saturate((lights[i].fallOffEnd - length(lightVec)) / (lights[i].fallOffEnd - lights[i].fallOffStart));
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
    
    // �ӽ÷� ��� �� ǥ��
    //if (input.texcoord.y < 0.2) {
    //    finalColor = float4(float3(1, 1, 1) * floor(input.texcoord.x * 11.0) / 10.0, 1);
    //}
    //else {
    //    finalColor = diffuse + specular;
    //}
    output.pixelColor += float4(rimEffect, 1.0);
    output.pixelColor = clamp(output.pixelColor, 0.0, 1000.0);
    
    return output;
}