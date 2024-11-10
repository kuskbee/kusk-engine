#include "D3D11Utils.h"

#include <directxtk/DDSTextureLoader.h> // 큐브맵 읽을 때 필요
#include <dxgi.h>		// DXGIFactory
#include <dxgi1_4.h>	// DXGIFactory4
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace kusk {
using namespace std;
using namespace DirectX;

bool D3D11Utils::CreateDepthBuffer(ComPtr<ID3D11Device>& device, int screenWidth,
                                  int screenHeight, UINT& numQualityLevels,
                                  ComPtr<ID3D11DepthStencilView>& depthStencilView) {
    // Create depth buffer
    D3D11_TEXTURE2D_DESC depthStencilBufferDesc;
    depthStencilBufferDesc.Width = screenWidth;
    depthStencilBufferDesc.Height = screenHeight;
    depthStencilBufferDesc.MipLevels = 1;
    depthStencilBufferDesc.ArraySize = 1;
    depthStencilBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    if (numQualityLevels > 0) {
        depthStencilBufferDesc.SampleDesc.Count = 4;
        depthStencilBufferDesc.SampleDesc.Quality = numQualityLevels - 1;
    }
    else {
        depthStencilBufferDesc.SampleDesc.Count = 1;
        depthStencilBufferDesc.SampleDesc.Quality = 0;
    }
    depthStencilBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    depthStencilBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthStencilBufferDesc.CPUAccessFlags = 0;
    depthStencilBufferDesc.MiscFlags = 0;

    ComPtr<ID3D11Texture2D> depthStencilBuffer;

    if (FAILED(device->CreateTexture2D(
        &depthStencilBufferDesc, 0,
        depthStencilBuffer.GetAddressOf( )))) {
        cout << "CreateTexture2D() depthStencilBuffer failed." << endl;
    }
    if (FAILED(device->CreateDepthStencilView(
        depthStencilBuffer.Get( ), 0,
        depthStencilView.GetAddressOf( )))) {
        cout << "CreateDepthStencilView() failed." << endl;
    }

    return true;
}

void CheckResult(HRESULT hr, ID3DBlob* errorBlob) {
    if (FAILED(hr)) {
        // 파일이 없을 경우
        if ((hr & D3D11_ERROR_FILE_NOT_FOUND) != 0) {
            cout << "File not found." << endl;
        }

        // 에러 메시지가 있으면 출력
        if (errorBlob) {
            cout << "Shader compiler error\n" << ( char* ) errorBlob->GetBufferPointer( ) << endl;
        }
    }
}

void D3D11Utils::CreateVertexShaderAndInputLayout(
    ComPtr<ID3D11Device>& device,
    const wstring& filename,
    const vector<D3D11_INPUT_ELEMENT_DESC>& inputElements,
    ComPtr<ID3D11VertexShader>& vertexShader,
    ComPtr<ID3D11InputLayout>& inputLayout) {

    ComPtr<ID3DBlob> shaderBlob;
    ComPtr<ID3DBlob> errorBlob;

    UINT compileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
    compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

    // 쉐이더의 시작점의 이름이 "main"인 함수로 지정
    // D3D_COMPILE_STANDARD_FILE_INCLUDE 추가 : 쉐이더에서 include 사용
    HRESULT hr =
        D3DCompileFromFile(filename.c_str( ), 0, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "vs_5_0",
                            compileFlags, 0, &shaderBlob, &errorBlob);

    CheckResult(hr, errorBlob.Get( ));

    device->CreateVertexShader(shaderBlob->GetBufferPointer( ), shaderBlob->GetBufferSize( ), NULL,
                                 vertexShader.GetAddressOf( ));

    device->CreateInputLayout(inputElements.data( ), UINT(inputElements.size( )),
                                shaderBlob->GetBufferPointer( ), shaderBlob->GetBufferSize( ),
                                inputLayout.GetAddressOf( ));
}

void D3D11Utils::CreateHullShader(ComPtr<ID3D11Device>& device,
                                  const wstring& filename,
                                  ComPtr<ID3D11HullShader>& hullShader) {
    ComPtr<ID3DBlob> shaderBlob;
    ComPtr<ID3DBlob> errorBlob;
        
    UINT compileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
    compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

    HRESULT hr =
        D3DCompileFromFile(filename.c_str( ), 0, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "hs_5_0",
                            compileFlags, 0, &shaderBlob, &errorBlob);

    CheckResult(hr, errorBlob.Get( ));

    device->CreateHullShader(shaderBlob->GetBufferPointer( ), 
                             shaderBlob->GetBufferSize( ), NULL, hullShader.GetAddressOf( ));

}

void D3D11Utils::CreateDomainShader(ComPtr<ID3D11Device>& device,
                                  const wstring& filename,
                                  ComPtr<ID3D11DomainShader>& domainShader) {
    ComPtr<ID3DBlob> shaderBlob;
    ComPtr<ID3DBlob> errorBlob;

    UINT compileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
    compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

    HRESULT hr =
        D3DCompileFromFile(filename.c_str( ), 0, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "ds_5_0",
                            compileFlags, 0, &shaderBlob, &errorBlob);

    CheckResult(hr, errorBlob.Get( ));

    device->CreateDomainShader(shaderBlob->GetBufferPointer( ),
                             shaderBlob->GetBufferSize( ), NULL, domainShader.GetAddressOf( ));

}

void D3D11Utils::CreatePixelShader(ComPtr<ID3D11Device>& device, const wstring& filename, ComPtr<ID3D11PixelShader>& pixelShader) {
    ComPtr<ID3DBlob> shaderBlob;
    ComPtr<ID3DBlob> errorBlob;

    UINT compileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
    compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

    // 쉐이더의 시작점의 이름이 "main"인 함수로 지정
    // D3D_COMPILE_STANDARD_FILE_INCLUDE 추가 : 쉐이더에서 include 사용
    HRESULT hr =
        D3DCompileFromFile(filename.c_str( ), 0, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "ps_5_0",
                            compileFlags, 0, &shaderBlob, &errorBlob);

    CheckResult(hr, errorBlob.Get( ));

    device->CreatePixelShader(shaderBlob->GetBufferPointer( ), shaderBlob->GetBufferSize( ), NULL,
                                pixelShader.GetAddressOf( ));
}

void D3D11Utils::CreateIndexBuffer(ComPtr<ID3D11Device>& device, 
                                const std::vector<uint32_t>& indices,
                                ComPtr<ID3D11Buffer>& indexBuffer) {
    D3D11_BUFFER_DESC bufferDesc = {};
    bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
    bufferDesc.ByteWidth = UINT(sizeof(uint32_t) * indices.size( ));
    bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bufferDesc.CPUAccessFlags = 0;
    bufferDesc.StructureByteStride = sizeof(uint32_t);

    D3D11_SUBRESOURCE_DATA indexBufferData = { 0 };
    indexBufferData.pSysMem = indices.data( );
    indexBufferData.SysMemPitch = 0;
    indexBufferData.SysMemSlicePitch = 0;

    device->CreateBuffer(&bufferDesc, &indexBufferData, indexBuffer.GetAddressOf( ));
}

void D3D11Utils::CreateGeometryShader(
    ComPtr<ID3D11Device>& device,
    const wstring& filename,
    ComPtr<ID3D11GeometryShader>& geometryShader) {

    ComPtr<ID3DBlob> shaderBlob;
    ComPtr<ID3DBlob> errorBlob;

    UINT compileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
    compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

    // 쉐이더의 시작점의 이름이 "main"인 함수로 지정
    // D3D_COMPILE_STANDARD_FILE_INCLUDE 추가 : 쉐이더에서 include 사용
    HRESULT hr = D3DCompileFromFile(
        filename.c_str( ), 0, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main",
        "gs_5_0", compileFlags, 0, &shaderBlob, &errorBlob);

    // CheckResult(hr, errorBlob.Get());

    device->CreateGeometryShader(shaderBlob->GetBufferPointer( ),
                                 shaderBlob->GetBufferSize( ), NULL,
                                 &geometryShader);

}

void ReadImage(const std::string filename, std::vector<uint8_t>& image, int& width, int& height) {
    
    int channels;

    unsigned char* img = stbi_load(filename.c_str( ), &width, &height, &channels, 0);

    // 4채널로 만들어서 복사
    image.resize(width * height * 4);

    if (channels == 1) {
        for (size_t i = 0; i < width * height; i++) {
            uint8_t g = img[ i * channels + 0 ];
            for (size_t c = 0; c < 4; c++) {
                image[ 4 * i + c ] = g;
            }
        }
    }
    else if (channels == 3) {
        for (size_t i = 0; i < width * height; i++) {
            for (size_t c = 0; c < 3; c++) {
                image[ 4 * i + c ] = img[ i * channels + c ];
            }
            image[ 4 * i + 3 ] = 255;
        }
    }
    else if (channels == 4) {
        for (size_t i = 0; i < width * height; i++) {
            for (size_t c = 0; c < 4; c++) {
                image[ 4 * i + c ] = img[ i * channels + c ];
            }
        }
    }
    else {
        std::cout << "Cannot read " << channels << " channels" << endl;
    }
}

ComPtr<ID3D11Texture2D> CreateStagingTexture(ComPtr<ID3D11Device>& device,
                                             ComPtr<ID3D11DeviceContext>& context, 
                                             const int width, const int height, 
                                             const std::vector<uint8_t>& image,
                                             const int mipLevels = 1, const int arraySize = 1) {
    // 스테이징 텍스쳐 만들기
    D3D11_TEXTURE2D_DESC txtDesc;
    ZeroMemory(&txtDesc, sizeof(txtDesc));
    txtDesc.Width = width;
    txtDesc.Height = height;
    txtDesc.MipLevels = mipLevels;
    txtDesc.ArraySize = arraySize;
    txtDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    txtDesc.SampleDesc.Count = 1;
    txtDesc.Usage = D3D11_USAGE_STAGING;
    txtDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;

    ComPtr<ID3D11Texture2D> stagingTexture;
    if (FAILED(device->CreateTexture2D(&txtDesc, nullptr, stagingTexture.GetAddressOf( )))) {
        cout << "Failed()" << endl;
    }

    // CPU에서 이미지 데이터 복사
    D3D11_MAPPED_SUBRESOURCE ms;
    context->Map(stagingTexture.Get( ), NULL, D3D11_MAP_WRITE, NULL, &ms);
    uint8_t* pData = ( uint8_t* ) ms.pData;
    for (UINT h = 0; h < UINT(height); h++) { // 가로줄 한 줄씩 복사
        memcpy(&pData[ h * ms.RowPitch ], &image[ h * width * 4 ], width * sizeof(uint8_t) * 4);
    }
    context->Unmap(stagingTexture.Get( ), NULL);

    return stagingTexture;
}

void D3D11Utils::CreateTexture(
    ComPtr<ID3D11Device>& device,
    ComPtr<ID3D11DeviceContext>& context,
    const std::string filename,
    ComPtr<ID3D11Texture2D>& texture,
    ComPtr<ID3D11ShaderResourceView>& textureResourceView) {

    int width, height;
    std::vector<uint8_t> image;

    ReadImage(filename, image, width, height);

    // 스테이징 텍스쳐 만들고 CPU의 이미지를 복사합니다.
    ComPtr<ID3D11Texture2D> stagingTexture = CreateStagingTexture(device, context, width, height, image);

    // 실제로 사용할 텍스쳐 설정
    D3D11_TEXTURE2D_DESC txtDesc;
    ZeroMemory(&txtDesc, sizeof(txtDesc));
    txtDesc.Width = width;
    txtDesc.Height = height;
    txtDesc.MipLevels = 0; // 밉맵 레벨 최대
    txtDesc.ArraySize = 1;
    txtDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    txtDesc.SampleDesc.Count = 1;
    txtDesc.Usage = D3D11_USAGE_DEFAULT; // 스테이징 텍스쳐로부터 복사 가능하도록
    txtDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
    txtDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS; // 밉맵 사용
    txtDesc.CPUAccessFlags = 0;

    // 초기 데이터 없이 텍스쳐 생성 (전부 검은색)
    device->CreateTexture2D(&txtDesc, nullptr, texture.GetAddressOf( ));

    // 실제로 생성된 MipLevels 확인용
    /*texture->GetDesc(&txtDesc);
    cout << txtDesc.MipLevels << endl;*/

    // 스테이징 텍스쳐로부터 가장 해상도가 높은 이미지 복사
    context->CopySubresourceRegion(texture.Get( ), 0, 0, 0, 0, stagingTexture.Get( ), 0, nullptr);

    // ResourceView 만들기
    device->CreateShaderResourceView(texture.Get( ), 0, textureResourceView.GetAddressOf( ));

    // 해상도를 낮춰가며 밉맵 생성
    context->GenerateMips(textureResourceView.Get( ));
}

void D3D11Utils::CreateTextureArray(
    ComPtr<ID3D11Device>& device, 
    ComPtr<ID3D11DeviceContext>& context, 
    const std::vector<std::string> filenames,
    ComPtr<ID3D11Texture2D>& texture, 
    ComPtr<ID3D11ShaderResourceView>& textureResourceView) {

    if (filenames.empty( ))
        return;

    // 모든 이미지의 width 와 height가 같다고 가정

    // 파일로부터 이미지 여러 개를 읽어들입니다.
    int width = 0, height = 0;
    std::vector<vector<uint8_t>> imageArray;
    for (const auto& f : filenames) {
        cout << f << endl;

        std::vector<uint8_t> image;
        
        ReadImage(f, image, width, height);
        
        imageArray.push_back(image);
    }

    UINT size = UINT(filenames.size( ));

    // Texture2DArray 생성. 이 때 데이터를 CPU로 부터 복사하지 않음.
    D3D11_TEXTURE2D_DESC txtDesc;
    ZeroMemory(&txtDesc, sizeof(txtDesc));
    txtDesc.Width = UINT(width);
    txtDesc.Height = UINT(height);
    txtDesc.MipLevels = 0; // 밉맵 레벨 최대
    txtDesc.ArraySize = size;
    txtDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    txtDesc.SampleDesc.Count = 1;
    txtDesc.SampleDesc.Quality = 0;
    txtDesc.Usage = D3D11_USAGE_DEFAULT; // 스테이징 텍스쳐로부터 복사 가능
    txtDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
    txtDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS; // 밉맵 사용

    // 초기 데이터 없이 텍스쳐를 만들기
    device->CreateTexture2D(&txtDesc, nullptr, texture.GetAddressOf( ));
    texture->GetDesc(&txtDesc);

    // StagingTexture를 만들어서 하나씩 복사
    for (size_t i = 0; i < imageArray.size( ); i++) {

        auto& image = imageArray[ i ];

        ComPtr<ID3D11Texture2D> stagingTexture = CreateStagingTexture(device, context, width, height, image, 1, 1);

        // 스테이징 텍스쳐를 텍스쳐 배열의 해당 위치에 복사
        UINT subresourceIndex = D3D11CalcSubresource(0, UINT(i), txtDesc.MipLevels);

        context->CopySubresourceRegion(texture.Get( ), subresourceIndex, 0, 0, 0, stagingTexture.Get( ), 0, nullptr);
    }

    device->CreateShaderResourceView(texture.Get( ), nullptr, textureResourceView.GetAddressOf( ));
    
    context->GenerateMips(textureResourceView.Get( ));
}

void D3D11Utils::CreateCubemapTexture(
    ComPtr<ID3D11Device>& device,
    const wchar_t* filename,
    ComPtr<ID3D11ShaderResourceView>& textureResourceView) {
    ComPtr<ID3D11Texture2D> texture;

    // https://github.com/microsoft/DirectXTK/wiki/DDSTextureLoader
    auto hr = CreateDDSTextureFromFileEx(
        device.Get( ), filename, 0, D3D11_USAGE_DEFAULT,
        D3D11_BIND_SHADER_RESOURCE, 0,
        D3D11_RESOURCE_MISC_TEXTURECUBE, // 큐브맵용 텍스춰
        DDS_LOADER_FLAGS(false), ( ID3D11Resource** ) texture.GetAddressOf( ),
        textureResourceView.GetAddressOf( ), nullptr);

    if (FAILED(hr)) {
        std::cout << "CreateDDSTextureFromFileEx() failed" << std::endl;
    }
}
}