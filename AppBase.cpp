#include "AppBase.h"

#include <algorithm>

#include "D3D11Utils.h"

// imgui_impl_win32.cpp에 정의된 메시지 처리 함수에 대한 전방 선언
// VCPKG를 통해 IMGUI를 사용할 경우 빨간줄로 경고가 뜰 수 있음
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, 
                                                             UINT msg, 
                                                             WPARAM wParam,
                                                             LPARAM lParam);

namespace kusk {

using namespace std;

// RegisterClassEx() 에서 멤버 함수를 직접 등록할 수 없기 때문에
// 클래스의 멤버 함수에서 간접적으로 메시지를 처리할 수 있도록 전역변수로 다룸.
AppBase* g_appBase = nullptr;

// RegisterClassEx() 에서 실제 등록될 콜백 함수
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    
    if (g_appBase) {
        // g_appBase를 이용해서 간접적으로 멤버 함수 호출
        return g_appBase->MsgProc(hWnd, msg, wParam, lParam);
    }
    return S_OK;
}

AppBase::AppBase()
    : m_screenWidth(1280), m_screenHeight(720), m_mainWindow(0),
      m_screenViewport(D3D11_VIEWPORT())
{
    g_appBase = this;

    m_camera.SetAspectRatio(this->GetAspectRatio( ));
}

AppBase::~AppBase() {
    g_appBase = nullptr;

    // Cleanup
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    DestroyWindow(m_mainWindow);

    // dx관련 리소스들은 ComPtr에서 알아서 release
    // https:learn.microsoft.com/en-us/cpp/cppcx/wrl/comptr-class?view=msvc-170
    // m_device.Reset() 생략
}

float AppBase::GetAspectRatio() const {
    return float(m_screenWidth - m_screenViewport.TopLeftX) / m_screenHeight;
}

int AppBase::Run() {

    // Main message loop
    MSG msg = { 0 };
    while (WM_QUIT != msg.message) {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else {
            ImGui_ImplDX11_NewFrame(); // GUI 프레임 시작
            ImGui_ImplWin32_NewFrame();

            ImGui::NewFrame();
            ImGui::Begin("Scene Control");

            // ImGui가 측정해주는 Framerate 출력
            ImGui::Text("Average %.3f ms/frame (%.1f FPS)", 
                        1000.0f / ImGui::GetIO().Framerate,
                        ImGui::GetIO().Framerate);

            UpdateGUI(); // 추가적으로 사용할 GUI

            m_guiWidth = 0;
            // 화면을 크게 쓰기 위해 아래 기능 주석
            //ImGui::SetWindowPos(ImVec2(0.0f, 0.0f));
            //m_guiWidth = int(ImGui::GetWindowWidth( ));

            ImGui::End();
            ImGui::Render();
            
            Update(ImGui::GetIO().DeltaTime);

            Render();

            ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData()); // GUI 렌더링

            // Switch the back buffer and the front buffer
            // ImGui RenderDrawData() 다음에 Present() 호출
            m_swapChain->Present(1, 0);
            
        }
    }

    return 0;
}

bool AppBase::Initialize() {

    if (!InitMainWindow())
        return false;

    if (!InitDirect3D())
        return false;

    if (!InitGUI())
        return false;

    // 콘솔창이 렌더링 창 덮는 것 방지
    SetForegroundWindow(m_mainWindow);

    return true;
}

void AppBase::OnMouseMove(int mouseX, int mouseY) {
    
    // 마우스 커서의 위치를 NDC로 변환
    // 마우스 커서는 좌측 상단 (0, 0), 우측 하단 (width-1, height-1)
    // NDC는 좌측 상단 (-1, -1), 우측 하단 (1, 1)
    m_cursorNdcX = mouseX * 2.0f / m_screenWidth - 1.0f;
    m_cursorNdcY = -mouseY * 2.0f / m_screenHeight + 1.0f;

    // 커서가 화면 밖으로 나갔을 경우 범위 조절
    m_cursorNdcX = std::clamp(m_cursorNdcX, -1.0f, 1.0f);
    m_cursorNdcY = std::clamp(m_cursorNdcY, -1.0f, 1.0f);

    // 카메라 시점 회전
    if (m_useFirstPersonView) {
        m_camera.UpdateMouse(m_cursorNdcX, m_cursorNdcY);
    }
}

LRESULT AppBase::MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {

    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg) {
    case WM_SIZE :
        // 화면 해상도가 바뀌면 SwapChain을 다시 생성
        if (m_swapChain) { // 처음 실행이 아닌 지 확인
            m_screenWidth = int(LOWORD(lParam));
            m_screenHeight = int(HIWORD(lParam));
            m_guiWidth = 0;

            m_backBufferRTV.Reset( );
            m_swapChain->ResizeBuffers( 0,   // 현재 개수 유지
                                        ( UINT ) LOWORD(lParam), // 해상도 변경
                                        ( UINT ) HIWORD(lParam),
                                        DXGI_FORMAT_UNKNOWN, // 현재 포맷 유지
                                        0);
            CreateBuffers( );
            SetViewport( );

            // 화면 해상도가 바뀌면 카메라 aspect ratio도 같이 변경
            m_camera.SetAspectRatio(this->GetAspectRatio());
        }
        break;
    case WM_SYSCOMMAND :
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_MOUSEMOVE:
        // cout << "Mouse " << LOWORD(lParam) << " " << HIWORD(lParam) << endl;
        OnMouseMove(LOWORD(lParam), HIWORD(lParam));
        break;
    case WM_LBUTTONDOWN:
        // cout << "WM_LBUTTONDOWN Left mouse button" << endl;
        if (!m_leftButton && !m_rightButton) {
            m_dragStartFlag = true;
        }
        m_leftButton = true;
        break;
    case WM_RBUTTONDOWN:
        // cout << "WM_RBUTTONDOWN Right mouse button" << endl;
        if (!m_rightButton && !m_leftButton) {
            m_dragStartFlag = true;
        }
        m_rightButton = true;
        break;
    case WM_LBUTTONUP:
        // cout << "WM_LBUTTONUP Left mouse button" << endl;
        m_leftButton = false;
        break;
    case WM_RBUTTONUP:
        // cout << "WM_RBUTTONUP Right mouse button" << endl;
        m_rightButton = false;
        break;
    case WM_KEYDOWN:
        // 키보드 키 눌림 갱신
        m_keyPressed[ wParam ] = true;
        
        if (wParam == 27) { // esc 키 종료
            DestroyWindow(hWnd);
        }
        // cout << "WM_KEYDOWN " << (int)wParam << endl;
        break;
    case WM_KEYUP :
        if (wParam == 70) { // 'f' 키, 일인칭 시점 On/Off
            m_useFirstPersonView = !m_useFirstPersonView;
        }

        if (wParam == 67) { // 'c'키, 화면 캡쳐
            ComPtr<ID3D11Texture2D> backBuffer;
            m_swapChain->GetBuffer(0, IID_PPV_ARGS(backBuffer.GetAddressOf( )));
            D3D11Utils::WriteToFile(m_device, m_context, backBuffer, "captured.png");
        }

        // 키보드 키 해제 갱신
        m_keyPressed[ wParam ] = false;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }

    return ::DefWindowProc(hWnd, msg, wParam, lParam);
}

// 여러 물체들이 공통적으로 사용하는 Const 업데이트
void AppBase::UpdateEyeViewProjBuffers(const Vector3& eyeWorld, 
                                       const Matrix& viewRow, 
                                       const Matrix& projRow, 
                                       const Matrix& refl = Matrix( )) {

    m_eyeViewProjConstData.eyeWorld = eyeWorld;
    m_eyeViewProjConstData.viewProj = (viewRow * projRow).Transpose( );
    m_mirrorEyeViewProjConstData.eyeWorld = eyeWorld;
    m_mirrorEyeViewProjConstData.viewProj = (refl * viewRow * projRow).Transpose( );

    D3D11Utils::UpdateBuffer(m_device, m_context, m_eyeViewProjConstData, m_eyeViewProjConstBuffer);
    D3D11Utils::UpdateBuffer(m_device, m_context, m_mirrorEyeViewProjConstData, m_mirrorEyeViewProjConstBuffer);
}

void AppBase::CreateDepthBuffers( ) {

    //DepthStencilView 만들기
    D3D11_TEXTURE2D_DESC desc;
    ZeroMemory(&desc, sizeof(desc));
    desc.Width = m_screenWidth;
    desc.Height = m_screenHeight;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;
    if (m_useMSAA && m_numQualityLevels > 0) {
        desc.SampleDesc.Count = 4;
        desc.SampleDesc.Quality = m_numQualityLevels - 1;
    }
    else {
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;
    }
    desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    ThrowIfFailed(m_device->CreateTexture2D(&desc, 0, m_depthStencilBuffer.GetAddressOf( )));
    ThrowIfFailed(m_device->CreateDepthStencilView(m_depthStencilBuffer.Get( ), NULL, m_depthStencilView.GetAddressOf( )));

    /* D3D11_DEPTH_STENCIL_DESC 옵션 정리
     * https://learn.microsoft.com/en-us/windows/win32/api/d3d11/ns-d3d11-d3d11_depth_stencil_desc
     * StencilRead/WriteMask: 예) uint8 중 어떤 비트를 사용할지
     */

    // m_drawDSS : 지금까지 사용해온 기본 DSS
    D3D11_DEPTH_STENCIL_DESC dsDesc;
    ZeroMemory(&dsDesc, sizeof(dsDesc));
    dsDesc.DepthEnable = true;
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
    dsDesc.StencilEnable = false; // Stencil 불필요
    dsDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
    dsDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
    // 앞면에 대해서 어떻게 작동할지 설정
    dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    // 뒷면에 대해 어떻게 작동할지 설정 (뒷면도 그릴 경우)
    dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
    dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    ThrowIfFailed(m_device->CreateDepthStencilState(&dsDesc, m_drawDSS.GetAddressOf( )));

    // Stencil에 1로 표기해주는 DSS
    dsDesc.DepthEnable = true; // 이미 그려진 물체 유지
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO; // 깊이를 덮어씌우지 않음
    dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
    dsDesc.StencilEnable = true; // Stencil 필수
    dsDesc.StencilReadMask = 0xFF; // 모든 비트 다 사용
    dsDesc.StencilWriteMask = 0xFF; // 모든 비트 다 사용
    // 앞면에 대해서 어떻게 작동할 지 설정
    dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
    dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    
    ThrowIfFailed(m_device->CreateDepthStencilState(&dsDesc, m_maskDSS.GetAddressOf( )));

    // Stencil에 1로 표기된 경우에'만' 그리는 DSS
    // DepthBuffer는 초기화된 상태로 가정
    // D3D11_COMPARISON_EQUAL 이미 1로 표기된 경우에만 그리기
    // OMSetDepthStencilState(..., 1); <- 여기의 1을 뜻한다.
    dsDesc.DepthEnable = true; // 거울 속을 다시 그릴 때 필요
    dsDesc.StencilEnable = true; // Stencil 사용
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL; // 
    dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;

    ThrowIfFailed(m_device->CreateDepthStencilState(&dsDesc, m_drawMaskedDSS.GetAddressOf( )));

    /* "이미 그려져있는 화면"과 어떻게 섞을지를 결정
    * Dest : 이미 그려져 있는 값들을 의미
    * Src : 픽셀 쉐이더가 계산한 값들을 의미 (여기서는 마지막 거울)
    */

    D3D11_BLEND_DESC mirrorBlendDesc;
    ZeroMemory(&mirrorBlendDesc, sizeof(mirrorBlendDesc));
    mirrorBlendDesc.AlphaToCoverageEnable = true; // MSAA
    mirrorBlendDesc.IndependentBlendEnable = false;
    // 개별 RenderTarget에 대해서 설정 (최대 8개)
    mirrorBlendDesc.RenderTarget[ 0 ].BlendEnable = true;
    mirrorBlendDesc.RenderTarget[ 0 ].SrcBlend = D3D11_BLEND_BLEND_FACTOR;
    mirrorBlendDesc.RenderTarget[ 0 ].DestBlend = D3D11_BLEND_INV_BLEND_FACTOR;
    mirrorBlendDesc.RenderTarget[ 0 ].BlendOp = D3D11_BLEND_OP_ADD;

    mirrorBlendDesc.RenderTarget[ 0 ].SrcBlendAlpha = D3D11_BLEND_ONE;
    mirrorBlendDesc.RenderTarget[ 0 ].DestBlendAlpha = D3D11_BLEND_ONE;
    mirrorBlendDesc.RenderTarget[ 0 ].BlendOpAlpha = D3D11_BLEND_OP_ADD;

    // 필요하면 RGBA 각각에 대해서도 조절 가능
    mirrorBlendDesc.RenderTarget[ 0 ].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    ThrowIfFailed(m_device->CreateBlendState(&mirrorBlendDesc, m_mirrorBS.GetAddressOf( )));
}

bool AppBase::InitMainWindow() {

    WNDCLASSEX wc = { sizeof(WNDCLASSEX),
                    CS_CLASSDC,
                    WndProc,
                    0L,
                    0L,
                    GetModuleHandle(NULL),
                    NULL,
                    NULL,
                    NULL,
                    NULL,
                    L"KuskEngine", // lpszClassName, L-string
                    NULL };

    if (!RegisterClassEx(&wc)) {
        cout << "RegisterClassEx() failed." << endl;
        return false;
    }

    // 툴바까지 포함한 윈도우 전체 해상도가 아니라
    // 실제로 그리는 해상도가 width x height가 되도록
    // 윈도우를 만들 해상도를 다시 계산해서 CreateWindow()에서 사용

    RECT wr = { 0, 0, m_screenWidth, m_screenHeight };

    // 필요한 윈도우 크기(해상도) 계산. wr 값이 조정됨.
    AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, false);

    m_mainWindow = CreateWindow(wc.lpszClassName, L"KuskEngine Window",
                                WS_OVERLAPPEDWINDOW,
                                100,
                                100,
                                wr.right - wr.left,
                                wr.bottom - wr.top,
                                NULL, NULL, wc.hInstance, NULL);

    if (!m_mainWindow) {
        cout << "CreateWindow() failed." << endl;
        return false;
    }

    ShowWindow(m_mainWindow, SW_SHOWDEFAULT);
    UpdateWindow(m_mainWindow);

    return true;
}

bool AppBase::InitDirect3D() {

    // 그래픽카드 호환성 문제로 실패 시 아래로 시도
    // const D3D_DRIVER_TYPE driverType = D3D_DRIVER_TYPE_WRAP; 
    const D3D_DRIVER_TYPE driverType = D3D_DRIVER_TYPE_HARDWARE;

    UINT createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    const D3D_FEATURE_LEVEL featureLevels[2] = {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_9_3
    };
    D3D_FEATURE_LEVEL featureLevel;

    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferDesc.Width = m_screenWidth;
    sd.BufferDesc.Height = m_screenHeight;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferCount = 2;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = m_mainWindow;                     // the window to be used
    sd.Windowed = TRUE;                                 // Windowed/full-screen mode
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;  // allow full-screen switching
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;

    ThrowIfFailed(D3D11CreateDeviceAndSwapChain(0, // Default adapter
        driverType,
        0, // No Software device
        createDeviceFlags, featureLevels, 1, D3D11_SDK_VERSION, &sd,
        m_swapChain.GetAddressOf( ), m_device.GetAddressOf( ), &featureLevel,
        m_context.GetAddressOf( )));

    if (featureLevel != D3D_FEATURE_LEVEL_11_0) {
        cout << "D3D Feature Level 11 unsupported." << endl;
        return false;
    }

    CreateBuffers( );
    SetViewport( );

    // Create a rasterize state
    D3D11_RASTERIZER_DESC rastDesc;
    ZeroMemory(&rastDesc, sizeof(D3D11_RASTERIZER_DESC));
    rastDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
    rastDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;
    rastDesc.FrontCounterClockwise = false;
    rastDesc.DepthClipEnable = true; // <- zNear, zFar 확인에 필요
    rastDesc.MultisampleEnable = true;

    ThrowIfFailed(m_device->CreateRasterizerState(&rastDesc, m_solidRS.GetAddressOf()));

    // 거울에 반사되면 삼각형의 Winding이 바뀌기 때문에 CCW로 그려야함.
    rastDesc.FrontCounterClockwise = true;
    ThrowIfFailed(m_device->CreateRasterizerState(&rastDesc, m_solidCCWRS.GetAddressOf( )));

    rastDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_WIREFRAME;
    ThrowIfFailed(m_device->CreateRasterizerState(&rastDesc, m_wireCCWRS.GetAddressOf( )));

    rastDesc.FrontCounterClockwise = false;
    ThrowIfFailed(m_device->CreateRasterizerState(&rastDesc, m_wireRS.GetAddressOf( )));

    // 공통으로 쓰이는 ConstBuffers
    D3D11Utils::CreateConstBuffer(m_device, m_eyeViewProjConstData, m_eyeViewProjConstBuffer);
    D3D11Utils::CreateConstBuffer(m_device, m_mirrorEyeViewProjConstData, m_mirrorEyeViewProjConstBuffer);

    return true;
}

bool AppBase::InitGUI() {

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    io.DisplaySize = ImVec2(float(m_screenWidth), float(m_screenHeight));
    ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    if (!ImGui_ImplDX11_Init(m_device.Get(), m_context.Get())) {
        return false;
    }

    if (!ImGui_ImplWin32_Init(m_mainWindow)) {
        return false;
    }

    return true;
}

void AppBase::SetViewport( ) {
    
    static int previousGuiWidth = -1;

    if (previousGuiWidth != m_guiWidth) {
        previousGuiWidth = m_guiWidth;

        // Set the viewport
        ZeroMemory(&m_screenViewport, sizeof(D3D11_VIEWPORT));
        m_screenViewport.TopLeftX = float(m_guiWidth);
        m_screenViewport.TopLeftY = 0;
        m_screenViewport.Width = float(m_screenWidth - m_guiWidth);
        m_screenViewport.Height = float(m_screenHeight);
        m_screenViewport.MinDepth = 0.0f;
        m_screenViewport.MaxDepth = 1.0f;

        m_context->RSSetViewports(1, &m_screenViewport);
    }
}

void AppBase::CreateBuffers( ) {
    
    // 레스터화 -> float/depthBuffer(MSAA) -> resolved -> backBuffer

    // BackBuffer는 화면으로 최종 출력 (SRV는 불필요)

    ComPtr<ID3D11Texture2D> backBuffer;
    ThrowIfFailed(m_swapChain->GetBuffer(0, IID_PPV_ARGS(backBuffer.GetAddressOf( ))));
    
    ThrowIfFailed(m_device->CreateRenderTargetView(backBuffer.Get( ), nullptr, m_backBufferRTV.GetAddressOf( )));

    // FLOAT MSAA RenderTargetView/ShaderResourceView
    ThrowIfFailed(m_device->CheckMultisampleQualityLevels(DXGI_FORMAT_R16G16B16A16_FLOAT, 4, &m_numQualityLevels));

    D3D11_TEXTURE2D_DESC desc;
    backBuffer->GetDesc(&desc);
    desc.MipLevels = desc.ArraySize = 1;
    desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
    desc.Usage = D3D11_USAGE_DEFAULT; // 스테이징 텍스쳐로부터 복사 가능
    desc.MiscFlags = 0;
    desc.CPUAccessFlags = 0;
    if (m_useMSAA && m_numQualityLevels) {
        desc.SampleDesc.Count = 4;
        desc.SampleDesc.Quality = m_numQualityLevels - 1;
    } else {
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;
    }

    ThrowIfFailed(m_device->CreateTexture2D(&desc, NULL, m_floatBuffer.GetAddressOf( )));
    ThrowIfFailed(m_device->CreateShaderResourceView(m_floatBuffer.Get( ), NULL, m_floatSRV.GetAddressOf( )));
    ThrowIfFailed(m_device->CreateRenderTargetView(m_floatBuffer.Get( ), NULL, m_floatRTV.GetAddressOf( )));

    // D3D11_RENDER_TARGET_VIEW_DESC viewDesc;
    // m_floatRTV->GetDesc(&viewDesc);

    // FLOAT MSAA를 Resolve해서 저장할 SRV/RTV
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;

    ThrowIfFailed(m_device->CreateTexture2D(&desc, NULL, m_resolvedBuffer.GetAddressOf( )));
    ThrowIfFailed(m_device->CreateShaderResourceView(m_resolvedBuffer.Get( ), NULL, m_resolvedSRV.GetAddressOf( )));
    ThrowIfFailed(m_device->CreateRenderTargetView(m_resolvedBuffer.Get( ), NULL, m_resolvedRTV.GetAddressOf( )));
    // m_resolvedRTV->GetDesc(&viewDesc);

    CreateDepthBuffers( );

    m_postProcess.Initialize(m_device, m_context, { m_resolvedSRV }, { m_backBufferRTV }, m_screenWidth, m_screenHeight, 4);
}


} // namespace kusk