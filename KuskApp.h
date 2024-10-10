#pragma once

#include <algorithm>
#include <directxtk/SimpleMath.h>
#include <iostream>
#include <memory>
#include <vector>

#include "AppBase.h"

namespace kusk {

using DirectX::SimpleMath::Matrix;
using DirectX::SimpleMath::Vector3;


struct Vertex {
	Vector3 position;
	Vector3 color;
};

struct ModelViewProjectionConstantBuffer {
	Matrix model;
	Matrix view;
	Matrix proj;
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
	ComPtr<ID3D11VertexShader> m_colorVertexShader;
	ComPtr<ID3D11PixelShader> m_colorPixelShader;
	ComPtr<ID3D11InputLayout> m_colorInputLayout;

	ComPtr<ID3D11Buffer> m_vertexBuffer;
	ComPtr<ID3D11Buffer> m_indexBuffer;
	ComPtr<ID3D11Buffer> m_constantBuffer;
	UINT m_indexCount;

	ModelViewProjectionConstantBuffer m_constantBufferData;

	bool m_usePerspectiveProjection = true;
};
} // namespace kusk
