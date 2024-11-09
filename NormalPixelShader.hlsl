struct PixelShaderInput
{
    float4 pos : SV_POSITION;
    // uint primID : SV_PrimitiveID;
    float3 color : COLOR;
};

float4 main(PixelShaderInput input) : SV_TARGET
{
	return float4(input.color, 1.0f);
}