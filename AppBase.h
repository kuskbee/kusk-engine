#pragma once


#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <iostream>
#include <vector>

#include "Camera.h"
#include "D3D11Utils.h"

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
	virtual void OnMouseMove(WPARAM btnState, int x, int y);

protected:
	bool InitMainWindow();
	bool InitDirect3D();
	bool InitGUI();

	void SetViewport( );
	bool CreateRenderTargetView( );

public:
	int m_screenWidth; // 렌더링할 최종 화면의 해상도
	int m_screenHeight;
	int m_guiWidth = 0;
	HWND m_mainWindow;
	UINT m_numQualityLevels = 0;

	ComPtr<ID3D11Device> m_device;
	ComPtr<ID3D11DeviceContext> m_context;
	ComPtr<ID3D11RenderTargetView> m_renderTargetView;
	ComPtr<ID3D11ShaderResourceView> m_shaderResourceView;
	ComPtr<IDXGISwapChain> m_swapChain;

	ComPtr<ID3D11RasterizerState> m_solidRasterizerState;
	ComPtr<ID3D11RasterizerState> m_wireRasterizerState;
	bool m_drawAsWire = false;
	bool m_usePostProcessing = true;

	// Depth Buffer
	ComPtr<ID3D11DepthStencilView> m_depthStencilView;
	ComPtr<ID3D11DepthStencilState> m_depthStencilState;

	Camera m_camera;
	bool m_useFirstPersonView = false;

	// 현재 키보드가 눌렸는지 상태를 저장하는 배열
	bool m_keyPressed[ 256 ] = { false };
	bool m_leftButton = false;
	bool m_dragStartFlag = false;
	bool m_selected = false;

	D3D11_VIEWPORT m_screenViewport;

	// 마우스 커서 위치 저장 (Picking에 사용)
	int m_cursorX = 0;
	int m_cursorY = 0;
	float m_cursorNdcX = 0.0f;
	float m_cursorNdcY = 0.0f;
};

} // namespace kusk


