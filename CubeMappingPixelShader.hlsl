#include "Common.hlsli"

TextureCube g_diffuseCube : register(t0);
TextureCube g_specularCube : register(t1);
SamplerState g_sampler : register(s0);

float4 main(PixelShaderInput input) : SV_Target
{
    // ���� : �ؽ��� ��ǥ�� float3
    return g_specularCube.Sample(g_sampler, input.posWorld.xyz);
}