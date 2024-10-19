#include "Common.hlsli"

TextureCube g_textureCube : register(t0);
SamplerState g_sampler : register(s0);

struct CubeMappingPixelShaderInput
{
    float4 posProj : SV_POSITION;
    float3 posModel : POSITION;
};

float4 main(CubeMappingPixelShaderInput input) : SV_Target
{
    // ���� : �ؽ��� ��ǥ�� float3
    return g_textureCube.Sample(g_sampler, input.posModel.xyz);
}