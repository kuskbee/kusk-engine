#include "Common.hlsli"

TextureCube g_diffuseCube : register(t0);
TextureCube g_specularCube : register(t1);

Texture2D g_albedoTexture : register(t2);
Texture2D g_normalTexture : register(t3);
Texture2D g_aoTexture : register(t4);

SamplerState g_sampler : register(s0);

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
    int useAlbedoTexture;
    int useNormalMap;
    int useAOMap;       // Ambient Occlusion
    int reverseNormalMapY;
};

float3 SchlickFresnel(float3 fresnelR0, float3 normal, float3 toEye)
{
    float normalDotView = saturate(dot(normal, toEye));
    float f0 = 1.0f - normalDotView; // 90도이면 f0 = 1, 0도이면 f0 = 0
    
    return fresnelR0 + (1.0f - fresnelR0) * pow(f0, 5.0f);
}

float4 main(PixelShaderInput input) : SV_TARGET {
    
    float3 toEye = normalize(eyeWorld - input.posWorld);
    
    float dist = length(eyeWorld - input.posWorld);
    float distMin = 5.0;
    float distMax = 20.0;
    float lod = 10.0 * saturate((dist - distMin) / (distMax - distMin));
        
    float3 color = float3(0.0, 0.0, 0.0);
    
    int i = 0;
    
    float3 normalWorld = input.normalWorld;
    if(useNormalMap) // NormalWorld를 교체
    {
        float3 normalTex = g_normalTexture.SampleLevel(g_sampler, input.texcoord, lod).rgb;
        normalTex = 2.0 * normalTex - 1.0; // 범위 조절 [0.0, 1.0] => [-1.0, 1.0]
        
        // OpenGL용 노멀맵인 경우 y 방향을 뒤집어주기
        normalTex.y = reverseNormalMapY ? -normalTex.y : normalTex.y;
        
        float3 N = normalWorld; // normal
        float3 T = normalize(input.tangentWorld - dot(input.tangentWorld, N) * N); // tangent
        float3 B = cross(N, T);
        
        // matrix는 float4x4, 여기서 벡터 변환용이라서 3x3 사용
        float3x3 TBN = float3x3(T, B, N);
        normalWorld = normalize(mul(normalTex, TBN));
    }
    
    [unroll]
    for (i = 0; i < NUM_DIR_LIGHTS; ++i)
    {
        color += ComputeDirectionalLight(lights[i], material, normalWorld, toEye);
    }

    [unroll]
    for (i = NUM_DIR_LIGHTS; i < NUM_DIR_LIGHTS + NUM_POINT_LIGHTS; ++i)
    {
        color += ComputePointLight(lights[i], material, input.posWorld, normalWorld, toEye);
    }
    
    [unroll]
    for (i = NUM_DIR_LIGHTS + NUM_POINT_LIGHTS; i < NUM_DIR_LIGHTS + NUM_POINT_LIGHTS + NUM_SPOT_LIGHTS; ++i)
    {
        color += ComputeSpotLight(lights[i], material, input.posWorld, normalWorld, toEye);
    }
    
    // sphere mapping 시 사용
    //float2 uv;
    //uv.x = atan2(input.posModel.z, input.posModel.x) / (3.141592 * 2.0) + 0.5;
    //uv.y = acos(input.posModel.y / 1.5) / 3.141592;
    //return useAlbedoTexture ? float4(color, 1.0) * g_texture0.Sample(g_sampler, uv) : float4(color, 1.0);
    
    // IBL
    float4 diffuse = g_diffuseCube.Sample(g_sampler, normalWorld) + float4(color, 1.0);
    
    float3 reflected = reflect(-toEye, normalWorld);
    float4 specular = g_specularCube.Sample(g_sampler, reflected);
    
    diffuse *= float4(material.diffuse, 1.0f);
    specular *= pow(abs(specular.r + specular.g + specular.b) / 3.0, material.shininess);
    specular *= float4(material.specular, 1.0f);
    
    float3 f = SchlickFresnel(material.fresnelR0, normalWorld, toEye);
    specular.xyz *= f;
    
    if (useAlbedoTexture)
    {
        diffuse *= g_albedoTexture.SampleLevel(g_sampler, input.texcoord, lod);
        //diffuse *= g_albedoTexture.SampleLevel(g_sampler, input.texcoord, 0.0); // 가장 높은 해상도로 테스트
        //diffuse *= g_albedoTexture.Sample(g_sampler, input.texcoord);
    }
    
    if(useAOMap)
    {
        diffuse *= g_aoTexture.SampleLevel(g_sampler, input.texcoord, lod);
    }
    
    float4 finalColor = diffuse + specular;
    
    // Rim Lighting
    float rim = 1.0 - dot(toEye, normalWorld);
    
    if (useSmoothstep)
        rim = smoothstep(0.0, 1.0, rim);
    rim = pow(abs(rim), rimPower);
    
    float3 rimEffect = rim * rimColor * rimStrength;
    
    return finalColor + float4(rimEffect, 1.0);
    //return useTexture ? float4(color, 1.0) * g_texture0.Sample(g_sampler, input.texcoord) : float4(color, 1.0);
}