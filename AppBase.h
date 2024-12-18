#pragma once

#include <directxtk/SimpleMath.h>
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <iostream>
#include <vector>

#include "Camera.h"
#include "ConstantBuffers.h"
#include "D3D11Utils.h"
#include "GraphicsPSO.h"
#include "PostProcess.h"
#include "MouseControlState.h"

namespace kusk {

using DirectX::BoundingSphere;
using DirectX::SimpleMath::Quaternion;
using DirectX::SimpleMath::Vector2;
using DirectX::SimpleMath::Vector3;
using DirectX::SimpleMath::Plane;
using Microsoft::WRL::ComPtr;
using std::shared_ptr;
using std::vector;
using std::wstring;

class AppBase {
public:
	AppBase( );
	virtual ~AppBase( );

	int Run( );
	float GetAspectRatio( ) const;

	virtual bool Initialize( );
	virtual void UpdateGUI( ) = 0;
	virtual void UpdatePopupGUI( ) = 0;
	virtual void Update(float dt) = 0;
	virtual void Render( ) = 0;
	virtual void OnMouseMove(int mouseX, int mouseY);
	virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	void InitCubemaps(wstring envFilePath, wstring specularFilePath,
					  wstring irradianceFilePath, wstring brdfFilePath);
	void UpdateGlobalConstants(const Vector3& eyeWorld, const Matrix& viewRow,
							   const Matrix& projRow, const Matrix& refl, const Plane& mirrorPlane);
	void SetGlobalConsts(ComPtr<ID3D11Buffer>& globalConstsGPU);

	void CreateDepthBuffers( );
	void SetPipelineState(const GraphicsPSO& pso);
	bool UpdateMouseControl(const BoundingSphere& bs, Quaternion& q,
							Vector3& dragTranslation, Vector3& pickPoint, float& distance,
							MouseControlState& mcs);

	// ImGui Popup 관련
	virtual void UpdateObjectCreationFrameGUI() = 0;

	void ShowPopup(const char* name, std::function<void( )> uiCode, std::function<void( )> confirmCode=nullptr);
	std::string OpenFileDialog(std::string filterName, std::string exts);
	std::string SaveFileDialog(std::string filterName, std::string exts, std::string defaultExt);

protected:
	bool InitMainWindow( );
	bool InitDirect3D( );
	bool InitGUI( );
	void CreateBuffers( );
	void SetMainViewport( );
	void SetShadowViewport( );

public:
	int m_screenWidth; // 렌더링할 최종 화면의 해상도
	int m_screenHeight;
	HWND m_mainWindow;
	bool m_useMSAA = true;
	UINT m_numQualityLevels = 0;
	bool m_drawAsWire = false;

	ComPtr<ID3D11Device> m_device;
	ComPtr<ID3D11DeviceContext> m_context;
	ComPtr<IDXGISwapChain> m_swapChain;
	ComPtr<ID3D11RenderTargetView> m_backBufferRTV;

	// 삼각형 레스터화 -> float(MSAA) -> resolved(No MSAA)
	// -> PostEffects(안개, ...)
	// -> 후처리(블룸, 톤매핑) -> backBuffer(최종 SwapChain Present)
	ComPtr<ID3D11Texture2D> m_floatBuffer;
	ComPtr<ID3D11Texture2D> m_resolvedBuffer;
	ComPtr<ID3D11Texture2D> m_postEffectsBuffer;
	ComPtr<ID3D11RenderTargetView> m_floatRTV;
	ComPtr<ID3D11RenderTargetView> m_resolvedRTV;
	ComPtr<ID3D11RenderTargetView> m_postEffectsRTV;
	ComPtr<ID3D11ShaderResourceView> m_resolvedSRV;
	ComPtr<ID3D11ShaderResourceView> m_postEffectsSRV;

	// Depth Buffer 관련
	ComPtr<ID3D11Texture2D> m_depthOnlyBuffer; // No MSAA
	ComPtr<ID3D11DepthStencilView> m_depthOnlyDSV;
	ComPtr<ID3D11DepthStencilView> m_depthStencilView;
	ComPtr<ID3D11ShaderResourceView> m_depthOnlySRV;

	// Shadow maps
	int m_shadowWidth = 1280;
	int m_shadowHeight = 1280;
	ComPtr<ID3D11Texture2D> m_shadowBuffers[ MAX_LIGHTS ]; // No MSAA
	ComPtr<ID3D11DepthStencilView> m_shadowDSVs[ MAX_LIGHTS ];
	ComPtr<ID3D11ShaderResourceView> m_shadowSRVs[ MAX_LIGHTS ];

	Camera m_camera;

	// 현재 키보드가 눌렸는지 상태를 저장하는 배열
	bool m_keyPressed[ 256 ] = { false };
	bool m_leftButton = false;
	bool m_rightButton = false;
	int m_selectedModelIndex = -1;

	D3D11_VIEWPORT m_screenViewport;

	// 마우스 커서 위치 저장 (Picking에 사용)
	float m_cursorNdcX = 0.0f;
	float m_cursorNdcY = 0.0f;

	// 렌더링 -> PostEffects -> PostProcess
	PostEffectsConstants m_postEffectsConstsCPU;
	ComPtr<ID3D11Buffer> m_postEffectsConstsGPU;

	PostProcess m_postProcess;

	// 다양한 Pass들을 더 간단히 구현하기 위해 ConstBuffers들 분리
	GlobalConstants m_globalConstsCPU;
	GlobalConstants m_reflectGlobalConstsCPU;
	GlobalConstants m_shadowGlobalConstsCPU[ MAX_LIGHTS ];
	ComPtr<ID3D11Buffer> m_globalConstsGPU;
	ComPtr<ID3D11Buffer> m_reflectGlobalConstsGPU;
	ComPtr<ID3D11Buffer> m_shadowGlobalConstsGPU[ MAX_LIGHTS ];

	// 공통으로 사용하는 텍스쳐들
	ComPtr<ID3D11ShaderResourceView> m_envSRV;
	ComPtr<ID3D11ShaderResourceView> m_irradianceSRV;
	ComPtr<ID3D11ShaderResourceView> m_specularSRV;
	ComPtr<ID3D11ShaderResourceView> m_brdfSRV;

	// IBL 텍스쳐 정보
	std::string m_cubemapTextureEnvFilePath;
	std::string m_cubemapTextureSpecularFilePath;
	std::string m_cubemapTextureIrradianceFilePath;
	std::string m_cubemapTextureBrdfFilePath;

	bool m_lightRotate = false;
protected:
	//ImGui Popup 관련
	std::string m_currentPopup;

	ModelCreationParams m_modelParams;
};

} // namespace kusk