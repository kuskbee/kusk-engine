#pragma once

#include "BasicConstantData.h"
#include "D3D11Utils.h"
#include "Mesh.h"
#include "MeshData.h"

namespace kusk {
class BasicMeshGroup
{
public:
    void Initialize(ComPtr<ID3D11Device>& device, 
                ComPtr<ID3D11DeviceContext>& context,
                const std::string& basePath,
                const std::string& filename);

    void Initialize(ComPtr<ID3D11Device>& device,
                    ComPtr<ID3D11DeviceContext>& context,
                    const std::vector<MeshData>& meshes);

    void UpdateConstantBuffers(ComPtr<ID3D11Device>& device,
                               ComPtr<ID3D11DeviceContext>& context);

    void Render(ComPtr<ID3D11DeviceContext>& context);

    void UpdateModelWorld(const Matrix& modelToWorldRow);
public:
    Matrix m_modelWorldRow = Matrix( );
    Matrix m_invTransposeRow = Matrix( );

	// ExampleApp::Update()에서 접근
    BasicVertexConstantData m_basicVertexConstantData;
    BasicPixelConstantData m_basicPixelConstantData;

	// ExampleApp::Initialize()에서 접근
    ComPtr<ID3D11ShaderResourceView> m_diffuseResView;
    ComPtr<ID3D11ShaderResourceView> m_specularResView;

    // GUI에서 업데이트할 때 사용
    NormalVertexConstantData m_normalVertexConstantData;
    bool m_drawNormalsDirtyFlag = true;
    bool m_drawNormals = false;

    std::vector<shared_ptr<Mesh>> m_meshes; // Mipmaps 생성을 위해 임시로
private:
    
    ComPtr<ID3D11VertexShader> m_basicVertexShader;
    ComPtr<ID3D11PixelShader> m_basicPixelShader;
    ComPtr<ID3D11InputLayout> m_basicInputLayout;

    ComPtr<ID3D11SamplerState> m_samplerState;

    ComPtr<ID3D11Buffer> m_vertexConstantBuffer;
    ComPtr<ID3D11Buffer> m_pixelConstantBuffer;

    // 메쉬의 노멀 벡터 그리기
    ComPtr<ID3D11VertexShader> m_normalVertexShader;
    ComPtr<ID3D11PixelShader> m_normalPixelShader;
    ComPtr<ID3D11GeometryShader> m_normalGeometryShader;

    ComPtr<ID3D11Buffer> m_normalVertexConstantBuffer;
    ComPtr<ID3D11Buffer> m_normalPixelConstantBuffer;

};

}


