#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <windows.h>
#include <wrl.h>
#include <vector>
#include <iostream>

namespace kusk {

using Microsoft::WRL::ComPtr;

// 같은 메쉬를 여러 번 그릴 때 버퍼들을 재사용
struct Mesh {
	ComPtr<ID3D11Buffer> vertexBuffer;
	ComPtr<ID3D11Buffer> indexBuffer;
	ComPtr<ID3D11Buffer> vertexConstantBuffer;
	ComPtr<ID3D11Buffer> pixelConstantBuffer;

	ComPtr<ID3D11Texture2D> texture;
	ComPtr<ID3D11ShaderResourceView> textureResourceView;

	UINT indexCount = 0;
};
} // namespace kusk