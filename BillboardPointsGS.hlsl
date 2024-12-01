#include "Common.hlsli"

cbuffer BillboardPointsConstantData : register(b0)
{
    float3 albedoFactor;
    float roughnessFactor;
    float metallicFactor;
    float width;
};

struct BillboardGeometryShaderInput
{
    float4 pos : SV_POSITION;
};

struct BillboardPixelShaderInput
{
    float4 pos : SV_POSITION; // Screen positon
    float4 posWorld : POSITION0;
    float4 center : POSITION1;
    float2 texCoord : TEXCOORD;
    uint primID : SV_PrimitiveID;
};

[maxvertexcount(4)] // 최대 출력 Vertex 갯수
void main(point BillboardGeometryShaderInput input[1], uint primID : SV_PrimitiveID,
                              inout TriangleStream<BillboardPixelShaderInput> outputStream)
{
    float hw = 0.5 * width;
    
    float4 up = float4(0.0, 1.0, 0.0, 0.0);
    float4 front = float4(eyeWorld, 1.0) - input[0].pos;
    front.w = 0.0;
    float4 right = float4(cross(up.xyz, normalize(front.xyz)), 0.0);
    
    BillboardPixelShaderInput output;
    
    output.center = input[0].pos; // 빌보드의 중심
    
    // Triangle Strips 순서로 생성
    output.posWorld = input[0].pos - hw * right - hw * up;
    output.pos = output.posWorld;
    output.pos = mul(output.pos, view);
    output.pos = mul(output.pos, proj);
    output.texCoord = float2(0.0, 1.0);
    output.primID = primID;
    outputStream.Append(output);
    
    output.posWorld = input[0].pos + hw * right - hw * up;
    output.pos = output.posWorld;
    output.pos = mul(output.pos, view);
    output.pos = mul(output.pos, proj);
    output.texCoord = float2(1.0, 1.0);
    output.primID = primID;
    outputStream.Append(output);
    
    output.posWorld = input[0].pos - hw * right + hw * up;
    output.pos = output.posWorld;
    output.pos = mul(output.pos, view);
    output.pos = mul(output.pos, proj);
    output.texCoord = float2(0.0, 0.0);
    output.primID = primID;
    outputStream.Append(output);
    
    output.posWorld = input[0].pos + hw * right + hw * up;
    output.pos = output.posWorld;
    output.pos = mul(output.pos, view);
    output.pos = mul(output.pos, proj);
    output.texCoord = float2(1.0, 0.0);
    output.primID = primID;
    outputStream.Append(output);
    
    outputStream.RestartStrip(); // Strip을 다시 시작
}