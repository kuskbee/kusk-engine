#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <iostream>
#include <vector>
#include <windows.h>
#include <wrl.h> // ComPtr

namespace kusk {

using Microsoft::WRL::ComPtr;
using std::shared_ptr;
using std::vector;
using std::wstring;

class AppBase {
public :
	AppBase();
	virtual ~AppBase();

	float GetAspectRatio() const;
	int Run();

	virtual bool Initialize();
	virtual void UpdateGUI() = 0;
	virtual void Update(float dt) = 0;
	virtual void Render() = 0;

	virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	// 마우스를 다루기 위한 편의성 재정의
	virtual void OnMouseDown(WPARAM btnState, int x, int y) {};
	virtual void OnMouseUp(WPARAM btnState, int x, int y) {};
	virtual void OnMouseMove(WPARAM btnState, int x, int y) {};

protected:
	bool InitMainWindow();
	bool InitDirect3D();
	bool InitGUI();

	void SetViewport( );
	bool CreateRenderTargetView( );
	bool CreateDepthBuffer( );
	void CreateVertexShaderAndInputLayout(
		const wstring& filename,
		const vector<D3D11_INPUT_ELEMENT_DESC>& inputElements,
		ComPtr<ID3D11VertexShader>& vertexShader,
		ComPtr<ID3D11InputLayout>& inputLayout);
	void CreatePixelShader(
		const wstring& filename, 
		ComPtr<ID3D11PixelShader>& pixelShader);
	void CreateIndexBuffer(
		const vector<uint32_t>& indices, 
		ComPtr<ID3D11Buffer>& indexBuffer);

	template <typename T_VERTEX>
	void CreateVertexBuffer(
		const vector<T_VERTEX>& vertices, 
		ComPtr<ID3D11Buffer>& vertexBuffer) {

		D3D11_BUFFER_DESC bufferDesc;
		ZeroMemory(&bufferDesc, sizeof(bufferDesc));
		bufferDesc.Usage = D3D11_USAGE_IMMUTABLE; // 초기화 후 변경 X
		bufferDesc.ByteWidth = UINT(sizeof(T_VERTEX) * vertices.size());
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.CPUAccessFlags = 0; // 0 if no CPU access is necessary.
		bufferDesc.StructureByteStride = sizeof(T_VERTEX);

		D3D11_SUBRESOURCE_DATA vertexBufferData = { 0 };
		vertexBufferData.pSysMem = vertices.data();
		vertexBufferData.SysMemPitch = 0;
		vertexBufferData.SysMemSlicePitch = 0;

		const HRESULT hr = m_device->CreateBuffer(
			&bufferDesc, &vertexBufferData, vertexBuffer.GetAddressOf());
		if (FAILED(hr)) {
			std::cout << "(vertex) CreateBuffer() failed. " << std::hex << hr << std::endl;
		}
	}

	template <typename T_CONSTANT>
	void CreateConstantBuffer(
		const T_CONSTANT& constantBufferData,
		ComPtr<ID3D11Buffer>& constantBuffer) {
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

		auto hr = m_device->CreateBuffer(&cbDesc, &initData, 
								constantBuffer.GetAddressOf());
		if (FAILED(hr)) {
			std::cout << "CreateConstantBuffer() CreateBuffer failed." << std::endl;
		}
	}

	template <typename T_DATA>
	void UpdateBuffer(const T_DATA& bufferData, ComPtr<ID3D11Buffer>& buffer) {

		if (!buffer) {
			std::cout << "UpdateBuffer() buffer was not initialized." << std::endl;
		}

		D3D11_MAPPED_SUBRESOURCE ms;
		m_context->Map(buffer.Get(), NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);
		memcpy(ms.pData, &bufferData, sizeof(bufferData));
		m_context->Unmap(buffer.Get(), NULL);
	}

	void CreateTexture(const std::string filename,
					   ComPtr<ID3D11Texture2D>& texture,
					   ComPtr<ID3D11ShaderResourceView>& textureResourceView);
	void CreateCubemapTexture(const wchar_t* filename,
							 ComPtr<ID3D11ShaderResourceView>& texResView);

public:
	int m_screenWidth; // 렌더링할 최종 화면의 해상도
	int m_screenHeight;
	int m_guiWidth = 0;
	HWND m_mainWindow;
	UINT m_numQualityLevels = 0;

	ComPtr<ID3D11Device> m_device;
	ComPtr<ID3D11DeviceContext> m_context;
	ComPtr<ID3D11RenderTargetView> m_renderTargetView;
	ComPtr<IDXGISwapChain> m_swapChain;

	ComPtr<ID3D11RasterizerState> m_solidRasterizerState;
	ComPtr<ID3D11RasterizerState> m_wireRasterizerState;
	bool m_drawAsWire = false;

	// Depth Buffer
	ComPtr<ID3D11Texture2D> m_depthStencilBuffer;
	ComPtr<ID3D11DepthStencilView> m_depthStencilView;
	ComPtr<ID3D11DepthStencilState> m_depthStencilState;

	D3D11_VIEWPORT m_screenViewport;
};

} // namespace kusk


