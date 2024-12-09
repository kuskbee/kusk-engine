#include "Common.hlsli"

cbuffer BillboardPointsConstantData : register(b0)
{
    float3 albedoFactor;
    float roughnessFactor;
    float metallicFactor;
    float width;
}

struct BillboardVertexShaderInput
{
    float4 pos : POSITION; // �� ��ǥ���� ��ġ position
};

struct BillboardGeometryShaderInput
{
    float4 pos : SV_POSITION; // Screen position
};

BillboardGeometryShaderInput main(BillboardVertexShaderInput input)
{
    BillboardGeometryShaderInput output;
    
    // Geometry Shader�� �״�� �Ѱ���.
    output.pos = input.pos;
    
    return output;
}