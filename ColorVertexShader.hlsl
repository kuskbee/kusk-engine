cbuffer ModelViewProjectionConstantBuffer : register(b0)
{
    matrix model;
    matrix view;
    matrix projection;
};

// Semantics
// https://learn.microsoft.com/en-us/windows/win32/direct3dhlsl/dx-graphics-hlsl-semantics

struct VertexShaderInput
{
    float3 pos : POSITION;
    float3 color : COLOR0;
    float2 uv : TEXCOORD;
};

struct PixelShaderInput
{
    float4 pos : SV_POSITION;
    float3 color : COLOR;
    float2 uv : TEXCOORD;
};

PixelShaderInput main(VertexShaderInput input)
{
    PixelShaderInput output;
    float4 pos = float4(input.pos, 1.0f);
    pos = mul(pos, model);
    pos = mul(pos, view);
    pos = mul(pos, projection);
    
    output.pos = pos;
    output.color = input.color;
    output.uv = input.uv;
    
    return output;
}