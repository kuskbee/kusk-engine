#include "Common.hlsli"

Texture2D g_texture0 : register(t0);
TextureCube g_textureCube : register(t1);
SamplerState g_sampler : register(s0);

cbuffer BasicPixelConstantBuffer : register(b0) 
{
    float3 eyeWorld;
    bool useTexture;
    Material material;
    Light lights[MAX_LIGHTS];
    float3 rimColor;
    float rimPower;
    float rimStrength;
    bool useSmoothstep;
};


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
    
    // Rim Lighting
    float rim = 1.0 - dot(toEye, input.normalWorld);
    
    if(useSmoothstep)
        rim = smoothstep(0.0, 1.0, rim);
    rim = pow(abs(rim), rimPower);
    
    color += rim * rimColor * rimStrength;
    
    // sphere mapping 시 사용
    //float2 uv;
    //uv.x = atan2(input.posModel.z, input.posModel.x) / (3.141592 * 2.0) + 0.5;
    //uv.y = acos(input.posModel.y / 1.5) / 3.141592;
    //return useTexture ? float4(color, 1.0) * g_texture0.Sample(g_sampler, uv) : float4(color, 1.0);
    
    // reflect(광선이 들어오는 방향, 노멀 벡터)
    float4 reflectedColor = g_textureCube.Sample(g_sampler, input.normalWorld.xyz);
    float4 reflectedColor2 = g_textureCube.Sample(g_sampler, reflect(-toEye, input.normalWorld));
    
    return useTexture ? reflectedColor2 + g_texture0.Sample(g_sampler, input.texcoord) : reflectedColor2;
    //return useTexture ? reflectedColor + g_texture0.Sample(g_sampler, input.texcoord) : reflectedColor;
    //return useTexture ? float4(color, 1.0) * g_texture0.Sample(g_sampler, input.texcoord) : float4(color, 1.0);
}