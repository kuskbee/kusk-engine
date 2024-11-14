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
    
    // Take 13 samples around current texel:
    // a - b - c
    // - j - k -
    // d - e - f
    // - l - m -
    // g - h - i
    // === ('e' is the current texel) ===
    float3 a = g_texture0.Sample(g_sampler, float2(x - 2 * dx, y + 2 * dy)).rgb;
    float3 b = g_texture0.Sample(g_sampler, float2(x, y + 2 * dy)).rgb;
    float3 c = g_texture0.Sample(g_sampler, float2(x + 2 * dx, y + 2 * dy)).rgb;
    float3 d = g_texture0.Sample(g_sampler, float2(x - 2 * dx, y)).rgb;
    float3 e = g_texture0.Sample(g_sampler, float2(x, y)).rgb;
    float3 f = g_texture0.Sample(g_sampler, float2(x + 2 * dx, y)).rgb;
    float3 g = g_texture0.Sample(g_sampler, float2(x - 2 * dx, y - 2 * dy)).rgb;
    float3 h = g_texture0.Sample(g_sampler, float2(x, y - 2 * dy)).rgb;
    float3 i = g_texture0.Sample(g_sampler, float2(x + 2 * dx, y - 2 * dy)).rgb;
    float3 j = g_texture0.Sample(g_sampler, float2(x - dx, y + dy)).rgb;
    float3 k = g_texture0.Sample(g_sampler, float2(x + dx, y + dy)).rgb;
    float3 l = g_texture0.Sample(g_sampler, float2(x - dx, y - dy)).rgb;
    float3 m = g_texture0.Sample(g_sampler, float2(x + dx, y - dy)).rgb;
    
    // Apply weighted distribution:
    // 0.5 + 0.125 + 0.125 + 0.125 + 0.125 = 1
    // a,b,d,e * 0.125
    // b,c,e,f * 0.125
    // d,e,g,h * 0.125
    // e,f,h,i * 0.125
    // j,k,l,m * 0.5
    // This shows 5 square areas that are being sampled. But some of them overlap,
    // so to have an energy preserving downsample we need to make some adjustments.
    // The weights are the distributed, so that the sum of j,k,l,m (e.g.)
    // contribute 0.5 to the final color output. The code below is written
    // to effectively yield this sum. We get:
    // 0.125*5 + 0.03125*4 + 0.0625*4 = 1
    float3 downsample = e * 0.125;
    downsample += (a + c + g + i) * 0.03125;
    downsample += (b + d + f + h) * 0.0625;
    downsample += (j + k + l + m) * 0.125;
    
    return float4(downsample, 1.0);
}