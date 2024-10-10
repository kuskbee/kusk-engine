cbuffer PixelShaderConstantBuffer : register(b0)
{
    float xSplit;
};

struct PixelShaderInput
{
    float4 pos : SV_POSITION;
    float3 color : COLOR;
    float2 uv : TEXCOORD;
};

float4 main(PixelShaderInput input) : SV_TARGET {
    
    if (input.uv.x < xSplit) {
        input.color = float3(1.0f, 0.0f, 0.0f);
    }
    
    // Use the interpolated vertex color
    return float4(input.color, 1.0);
}