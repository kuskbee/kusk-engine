#include "Common.hlsli" // 쉐이더에서도 include 사용 가능

cbuffer BasicVertexConstantData : register(b0)
{
    matrix modelWorld;
    matrix invTranspose;
    matrix view;
    matrix projection;
};

PixelShaderInput main(VertexShaderInput input)
{
    PixelShaderInput output;
    output.posModel = input.posModel;
    float4 pos = float4(input.posModel, 1.0f);
    pos = mul(pos, modelWorld);
    
    output.posWorld = pos.xyz;
    
    pos = mul(pos, view);
    pos = mul(pos, projection);
    
    output.posProj = pos;
    output.texcoord = input.texcoord;
    output.color = float3(0.0f, 0.0f, 0.0f); // 다른 쉐이더에서 사용
    
    float4 normal = float4(input.normalModel, 0.0f);
    output.normalWorld = mul(normal, invTranspose).xyz;
    output.normalWorld = normalize(output.normalWorld);
    
    return output;
}