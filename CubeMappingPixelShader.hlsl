#include "Common.hlsli"

TextureCube g_textureCube0 : register(t0);
SamplerState g_sampler : register(s0);

float4 main(PixelShaderInput input) : SV_Target
{
    // 주의 : 텍스쳐 좌표가 float3
    return g_textureCube0.Sample(g_sampler, input.posWorld.xyz);
}