#include "Common.hlsli"

struct GeometryShaderInput
{
    float4 posModel : SV_POSITION;
    float3 normalModel : NORMAL; // �� ��ǥ���� normal
};

GeometryShaderInput main(VertexShaderInput input)
{
    GeometryShaderInput output;
    output.posModel = float4(input.posModel, 1.0);
    output.normalModel = input.normalModel;
    
	return output;
}