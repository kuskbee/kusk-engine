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
    float4 position : SV_Position;
    float2 texcoord : TEXCOORD;
};

float4 main(SamplingPixelShaderInput input) : SV_Target
{
    float x = input.texcoord.x;
    float y = input.texcoord.y;
    
    // Take 9 samples around current texel:
    // a - b - c
    // d - e - f
    // g - h - i
    // === ('e' is the current texel) ===
    float3 a = g_texture0.Sample(g_sampler, float2(x - dx, y + dy)).rgb;
    float3 b = g_texture0.Sample(g_sampler, float2(x, y + dy)).rgb;
    float3 c = g_texture0.Sample(g_sampler, float2(x + dx, y + dy)).rgb;
    float3 d = g_texture0.Sample(g_sampler, float2(x - dx, y)).rgb;
    float3 e = g_texture0.Sample(g_sampler, float2(x, y)).rgb;
    float3 f = g_texture0.Sample(g_sampler, float2(x + dx, y)).rgb;
    float3 g = g_texture0.Sample(g_sampler, float2(x - dx, y - dy)).rgb;
    float3 h = g_texture0.Sample(g_sampler, float2(x, y - dy)).rgb;
    float3 i = g_texture0.Sample(g_sampler, float2(x + dx, y - dy)).rgb;
    
    // Apply weighted distribution, by using a 3x3 tent filter :
    //  1   | 1 2 1 |
    // -- * | 2 4 2 |
    // 16   | 1 2 1 |
    float3 upsample = e * 4.0;
    upsample += (b + d + f + h) * 2.0;
    upsample += (a + c + g + i);
    upsample *= 1.0 / 16.0;
    
    return float4(upsample, 1.0);
}