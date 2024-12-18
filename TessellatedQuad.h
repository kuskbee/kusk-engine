#pragma once

#include <directxtk/SimpleMath.h>
#include <vector>

#include "D3D11Utils.h"

namespace kusk {
using DirectX::SimpleMath::Matrix;
using DirectX::SimpleMath::Vector2;
using DirectX::SimpleMath::Vector3;
using DirectX::SimpleMath::Vector4;
using std::vector;

struct TessellatedQuadConstantData {
	Vector3 eyeWorld;
	float width;
	Matrix model;
	Matrix view;
	Matrix proj;
	float time = 0.0f;
	Vector3 padding;
	Vector4 edges = Vector4(1.0f);
	Vector2 inside = Vector2(1.0f);
	Vector2 padding2;
};

class TessellatedQuad
{
public:
	void Initialize(ComPtr<ID3D11Device>& device);
	void Render(ComPtr<ID3D11DeviceContext>& context);

public:
	TessellatedQuadConstantData m_constantData;
	ComPtr<ID3D11Buffer> m_constantBuffer;

	ComPtr<ID3D11ShaderResourceView> m_diffuseResView;
	ComPtr<ID3D11ShaderResourceView> m_specularResView;

protected:
	ComPtr<ID3D11Buffer> m_vertexBuffer;
	ComPtr<ID3D11SamplerState> m_samplerState;
	ComPtr<ID3D11VertexShader> m_vertexShader;
	ComPtr<ID3D11PixelShader> m_pixelShader;
	ComPtr<ID3D11InputLayout> m_inputLayout;

	// Hull Shader -> Tessellation stage -> Domain Shader
	ComPtr<ID3D11HullShader> m_hullShader;
	ComPtr<ID3D11DomainShader> m_domainShader;

	// ComPtr<ID3D11GeometryShader> m_geometryShader; // 미사용

	uint32_t m_indexCount = 0;

	ComPtr<ID3D11Texture2D> m_texArray;
	ComPtr<ID3D11ShaderResourceView> m_texArraySRV;

};
} // namespace kusk


