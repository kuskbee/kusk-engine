#include "Common.hlsli" // 쉐이더에서도 include 사용 가능

// Vertex Shader에서도 텍스쳐 사용
Texture2D g_heightTexture : register(t0);
SamplerState g_sampler : register(s0);

cbuffer BasicVertexConstantData : register(b0)
{
    matrix modelWorld;
    matrix invTranspose;
    matrix view;
    matrix projection;
    int useHeightMap;
    float heightScale;
    float2 dummy;
};

PixelShaderInput main(VertexShaderInput input)
{
    PixelShaderInput output;
    
    // Normal 벡터 먼저 변환 (Height Mapping)
    float4 normal = float4(input.normalModel, 0.0);
    output.normalWorld = mul(normal, invTranspose).xyz;
    output.normalWorld = normalize(output.normalWorld);
    
    // Tangent 벡터는 modelWorld로 변환
    float4 tangentWorld = float4(input.tangentModel, 0.0);
    tangentWorld = mul(tangentWorld, modelWorld);
    
    output.posModel = input.posModel;
    float4 pos = float4(input.posModel, 1.0f);
    pos = mul(pos, modelWorld);
    
    if(useHeightMap)
    {
        float height = g_heightTexture.SampleLevel(g_sampler, input.texcoord, 0).r;
        height = height * 2.0 - 1.0;
        pos += float4(output.normalWorld * height * heightScale, 0.0);
    }
    
    output.posWorld = pos.xyz;
    
    pos = mul(pos, view);
    pos = mul(pos, projection);
    
    output.posProj = pos;
    output.texcoord = input.texcoord;
    output.tangentWorld = tangentWorld.xyz;
    
    output.color = float3(0.0f, 0.0f, 0.0f);
    
    return output;
}