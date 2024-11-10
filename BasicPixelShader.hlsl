#include "Common.hlsli"

Texture2D g_texture0 : register(t0);
TextureCube g_diffuseCube : register(t1);
TextureCube g_specularCube : register(t2);
SamplerState g_sampler : register(s0);

cbuffer BasicPixelConstantData : register(b0) 
{
    float3 eyeWorld;
    bool useTexture;
    Material material;
    Light lights[MAX_LIGHTS];
    float3 rimColor;
    float rimPower;
    float rimStrength;
    bool useSmoothstep;
    float mipmapLevel;
};

float3 SchlickFresnel(float3 fresnelR0, float3 normal, float3 toEye)
{
    float normalDotView = saturate(dot(normal, toEye));
    float f0 = 1.0f - normalDotView; // 90도이면 f0 = 1, 0도이면 f0 = 0
    
    return fresnelR0 + (1.0f - fresnelR0) * pow(f0, 5.0f);
}

float4 main(PixelShaderInput input) : SV_TARGET {
    
    float3 toEye = normalize(eyeWorld - input.posWorld);
    float3 color = float3(0.0, 0.0, 0.0);
    
    int i = 0;
    
    [unroll]
    for (i = 0; i < NUM_DIR_LIGHTS; ++i)
    {
        color += ComputeDirectionalLight(lights[i], material, input.normalWorld, toEye);
    }

    [unroll]
    for (i = NUM_DIR_LIGHTS; i < NUM_DIR_LIGHTS + NUM_POINT_LIGHTS; ++i)
    {
        color += ComputePointLight(lights[i], material, input.posWorld, input.normalWorld, toEye);
    }
    
    [unroll]
    for (i = NUM_DIR_LIGHTS + NUM_POINT_LIGHTS; i < NUM_DIR_LIGHTS + NUM_POINT_LIGHTS + NUM_SPOT_LIGHTS; ++i)
    {
        color += ComputeSpotLight(lights[i], material, input.posWorld, input.normalWorld, toEye);
    }
    
    // sphere mapping 시 사용
    //float2 uv;
    //uv.x = atan2(input.posModel.z, input.posModel.x) / (3.141592 * 2.0) + 0.5;
    //uv.y = acos(input.posModel.y / 1.5) / 3.141592;
    //return useTexture ? float4(color, 1.0) * g_texture0.Sample(g_sampler, uv) : float4(color, 1.0);
    
    // IBL
    float4 diffuse = g_diffuseCube.Sample(g_sampler, input.normalWorld);
    
    float3 reflected = reflect(-toEye, input.normalWorld);
    float4 specular = g_specularCube.Sample(g_sampler, reflected);
    
    diffuse *= float4(material.diffuse, 1.0f);
    specular *= pow(abs(specular.r + specular.g + specular.b) / 3.0, material.shininess);
    specular *= float4(material.specular, 1.0f);
    
    float3 f = SchlickFresnel(material.fresnelR0, input.normalWorld, toEye);
    specular.xyz *= f;
    
    if(useTexture)
    {
        float dist = length(eyeWorld - input.posWorld);
        float distMin = 2;
        float distMax = 10;
        float lod = 10.0 * saturate((dist - distMin) / (distMax - distMin));
        diffuse *= g_texture0.SampleLevel(g_sampler, input.texcoord, mipmapLevel);
        //diffuse *= g_texture0.Sample(g_sampler, input.texcoord);
    }
    
    float4 finalColor = float4(color, 1.0) + diffuse + specular;
    
    // Rim Lighting
    float rim = 1.0 - dot(toEye, input.normalWorld);
    
    if (useSmoothstep)
        rim = smoothstep(0.0, 1.0, rim);
    rim = pow(abs(rim), rimPower);
    
    float3 rimEffect = rim * rimColor * rimStrength;
    
    return finalColor + float4(rimEffect, 1.0);
    //return useTexture ? float4(color, 1.0) * g_texture0.Sample(g_sampler, input.texcoord) : float4(color, 1.0);
}