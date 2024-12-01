#pragma once

#include <directxtk/SimpleMath.h>
#include <vector>

#include "D3D11Utils.h"

namespace kusk{
using DirectX::SimpleMath::Matrix;
using DirectX::SimpleMath::Vector3;
using DirectX::SimpleMath::Vector4;
using std::vector;

struct BillboardPointsConstantData {
	Vector3 albedoFactor = Vector3(0.1f);
	float roughnessFactor = 0.5f;
	float metallicFactor = 0.3f;
	float width;
	int textureCnt;
	float time = 0.0f;
};

class BillboardPoints
{
public:
	void Initialize(ComPtr<ID3D11Device>& device, 
					ComPtr<ID3D11DeviceContext>& context,
					const std::vector<Vector4>& points,
					const float width,
					std::vector<std::string> filenames = {});
	void Render(ComPtr<ID3D11DeviceContext>& context);
	void UpdateVertexBuffer(ComPtr<ID3D11Device>& device);
	void UpdateConstBuffer(ComPtr<ID3D11Device>& device, ComPtr<ID3D11DeviceContext>& context);
public:
	BillboardPointsConstantData m_constantData;

	// 편의상 ConstantBuffer를 하나만 사용
	ComPtr<ID3D11Buffer> m_constantBuffer;

	vector<Vector4> m_points;
	vector<std::string> m_filenames;
protected:
	ComPtr<ID3D11Buffer> m_vertexBuffer;

	uint32_t m_indexCount = 0;

	ComPtr<ID3D11Texture2D> m_texArray;
	ComPtr<ID3D11ShaderResourceView> m_texArraySRV;
};
}



