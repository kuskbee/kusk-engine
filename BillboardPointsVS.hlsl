cbuffer BillboardPointsConstantData : register(b0)
{
    float3 eyeWorld;
    float width;
    Matrix modelWorld;
    Matrix view;
    Matrix proj;
}

struct VertexShaderInput
{
    float4 pos : POSITION; // 모델 좌표계의 위치 position
};

struct GeometryShaderInput
{
    float4 pos : SV_POSITION; // Screen position
};

GeometryShaderInput main(VertexShaderInput input)
{
    GeometryShaderInput output;
    
    // Geometry Shader로 그대로 넘겨줌.
    output.pos = input.pos;
    
    return output;
}