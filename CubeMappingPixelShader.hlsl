#include "Common.hlsli"

TextureCube g_textureCube0 : register(t0);
SamplerState g_sampler : register(s0);

float4 main(PixelShaderInput input) : SV_Target
{
    // ���� : �ؽ��� ��ǥ�� float3
    return g_textureCube0.Sample(g_sampler, input.posWorld.xyz);
}