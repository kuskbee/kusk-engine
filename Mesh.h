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

	ComPtr<ID3D11Texture2D> albedoTexture;
	ComPtr<ID3D11Texture2D> normalTexture;
	ComPtr<ID3D11Texture2D> heightTexture;
	ComPtr<ID3D11Texture2D> aoTexture;
	ComPtr<ID3D11Texture2D> metallicTexture;
	ComPtr<ID3D11Texture2D> roughnessTextrure;

	ComPtr<ID3D11ShaderResourceView> albedoSRV;
	ComPtr<ID3D11ShaderResourceView> normalSRV;
	ComPtr<ID3D11ShaderResourceView> heightSRV;
	ComPtr<ID3D11ShaderResourceView> aoSRV;
	ComPtr<ID3D11ShaderResourceView> metallicSRV;
	ComPtr<ID3D11ShaderResourceView> roughnessSRV;

	UINT indexCount = 0;
	UINT vertexCount = 0;
};
} // namespace kusk