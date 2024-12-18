#include "Common.hlsli"

struct SkyboxPixelShaderInput
{
    float4 posProj : SV_POSITION;
    float3 posModel : POSITION;
};

struct PixelShaderOutput
{
    float4 pixelColor : SV_Target0;
};

PixelShaderOutput main(SkyboxPixelShaderInput input) : SV_Target
{
    PixelShaderOutput output;
    if(textureToDraw == 0)
    {
        output.pixelColor = envIBLTex.SampleLevel(linearWrapSampler, input.posModel.xyz, envLodBias);
    }
    else if (textureToDraw == 1)
    {
        output.pixelColor = specularIBLTex.SampleLevel(linearWrapSampler, input.posModel.xyz, envLodBias);
    }
    else if (textureToDraw == 2)
    {
        output.pixelColor = irradianceIBLTex.SampleLevel(linearWrapSampler, input.posModel.xyz, envLodBias);
    }
    else
    {
        output.pixelColor = float4(135 / 255, 206 / 255, 235 / 255, 1);
    }
    
    output.pixelColor *= strengthIBL;
    output.pixelColor = float4(output.pixelColor.xyz, 1.0);
    output.pixelColor = clamp(output.pixelColor, 0.0, 1000.0);
    
    return output;
}