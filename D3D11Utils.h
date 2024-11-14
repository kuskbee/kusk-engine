#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <windows.h>
#include <wrl/client.h> // ComPtr

namespace kusk {
using namespace std;
using Microsoft::WRL::ComPtr;

inline void ThrowIfFailed(HRESULT hr) {
	if (FAILED(hr)) {
		// 디버깅할 때 여기에 breakpoint 설정
		throw std::exception( );
	}
}

class D3D11Utils
{
public:
	static void CreateDepthBuffer(ComPtr<ID3D11Device>& device, int screenWidth,
								  int screenHeight, UINT numQualityLevels,
								  ComPtr<ID3D11DepthStencilView>& depthStencilView);
	static void CreateVertexShaderAndInputLayout(
		ComPtr<ID3D11Device>& device,
		const wstring& filename,
		const vector<D3D11_INPUT_ELEMENT_DESC>& inputElements,
		ComPtr<ID3D11VertexShader>& m_vertexShader,
		ComPtr<ID3D11InputLayout>& m_inputLayout);

	static void CreateHullShader(ComPtr<ID3D11Device>& device,
								 const wstring& filename,
								 ComPtr<ID3D11HullShader>& hullShader);

	static void CreateDomainShader(ComPtr<ID3D11Device>& device,
								   const wstring& filename,
								   ComPtr<ID3D11DomainShader>& domainShader);

	static void CreateGeometryShader(
		ComPtr<ID3D11Device>& device,
		const wstring& filename,
		ComPtr<ID3D11GeometryShader>& geometryShader);
	
	static void CreatePixelShader(
		ComPtr<ID3D11Device>& device,
		const wstring& filename,
		ComPtr<ID3D11PixelShader>& m_pixelShader);

	static void CreateIndexBuffer(
		ComPtr<ID3D11Device>& device,
		const vector<uint32_t>& indices,
		ComPtr<ID3D11Buffer>& indexBuffer);

	template <typename T_VERTEX>
	static void CreateVertexBuffer(
		ComPtr<ID3D11Device>& device,
		const vector<T_VERTEX>& vertices,
		ComPtr<ID3D11Buffer>& vertexBuffer) {

		D3D11_BUFFER_DESC bufferDesc;
		ZeroMemory(&bufferDesc, sizeof(bufferDesc));
		bufferDesc.Usage = D3D11_USAGE_IMMUTABLE; // 초기화 후 변경 X
		bufferDesc.ByteWidth = UINT(sizeof(T_VERTEX) * vertices.size( ));
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.CPUAccessFlags = 0; // 0 if no CPU access is necessary.
		bufferDesc.StructureByteStride = sizeof(T_VERTEX);

		D3D11_SUBRESOURCE_DATA vertexBufferData = { 0 };
		vertexBufferData.pSysMem = vertices.data( );
		vertexBufferData.SysMemPitch = 0;
		vertexBufferData.SysMemSlicePitch = 0;

		ThrowIfFailed(device->CreateBuffer(&bufferDesc, &vertexBufferData, vertexBuffer.GetAddressOf( )));
	}

	template <typename T_CONSTANT>
	static void CreateConstBuffer(
		ComPtr<ID3D11Device>& device,
		const T_CONSTANT& constantBufferData,
		ComPtr<ID3D11Buffer>& constantBuffer) {

		static_assert((sizeof(T_CONSTANT) % 16) == 0,
					  "Constant Buffer size must be 16-byte aligned");

		D3D11_BUFFER_DESC cbDesc;
		cbDesc.ByteWidth = sizeof(constantBufferData);
		cbDesc.Usage = D3D11_USAGE_DYNAMIC;
		cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbDesc.MiscFlags = 0;
		cbDesc.StructureByteStride = 0;

		// Fill in the subresource data.
		D3D11_SUBRESOURCE_DATA initData;
		initData.pSysMem = &constantBufferData;
		initData.SysMemPitch = 0;
		initData.SysMemSlicePitch = 0;

		ThrowIfFailed(device->CreateBuffer(&cbDesc, &initData,constantBuffer.GetAddressOf( )));
	}

	template <typename T_DATA>
	static void UpdateBuffer(ComPtr<ID3D11Device>& device,
		ComPtr<ID3D11DeviceContext>& context,
		const T_DATA& bufferData, ComPtr<ID3D11Buffer>& buffer) {

		if (!buffer) {
			std::cout << "UpdateBuffer() buffer was not initialized." << std::endl;
		}

		D3D11_MAPPED_SUBRESOURCE ms;
		context->Map(buffer.Get( ), NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);
		memcpy(ms.pData, &bufferData, sizeof(bufferData));
		context->Unmap(buffer.Get( ), NULL);
	}

	static void CreateTexture(ComPtr<ID3D11Device>& device,
					   ComPtr<ID3D11DeviceContext>& context,
					   const std::string filename,
					   const bool useSRGB,
					   ComPtr<ID3D11Texture2D>& texture,
					   ComPtr<ID3D11ShaderResourceView>& textureResourceView);

	static void CreateTextureArray(ComPtr<ID3D11Device>& device, 
							ComPtr<ID3D11DeviceContext>& context,
							const std::vector<std::string> filenames,
							ComPtr<ID3D11Texture2D>& texture, 
							ComPtr<ID3D11ShaderResourceView>& textureResourceView);

	static void CreateDDSTexture(ComPtr<ID3D11Device>& device,
							 const wchar_t* filename, const bool isCubeMap,
							 ComPtr<ID3D11ShaderResourceView>& texResView);

	// 텍스쳐를 이미지 파일로 저장
	static void WriteToFile(ComPtr<ID3D11Device>& device,
							ComPtr<ID3D11DeviceContext>& context,
							ComPtr<ID3D11Texture2D>& textureToWrite,
							const std::string filename);

};

} // namespace kusk

