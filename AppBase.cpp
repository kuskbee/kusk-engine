#include "AppBase.h"

#include <algorithm>
#include <directxtk/SimpleMath.h>

#include "D3D11Utils.h"
#include "GraphicsCommon.h"
#include "JsonManager.h"

// imgui_impl_win32.cpp에 정의된 메시지 처리 함수에 대한 전방 선언
// VCPKG를 통해 IMGUI를 사용할 경우 빨간줄로 경고가 뜰 수 있음
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, 
                                                             UINT msg, 
                                                             WPARAM wParam,
                                                             LPARAM lParam);

namespace kusk {

using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;
using DirectX::BoundingSphere;
using DirectX::SimpleMath::Vector3;

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
    return float(m_screenWidth) / m_screenHeight;
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

            ImGui::End();

            UpdatePopupGUI( );

            UpdateObjectCreationFrameGUI( );

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
    m_camera.UpdateMouse(m_cursorNdcX, m_cursorNdcY);
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

            m_backBufferRTV.Reset( );
            m_swapChain->ResizeBuffers( 0,   // 현재 개수 유지
                                        ( UINT ) LOWORD(lParam), // 해상도 변경
                                        ( UINT ) HIWORD(lParam),
                                        DXGI_FORMAT_UNKNOWN, // 현재 포맷 유지
                                        0);
            CreateBuffers( );
            SetMainViewport( );

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
        m_leftButton = true;
        break;
    case WM_RBUTTONDOWN:
        // cout << "WM_RBUTTONDOWN Right mouse button" << endl;
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
        
        //if (wParam == VK_ESCAPE) { // esc 키 종료
        //    DestroyWindow(hWnd);
        //}
        
        // cout << "WM_KEYDOWN " << (int)wParam << endl;
        break;
    case WM_KEYUP :
        if (wParam == 'F') { // 'f' 키, 일인칭 시점 On/Off
            m_camera.m_useFirstPersonView = !m_camera.m_useFirstPersonView;
        }

        if (wParam == 'C') { // 'c'키, 화면 캡쳐
            ComPtr<ID3D11Texture2D> backBuffer;
            m_swapChain->GetBuffer(0, IID_PPV_ARGS(backBuffer.GetAddressOf( )));
            D3D11Utils::WriteToFile(m_device, m_context, backBuffer, "captured.png");
        }

        if (wParam == VK_SPACE) {
            m_lightRotate = !m_lightRotate;
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

void AppBase::InitCubemaps(wstring envFilePath,
                           wstring specularFilePath, wstring irradianceFilePath,
                           wstring brdfFilePath) {

    m_cubemapTextureEnvFilePath = JsonManager::WStringToUTF8(envFilePath);
    m_cubemapTextureSpecularFilePath = JsonManager::WStringToUTF8(specularFilePath);
    m_cubemapTextureIrradianceFilePath = JsonManager::WStringToUTF8(irradianceFilePath);
    m_cubemapTextureBrdfFilePath = JsonManager::WStringToUTF8(brdfFilePath);


    // BRDF LookUp Table은 CubeMap이 아니라 2D 텍스쳐
    D3D11Utils::CreateDDSTexture(m_device, envFilePath.c_str( ), true, m_envSRV);
    D3D11Utils::CreateDDSTexture(m_device, specularFilePath.c_str( ), true, m_specularSRV);
    D3D11Utils::CreateDDSTexture(m_device, irradianceFilePath.c_str( ), true, m_irradianceSRV);
    D3D11Utils::CreateDDSTexture(m_device, brdfFilePath.c_str( ), false, m_brdfSRV);
}
 
// 여러 물체들이 공통적으로 사용하는 Const 업데이트
void AppBase::UpdateGlobalConstants(const Vector3& eyeWorld, 
                                    const Matrix& viewRow, 
                                    const Matrix& projRow, 
                                    const Matrix& refl,
                                    const Plane& mirrorPlane) {

    m_globalConstsCPU.eyeWorld = eyeWorld;
    m_globalConstsCPU.view = viewRow.Transpose( );
    m_globalConstsCPU.proj = projRow.Transpose( );
    m_globalConstsCPU.invProj = projRow.Invert( ).Transpose();
    m_globalConstsCPU.viewProj = (viewRow * projRow).Transpose( );
    // 그림자 렌더링에 사용
    m_globalConstsCPU.invViewProj = m_globalConstsCPU.viewProj.Invert( );
    
    m_reflectGlobalConstsCPU = m_globalConstsCPU;
    memcpy(&m_reflectGlobalConstsCPU, &m_globalConstsCPU, sizeof(m_globalConstsCPU));
    m_reflectGlobalConstsCPU.view = (refl * viewRow).Transpose( );
    m_reflectGlobalConstsCPU.viewProj = (refl * viewRow * projRow).Transpose( );
    // 그림자 렌더링에 사용
    m_reflectGlobalConstsCPU.invViewProj = m_reflectGlobalConstsCPU.viewProj.Invert();

    m_reflectGlobalConstsCPU.mirrorPlane.x = mirrorPlane.Normal( ).x;
    m_reflectGlobalConstsCPU.mirrorPlane.y = mirrorPlane.Normal( ).y;
    m_reflectGlobalConstsCPU.mirrorPlane.z = mirrorPlane.Normal( ).z;
    m_reflectGlobalConstsCPU.mirrorPlane.w = mirrorPlane.D( );

    m_globalConstsCPU.isMirror = false;
    m_reflectGlobalConstsCPU.isMirror = true;

    D3D11Utils::UpdateBuffer(m_device, m_context, m_globalConstsCPU, m_globalConstsGPU);
    D3D11Utils::UpdateBuffer(m_device, m_context, m_reflectGlobalConstsCPU, m_reflectGlobalConstsGPU);
}

void AppBase::SetGlobalConsts(ComPtr<ID3D11Buffer>& globalConstsGPU) {
    // 쉐이더와 일관성 유지 register(b1)
    m_context->VSSetConstantBuffers(1, 1, globalConstsGPU.GetAddressOf( ));
    m_context->PSSetConstantBuffers(1, 1, globalConstsGPU.GetAddressOf( ));
    m_context->GSSetConstantBuffers(1, 1, globalConstsGPU.GetAddressOf( ));
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
    
    ComPtr<ID3D11Texture2D> depthStencilBuffer;
    ThrowIfFailed(m_device->CreateTexture2D(&desc, 0, depthStencilBuffer.GetAddressOf( )));
    ThrowIfFailed(m_device->CreateDepthStencilView(depthStencilBuffer.Get( ), NULL, m_depthStencilView.GetAddressOf( )));

    // Depth 전용
    desc.Format = DXGI_FORMAT_R32_TYPELESS;
    desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    ThrowIfFailed(m_device->CreateTexture2D(&desc, NULL, m_depthOnlyBuffer.GetAddressOf( )));

    // 그림자 Buffers (Depth 전용)
    desc.Width = m_shadowWidth;
    desc.Height = m_shadowHeight;
    for (int i = 0; i < MAX_LIGHTS; i++) {
        ThrowIfFailed(m_device->CreateTexture2D(&desc, NULL, m_shadowBuffers[ i ].GetAddressOf( )));
    }

    D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
    ZeroMemory(&dsvDesc, sizeof(dsvDesc));
    dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
    dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    ThrowIfFailed(m_device->CreateDepthStencilView(m_depthOnlyBuffer.Get( ), &dsvDesc, m_depthOnlyDSV.GetAddressOf( )));

    // 그림자 DSVs
    for (int i = 0; i < MAX_LIGHTS; i++) {
        ThrowIfFailed(
            m_device->CreateDepthStencilView(m_shadowBuffers[ i ].Get( ), &dsvDesc, m_shadowDSVs[ i ].GetAddressOf( )));
    }

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    ZeroMemory(&srvDesc, sizeof(srvDesc));
    srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;
    ThrowIfFailed(m_device->CreateShaderResourceView(m_depthOnlyBuffer.Get( ), &srvDesc, m_depthOnlySRV.GetAddressOf( )));

    // 그림자 SRVs
    for (int i = 0; i < MAX_LIGHTS; i++) {
        ThrowIfFailed(
            m_device->CreateShaderResourceView(m_shadowBuffers[ i ].Get( ), &srvDesc, m_shadowSRVs[ i ].GetAddressOf( )));
    }
}

void AppBase::SetPipelineState(const GraphicsPSO& pso) {

    m_context->VSSetShader(pso.m_vertexShader.Get( ), 0, 0);
    m_context->PSSetShader(pso.m_pixelShader.Get( ), 0, 0);
    m_context->HSSetShader(pso.m_hullShader.Get( ), 0, 0);
    m_context->DSSetShader(pso.m_domainShader.Get( ), 0, 0);
    m_context->GSSetShader(pso.m_geometryShader.Get( ), 0, 0);
    m_context->IASetInputLayout(pso.m_inputLayout.Get( ));
    m_context->RSSetState(pso.m_rasterizerState.Get( ));
    m_context->OMSetBlendState(pso.m_blendState.Get( ), pso.m_blendFactor, 0xffffffff);
    m_context->OMSetDepthStencilState(pso.m_depthStencilState.Get( ), pso.m_stencilRef);
    m_context->IASetPrimitiveTopology(pso.m_primitiveTopology);
}

bool AppBase::UpdateMouseControl(const BoundingSphere& bs, Quaternion& q,
                                 Vector3& dragTranslation, Vector3& pickPoint, float& distance,
                                 MouseControlState& mcs) {
    const Matrix viewRow = m_camera.GetViewRow( );
    const Matrix projRow = m_camera.GetProjRow( );

    // 회전과 이동 초기화
    q = Quaternion::CreateFromAxisAngle(Vector3(1.0f, 0.0f, 0.0f), 0.0f);
    dragTranslation = Vector3(0.0f);

    // 마우스 선택했을 때만 업데이트
    if (m_leftButton || m_rightButton)
    {
        // OnMouseMove에서 m_mouseNdcX, m_mouseNdxY 저장

        // ViewFrustum에서 가까운 면 위의 커서 위치 (z값 유의)
        Vector3 cursorNdcNear = Vector3(m_cursorNdcX, m_cursorNdcY, 0.0f);

        // ViewFrustum에서 먼 면 위의 커서 위치 (z값 유의)
        Vector3 cursorNdcFar = Vector3(m_cursorNdcX, m_cursorNdcY, 1.0f);

        // NDC 커서 위치를 월드 좌표계로 역변환 해주는 행렬
        Matrix inverseProjView = (viewRow * projRow).Invert( );

        // ViewFrustum 안에서 PickingRay의 방향 구하기
        Vector3 cursorWorldNear = Vector3::Transform(cursorNdcNear, inverseProjView);
        Vector3 cursorWorldFar = Vector3::Transform(cursorNdcFar, inverseProjView);
        Vector3 dir = cursorWorldFar - cursorWorldNear;
        dir.Normalize( );

        // 광선을 만들고 충돌 감지
        SimpleMath::Ray curRay = SimpleMath::Ray(cursorWorldNear, dir);
        float dist = 0.0f;
        if (curRay.Intersects(bs, dist)) {
            distance = dist;
            pickPoint = cursorWorldNear + dist * dir;

            // mainSphere를 어떻게 회전시킬지 결정
            if (m_leftButton)
            {
                if (false == mcs.isDragging) { // 드래그를 시작하는 경우
                    mcs.isDragging = true;
                    mcs.prevVector = pickPoint - Vector3(bs.Center);
                    mcs.prevVector.Normalize( );
                }
                else {
                    Vector3 currentVector = pickPoint - Vector3(bs.Center);
                    currentVector.Normalize( );
                    float theta = acos(mcs.prevVector.Dot(currentVector));
                    // 마우스가 조금이라도 움직였을 경우에만 회전시키기
                    if (theta > 3.141592f / 180.0f * 3.0f) {
                        Vector3 axis = mcs.prevVector.Cross(currentVector);
                        axis.Normalize( );
                        q = SimpleMath::Quaternion::CreateFromAxisAngle(axis, theta);
                        mcs.prevVector = currentVector;
                    }
                }
                return true; // selected
            }
            // mainSphere를 어떻게 이동시킬지 결정
            else if (m_rightButton)
            {
                if (false == mcs.isDragging) { // 드래그를 시작하는 경우
                    mcs.isDragging = true;
                    mcs.prevRatio = dist / (cursorWorldFar - cursorWorldNear).Length( );
                    mcs.prevPos = pickPoint;
                }
                else {
                    Vector3 newPos = cursorWorldNear + mcs.prevRatio * (cursorWorldFar - cursorWorldNear);
                    dragTranslation = newPos - mcs.prevPos;
                    mcs.prevPos = newPos;
                }
                return true; // selected
            }
        }
    }

    mcs.isDragging = false;
    return false;
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

    Graphics::InitCommonStates(m_device);

    CreateBuffers( );
    
    SetMainViewport( );

    // 공통으로 쓰이는 ConstBuffers
    D3D11Utils::CreateConstBuffer(m_device, m_globalConstsCPU, m_globalConstsGPU);
    D3D11Utils::CreateConstBuffer(m_device, m_reflectGlobalConstsCPU, m_reflectGlobalConstsGPU);

    // 그림자맵 렌더링할 때 사용할 GlobalConsts들 별도 생성
    for (int i = 0; i < MAX_LIGHTS; i++) {
        D3D11Utils::CreateConstBuffer(m_device, m_shadowGlobalConstsCPU[ i ], m_shadowGlobalConstsGPU[ i ]);
    }

    // 후처리 효과용 ConstBuffer
    D3D11Utils::CreateConstBuffer(m_device, m_postEffectsConstsCPU, m_postEffectsConstsGPU);

    return true;
}

void SetWindowTransparency(float transparency) {
    ImGuiStyle& style = ImGui::GetStyle( );
    ImVec4& windowBg = style.Colors[ ImGuiCol_WindowBg ];

    // 기존 색상 유지, 알파(투명도)만 조정
    windowBg.w = transparency;
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

    ImGui::StyleColorsClassic( );
    SetWindowTransparency(0.5f);

    return true;
}

void AppBase::SetMainViewport( ) {
    
    // Set the viewport
    ZeroMemory(&m_screenViewport, sizeof(D3D11_VIEWPORT));
    m_screenViewport.TopLeftX = 0;
    m_screenViewport.TopLeftY = 0;
    m_screenViewport.Width = float(m_screenWidth);
    m_screenViewport.Height = float(m_screenHeight);
    m_screenViewport.MinDepth = 0.0f;
    m_screenViewport.MaxDepth = 1.0f;

    m_context->RSSetViewports(1, &m_screenViewport);
}

void AppBase::SetShadowViewport( ) {

    // Set the viewport
    D3D11_VIEWPORT shadowViewport;
    ZeroMemory(&shadowViewport, sizeof(D3D11_VIEWPORT));
    shadowViewport.TopLeftX = 0;
    shadowViewport.TopLeftY = 0;
    shadowViewport.Width = float(m_shadowWidth);
    shadowViewport.Height = float(m_shadowHeight);
    shadowViewport.MinDepth = 0.0f;
    shadowViewport.MaxDepth = 1.0f;

    m_context->RSSetViewports(1, &shadowViewport);
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
    ThrowIfFailed(m_device->CreateRenderTargetView(m_floatBuffer.Get( ), NULL, m_floatRTV.GetAddressOf( )));

    // D3D11_RENDER_TARGET_VIEW_DESC viewDesc;
    // m_floatRTV->GetDesc(&viewDesc);

    // FLOAT MSAA를 Resolve해서 저장할 SRV/RTV
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;

    ThrowIfFailed(m_device->CreateTexture2D(&desc, NULL, m_resolvedBuffer.GetAddressOf( )));
    ThrowIfFailed(m_device->CreateTexture2D(&desc, NULL, m_postEffectsBuffer.GetAddressOf( )));
    ThrowIfFailed(m_device->CreateShaderResourceView(m_resolvedBuffer.Get( ), NULL, m_resolvedSRV.GetAddressOf( )));
    ThrowIfFailed(m_device->CreateShaderResourceView(m_postEffectsBuffer.Get( ), NULL, m_postEffectsSRV.GetAddressOf( )));
    ThrowIfFailed(m_device->CreateRenderTargetView(m_resolvedBuffer.Get( ), NULL, m_resolvedRTV.GetAddressOf( )));
    ThrowIfFailed(m_device->CreateRenderTargetView(m_postEffectsBuffer.Get( ), NULL, m_postEffectsRTV.GetAddressOf( )));
    // m_resolvedRTV->GetDesc(&viewDesc);

    CreateDepthBuffers( );

    m_postProcess.Initialize(m_device, m_context, { m_postEffectsSRV }, { m_backBufferRTV }, m_screenWidth, m_screenHeight, 4);
}

void AppBase::ShowPopup(const char* name, std::function<void( )> uiCode, std::function<void( )> confirmCode) {

    ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO( ).DisplaySize.x * 0.5f,
        ImGui::GetIO( ).DisplaySize.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));

    if (ImGui::BeginPopupModal(name, NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        uiCode( );

        if (confirmCode) {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.7f, 0.3f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2f, 0.5f, 0.2f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.4f, 0.1f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));

            if (ImGui::Button("Confirm")) {
                confirmCode( );
                m_currentPopup.clear( );
                ImGui::CloseCurrentPopup( );
            }
            ImGui::PopStyleColor(4);
            ImGui::SameLine( );

            if (ImGui::Button("Close")) {
                m_currentPopup.clear( );
                ImGui::CloseCurrentPopup( );
            }
        }

        ImGui::EndPopup( );
    }
}

std::string AppBase::OpenFileDialog(std::string filterName, std::string exts) {
    char filePath[ MAX_PATH ] = "";

    OPENFILENAMEA ofn;
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    std::vector<char> combinedFilter;
    combinedFilter.insert(combinedFilter.end( ), filterName.begin( ), filterName.end( ));
    combinedFilter.push_back('\0');
    combinedFilter.insert(combinedFilter.end( ), exts.begin( ), exts.end( ));
    combinedFilter.push_back('\0');
    ofn.lpstrFilter = combinedFilter.data( );
    ofn.lpstrFile = filePath;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetOpenFileNameA(&ofn)) {
        return std::string(filePath);
    }

    return "";
}

std::string AppBase::SaveFileDialog(std::string filterName, std::string exts, std::string defaultExt) {
    char filePath[ MAX_PATH ] = "";

    OPENFILENAMEA ofn;
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    std::vector<char> combinedFilter;
    combinedFilter.insert(combinedFilter.end( ), filterName.begin( ), filterName.end( ));
    combinedFilter.push_back('\0');
    combinedFilter.insert(combinedFilter.end( ), exts.begin( ), exts.end( ));
    combinedFilter.push_back('\0');
    ofn.lpstrFilter = combinedFilter.data( );
    ofn.lpstrFile = filePath;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_OVERWRITEPROMPT; // 덮어쓰기 경고
    ofn.lpstrDefExt = defaultExt.c_str( );

    if (GetSaveFileNameA(&ofn)) {
        return std::string(filePath); // 선택된 파일 경로 반환
    }

    return "";
}

} // namespace kusk