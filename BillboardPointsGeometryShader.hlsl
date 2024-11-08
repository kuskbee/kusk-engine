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
    uint primID : SV_PrimitiveID;
};

[maxvertexcount(4)] // �ִ� ��� Vertex ����
void main(point GeometryShaderInput input[1], uint primID : SV_PrimitiveID,
                              inout TriangleStream<PixelShaderInput> outputStream)
{
    float hw = 0.5 * width;
    
    PixelShaderInput output;
    
    // Triangle Strips ������ ����
    output.pos = input[0].pos + float4(-hw, -hw, 0.0, 0.0);
    output.pos = mul(output.pos, view);
    output.pos = mul(output.pos, proj);
    output.primID = primID;
    outputStream.Append(output);
    
    output.pos = input[0].pos + float4(-hw, hw, 0.0, 0.0);
    output.pos = mul(output.pos, view);
    output.pos = mul(output.pos, proj);
    output.primID = primID;
    outputStream.Append(output);
    
    output.pos = input[0].pos + float4(hw, -hw, 0.0, 0.0);
    output.pos = mul(output.pos, view);
    output.pos = mul(output.pos, proj);
    output.primID = primID;
    outputStream.Append(output);
    
    output.pos = input[0].pos + float4(hw, hw, 0.0, 0.0);
    output.pos = mul(output.pos, view);
    output.pos = mul(output.pos, proj);
    output.primID = primID;
    outputStream.Append(output);
    
    // outputStream.RestartStrip(); // Strip�� �ٽ� ����
}