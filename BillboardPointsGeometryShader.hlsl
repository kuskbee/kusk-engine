cbuffer BillboardPointsConstantData : register(b0)
{
    float3 eyeWorld;
    float width;
    Matrix modelWorld;
    Matrix view;
    Matrix proj;
};

struct GeometryShaderInput
{
    float4 pos : SV_POSITION;
};

struct PixelShaderInput
{
    float4 pos : SV_POSITION;
    float2 texCoord : TEXCOORD;
    uint primID : SV_PrimitiveID;
};

[maxvertexcount(4)] // 최대 출력 Vertex 갯수
void main(point GeometryShaderInput input[1], uint primID : SV_PrimitiveID,
                              inout TriangleStream<PixelShaderInput> outputStream)
{
    float hw = 0.5 * width;
    
    float4 up = float4(0.0, 1.0, 0.0, 0.0);
    float4 front = float4(eyeWorld, 1.0) - input[0].pos;
    front.w = 0.0;
    float4 right = float4(cross(up.xyz, normalize(front.xyz)), 0.0);
    
    PixelShaderInput output;
    
    // Triangle Strips 순서로 생성
    output.pos = input[0].pos - hw * right - hw * up;
    output.pos = mul(output.pos, view);
    output.pos = mul(output.pos, proj);
    output.texCoord = float2(0.0, 1.0);
    output.primID = primID;
    outputStream.Append(output);
    
    output.pos = input[0].pos - hw * right + hw * up;
    output.pos = mul(output.pos, view);
    output.pos = mul(output.pos, proj);
    output.texCoord = float2(0.0, 0.0);
    output.primID = primID;
    outputStream.Append(output);
    
    output.pos = input[0].pos + hw * right - hw * up;
    output.pos = mul(output.pos, view);
    output.pos = mul(output.pos, proj);
    output.texCoord = float2(1.0, 1.0);
    output.primID = primID;
    outputStream.Append(output);
    
    output.pos = input[0].pos + hw * right + hw * up;
    output.pos = mul(output.pos, view);
    output.pos = mul(output.pos, proj);
    output.texCoord = float2(1.0, 0.0);
    output.primID = primID;
    outputStream.Append(output);
    
    outputStream.RestartStrip(); // Strip을 다시 시작
}