TextureCube g_envTex : register(t0);
TextureCube g_specularTex : register(t1);
TextureCube g_irradianceTex : register(t2);
SamplerState g_sampler : register(s0);

cbuffer PixelConstData : register(b0)
{
    int textureToDraw = 0; // 0 : Env, 1 : Specular, 2 : Irradiance
    float mipLevel = 0.0f;
    float dummy1;
    float dummy2;
};

struct CubeMappingPixelShaderInput
{
    float4 posProj : SV_POSITION;
    float3 posModel : POSITION;
};

float4 main(CubeMappingPixelShaderInput input) : SV_Target
{
    float4 output;
    if(textureToDraw == 0)
    {
        output = g_envTex.SampleLevel(g_sampler, input.posModel.xyz, mipLevel);
    }
    else if (textureToDraw == 1)
    {
        output = g_specularTex.SampleLevel(g_sampler, input.posModel.xyz, mipLevel);
    }
    else
    {
        output = g_irradianceTex.SampleLevel(g_sampler, input.posModel.xyz, mipLevel);
    }
    
    return output;
}