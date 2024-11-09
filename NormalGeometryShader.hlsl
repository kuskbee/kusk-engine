cbuffer GeometryConstantData : register(b0)
{
    Matrix modelWorld;
    Matrix invTranspose;
    Matrix view;
    Matrix proj;
    float scale;
}

struct GeometryShaderInput
{
    float4 posModel : SV_POSITION;
    float3 normalModel : NORMAL; // ¸ðµ¨ ÁÂÇ¥°èÀÇ normal
};

struct PixelShaderInput
{
    float4 pos : SV_POSITION;
    //uint primID : SV_PrimitiveID;
    float3 color : COLOR;
};

[maxvertexcount(2)]
void main(point GeometryShaderInput input[1], uint primID : SV_PrimitiveID,
                              inout LineStream<PixelShaderInput> outputStream)
{
    PixelShaderInput output;
    
    output.pos = input[0].posModel;
    output.pos = mul(output.pos, modelWorld);
    output.pos = mul(output.pos, view);
    output.pos = mul(output.pos, proj);
    output.color = float3(1.0, 1.0, 0.0);
    outputStream.Append(output);
    
    float4 normalModel = float4(input[0].normalModel, 0.0);
    float3 normalWorld = mul(normalModel, invTranspose).xyz;
    normalWorld = normalize(normalWorld);
    
    output.pos = input[0].posModel;
    output.pos = mul(output.pos, modelWorld);
    output.pos.xyz += normalWorld * scale;
    
    output.pos = mul(output.pos, view);
    output.pos = mul(output.pos, proj);
    output.color = float3(1.0, 0.0, 0.0);
    outputStream.Append(output);
}