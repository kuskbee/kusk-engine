#include "Common.hlsli"

Texture2DArray g_texArray : register(t0);

cbuffer BillboardPointsConstantData : register(b0)
{
    float3 albedoFactor;
    float roughnessFactor;
    float metallicFactor;
    float width;
    int textureCnt;
}

struct BillboardPixelShaderInput
{
    float4 pos : SV_POSITION; // Screen Position
    float4 posWorld : POSITION0;
    float4 center : POSITION1;
    float2 texCoord : TEXCOORD;
    uint primID : SV_PrimitiveID;
};

struct BillboardPixelShaderOutput
{
    float4 pixelColor : SV_Target0;
};

BillboardPixelShaderOutput main(BillboardPixelShaderInput input)
{
    float3 uvw = float3(input.texCoord, float(input.primID % textureCnt));
    //float4 pixelColor = g_texArray.SampleLevel(g_sampler, uvw, 4);
    float4 pixelColor = g_texArray.Sample(linearWrapSampler, uvw);
    
    // 1. alpha 값이 있는 이미지에서 불투명도가 0.9보다 작으면 clip
    // 2. 픽셀의 값이 흰색에 가까운 배경 색이면 clip
    clip((pixelColor.a < 0.9f) || (pixelColor.r + pixelColor.g + pixelColor.b) > 2.4 ? -1 : 1);
    
    float3 pixelToEye = normalize(eyeWorld - input.posWorld.xyz);
    float3 albedo = pixelColor.rgb * albedoFactor;
    float3 ambientLighting = AmbientLightingByIBL(albedo, pixelToEye, pixelToEye, 1.0f, metallicFactor, roughnessFactor) * strengthIBL;
    
    BillboardPixelShaderOutput output;
    output.pixelColor = float4(ambientLighting, 1.0);
    return output;
}
