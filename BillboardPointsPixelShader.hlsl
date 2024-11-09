Texture2DArray g_texArray : register(t0);
SamplerState g_sampler : register(s0);

struct PixelShaderInput
{
    float4 posProj : SV_POSITION; // Screen Position
    float2 texCoord : TEXCOORD;
    uint primID : SV_PrimitiveID;
};

struct PixelShaderOutput
{
    float4 pixelColor : SV_Target0;
};

PixelShaderOutput main(PixelShaderInput input)
{
    float3 uvw = float3(input.texCoord, float(input.primID % 5));
    float4 pixelColor = g_texArray.Sample(g_sampler, uvw);
    
    // 1. alpha 값이 있는 이미지에서 불투명도가 0.9보다 작으면 clip
    // 2. 픽셀의 값이 흰색에 가까운 배경 색이면 clip
    clip((pixelColor.a < 0.9f) || (pixelColor.r + pixelColor.g + pixelColor.b) > 2.4 ? -1 : 1);
    
    PixelShaderOutput output;
    output.pixelColor = pixelColor;
    return output;
}
