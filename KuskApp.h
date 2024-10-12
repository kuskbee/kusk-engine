#pragma once

#include <algorithm>
#include <iostream>
#include <memory>

#include "AppBase.h"
#include "GeometryGenerator.h"
#include "Mesh.h"

namespace kusk {

using DirectX::SimpleMath::Matrix;
using DirectX::SimpleMath::Vector2;
using DirectX::SimpleMath::Vector3;

// 재질
struct Material {
	Vector3 ambient = Vector3(0.1f);	// 12
	float shininess = 1.0f;				// 4
	Vector3 diffuse = Vector3(0.5f);	// 12
	float dummy1;						// 4
	Vector3 specular = Vector3(0.5f);	// 12
	float dummy2;						// 4
};

struct Light {
	// 순서와 크기 관계 주의 (16 byte 패딩)
	Vector3 strength = Vector3(1.0f);				// 12
	float fallOffStart = 0.0f;						// 4
	Vector3 direction = Vector3(0.0f, 0.0f, 1.0f);	// 12
	float fallOffEnd = 10.0f;						// 4
	Vector3 position = Vector3(0.0f, 0.0f, -2.0f);	// 12
	float spotPower = 1.0f;							// 4
};

struct BasicVertexConstantBuffer {
	Matrix model;
	Matrix invTranspose;
	Matrix view;
	Matrix proj;
};

// 주의:
// For a constant buffer (BindFlags of D3D11_BUFFER_DESC set to
// D3D11_BIND_CONSTANT_BUFFER), you must set the ByteWidth value of
// D3D11_BUFFER_DESC in multiples of 16, and less than or equal to
// D3D11_REQ_CONSTANT_BUFFER_ELEMENT_COUNT.
// https://learn.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11device-createbuffer

static_assert((sizeof(BasicVertexConstantBuffer) % 16) == 0,
	"Constant Buffer size must be 16-byte aligned");

#define MAX_LIGHTS 3

struct BasicPixelConstatntBuffer {
	Vector3 eyeWorld;		// 12
	bool useTexture;		// 4
	Material material;		// 48
	Light light[ MAX_LIGHTS ]; // 48 * MAX_LIGHTS
};

static_assert((sizeof(BasicPixelConstatntBuffer) % 16) == 0,
	"Constant Buffer size must be 16-byte aligned");

struct NormalVertexConstantBuffer {
	float scale = 0.1f;
	float dummy[ 3 ];
};

class KuskApp : public AppBase
{
public:
	KuskApp();

	virtual bool Initialize() override;
	virtual void UpdateGUI() override;
	virtual void Update(float dt) override;
	virtual void Render() override;

protected:
	ComPtr<ID3D11VertexShader> m_basicVertexShader;
	ComPtr<ID3D11PixelShader> m_basicPixelShader;
	ComPtr<ID3D11InputLayout> m_basicInputLayout;

	shared_ptr<Mesh> m_mesh;

	// Texturing
	ComPtr<ID3D11Texture2D> m_texture;
	ComPtr<ID3D11ShaderResourceView> m_textureResourceView;
	ComPtr<ID3D11Texture2D> m_texture2;
	ComPtr<ID3D11ShaderResourceView> m_textureResourceView2;
	ComPtr<ID3D11SamplerState> m_samplerState;

	BasicVertexConstantBuffer m_basicVertexConstantBufferData;
	BasicPixelConstatntBuffer m_basicPixelConstantBufferData;

	bool m_usePerspectiveProjection = true;
	Vector3 m_modelTranslation = Vector3(0.0f, 0.0f, 0.0f);
	Vector3 m_modelRotation = Vector3(-0.6f, 1.0f, 0.0f);
	Vector3 m_modelScaling = Vector3(0.5f);
	/*Vector3 m_viewEyePos = { 0.0f, 0.0f, -2.0f };
	Vector3 m_viewEyeDir = { 0.0f, 0.0f, 1.0f };
	Vector3 m_viewUp = { 0.0f, 1.0f, 0.0f };*/
	float m_viewRot = 0.0f;

	float m_projFovAngleY = 70.0f;
	float m_nearZ = 0.01f;
	float m_farZ = 100.0f;

	int m_lightType = 0;
	Light m_lightFromGUI;
	float m_materialDiffuse = 0.7f;
	float m_materialSpecular = 0.2f;

	// 노멀 벡터 그리기
	ComPtr<ID3D11VertexShader> m_normalVertexShader;
	ComPtr<ID3D11PixelShader> m_normalPixelShader;
	// ID3D11InputLayout은 basic과 같이 사용

	shared_ptr<Mesh> m_normalLines;
	NormalVertexConstantBuffer m_normalVertexConstantBufferData;
	bool m_drawNormals = false;
	bool m_drawNormalsDirtyFlag = false;
};
} // namespace kusk
