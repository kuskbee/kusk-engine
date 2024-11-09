#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <windows.h>
#include <wrl/client.h>
#include <vector>
#include <iostream>

namespace kusk {

using Microsoft::WRL::ComPtr;

struct Mesh {
	ComPtr<ID3D11Buffer> vertexBuffer;
	ComPtr<ID3D11Buffer> indexBuffer;
	ComPtr<ID3D11Buffer> vertexConstantBuffer;
	ComPtr<ID3D11Buffer> pixelConstantBuffer;
	ComPtr<ID3D11Buffer> geometryConstantBuffer;

	ComPtr<ID3D11Texture2D> texture;
	ComPtr<ID3D11ShaderResourceView> textureResourceView;

	UINT indexCount = 0;
	UINT vertexCount = 0;
};
} // namespace kusk