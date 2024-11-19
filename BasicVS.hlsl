#include "Common.hlsli" // ���̴������� include ��� ����

// Vertex Shader������ �ؽ��� ���
Texture2D g_heightTexture : register(t0);

cbuffer MeshConstants : register(b0)
{
    matrix world;   // Model(�Ǵ� Object) ��ǥ�� -> World�� ��ȯ
    matrix worldIT; // World�� InverseTranspose
    int useHeightMap;
    float heightScale;
    float2 dummy;
};

PixelShaderInput main(VertexShaderInput input)
{
    PixelShaderInput output;
    
    // Normal ���� ���� ��ȯ (Height Mapping)
    float4 normal = float4(input.normalModel, 0.0);
    output.normalWorld = mul(normal, worldIT).xyz;
    output.normalWorld = normalize(output.normalWorld);
    
    // Tangent ���ʹ� world�� ��ȯ
    float4 tangentWorld = float4(input.tangentModel, 0.0);
    tangentWorld = mul(tangentWorld, world);
    
    output.posModel = input.posModel;
    float4 pos = float4(input.posModel, 1.0f);
    pos = mul(pos, world);
    
    if(useHeightMap)
    {
        float height = g_heightTexture.SampleLevel(linearClampSampler, input.texcoord, 0).r;
        height = height * 2.0 - 1.0;
        pos += float4(output.normalWorld * height * heightScale, 0.0);
    }
    
    output.posWorld = pos.xyz;
    
    pos = mul(pos, viewProj);
    
    output.posProj = pos;
    output.texcoord = input.texcoord;
    output.tangentWorld = tangentWorld.xyz;
    
    return output;
}