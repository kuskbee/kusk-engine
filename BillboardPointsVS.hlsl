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
    float4 pos : POSITION; // 모델 좌표계의 위치 position
};

struct BillboardGeometryShaderInput
{
    float4 pos : SV_POSITION; // Screen position
};

BillboardGeometryShaderInput main(BillboardVertexShaderInput input)
{
    BillboardGeometryShaderInput output;
    
    // Geometry Shader로 그대로 넘겨줌.
    output.pos = input.pos;
    
    return output;
}