Texture2D g_texture0 : register(t0);
Texture2D g_texture1 : register(t1);
SamplerState g_sampler : register(s0);

cbuffer PixelShaderConstantBuffer : register(b0) { float xSplit; };

struct PixelShaderInput
{
    float4 pos : SV_POSITION;
    float3 color : COLOR;
    float2 texcoord : TEXCOORD;
};

float4 main(PixelShaderInput input) : SV_TARGET {
    
    //1
    //return input.texcoord.x > xSplit ?
    //    g_texture0.Sample(g_sampler, input.texcoord)
    //    : float4(1.0, 0.0, 0.0, 1.0);
    
    //2
    //return g_texture0.Sample(g_sampler, input.texcoord * 10.0);
    
    //3
    //input.texcoord.x -= xSplit;
    
    //4
    //float dis = length(input.texcoord - float2(0.5f, 0.5f));
    //if (dis <= xSplit)
    //    return float4(1.0, 0.0, 0.0, 1.0);
    //return g_texture0.Sample(g_sampler, input.texcoord);
    
    if (input.texcoord.x < xSplit) {
        return g_texture1.Sample(g_sampler, input.texcoord);
    }
    
    // Use the interpolated vertex color
    return g_texture0.Sample(g_sampler, input.texcoord);
}