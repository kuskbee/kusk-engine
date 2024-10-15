#include "Common.hlsli"

cbuffer BasicVertexConstantBuffer : register(b0)
{
    matrix model;
    matrix invTranspose;
    matrix view;
    matrix projection;
};

PixelShaderInput main(VertexShaderInput input)
{
    // ���ʿ��� ����鵵 VertexShaderInput�� ���Ͻ��ױ� ������ ä���ֱ�.
    PixelShaderInput output;
    output.posModel = input.posModel;
    float4 pos = float4(input.posModel, 1.0f);
    pos = mul(pos, model); // Identity
    output.posWorld = pos.xyz;
    
    float4 normal = float4(input.normalModel, 0.0f);
    output.normalWorld = mul(normal, invTranspose);
    output.normalWorld = normalize(output.normalWorld);
    
    pos = mul(pos, view);
    pos = mul(pos, projection);
    output.posProj = pos;
    
    output.texcoord = input.texcoord;
    output.color = float3(1.0f, 1.0f, 0.0);

    return output;
}
