Texture2D g_texture0 : register(t0);
SamplerState g_sampler : register(s0);

static const float weights[5] = { 0.0545, 0.2442, 0.4026, 0.2442, 0.0545 };

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
    float4 position : SV_Position;
    float2 texcoord : TEXCOORD;
};

float4 main(SamplingPixelShaderInput input) : SV_Target
{
    // Compute Shader X
    float3 color = float3(0.0, 0.0, 0.0);
    
    int i;
    for (i = 0; i < 5; i++)
    {
        color += weights[i] * g_texture0.Sample(g_sampler, input.texcoord + float2(0.0, dy) * float(i - 2)).rgb;
    }

    return float4(color, 1.0);
}