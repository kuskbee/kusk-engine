Texture2D g_texture0 : register(t0);
SamplerState g_sampler : register(s0);

cbuffer SamplingPixelConstantData : register(b0)
{
    float dx;
    float dy;
    float threshold;
    float strength;
    float4 options;
};

struct SamplingPixelShaderInput
{
    float4 positon : SV_POSITION;
    float2 texcoord : TEXCOORD;
};

float4 main (SamplingPixelShaderInput input) : SV_TARGET
{
    float3 color = g_texture0.Sample(g_sampler, input.texcoord).rgb;
    float lumi = (color.r + color.g + color.b) / 3;
    if (lumi > threshold)
    {
        return float4(color, 1.0);
    }
    else
    {
        return float4(0.0, 0.0, 0.0, 0.0);
    }
}