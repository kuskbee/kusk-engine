#include "BillboardPoints.h"

#include <numeric>

namespace kusk {

void BillboardPoints::Initialize(ComPtr<ID3D11Device>& device,
								 const std::vector<Vector4>& points,
								 const float width,
								 const std::wstring pixelShaderFilename,
								 std::vector<std::string> filenames) {
	// Sampler 만들기
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	device->CreateSamplerState(&sampDesc, m_samplerState.GetAddressOf( ));

	D3D11Utils::CreateVertexBuffer(device, points, m_vertexBuffer);

	m_indexCount = uint32_t(points.size( ));

	m_constantData.width = width;
	D3D11Utils::CreateConstantBuffer(device, m_constantData, m_constantBuffer);

	// Geometry shader 초기화하기
	D3D11Utils::CreateGeometryShader(device, L"BillboardPointsGeometryShader.hlsl", m_normalGeometryShader);

	vector<D3D11_INPUT_ELEMENT_DESC> inputElements = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}};

	D3D11Utils::CreateVertexShaderAndInputLayout(device, L"BillboardPointsVertexShader.hlsl", inputElements,
												 m_vertexShader, m_inputLayout);
	D3D11Utils::CreatePixelShader(device, pixelShaderFilename, m_pixelShader);

	D3D11Utils::CreateTextureArray(device, filenames, m_texArray, m_texArraySRV);
}

void BillboardPoints::Render(ComPtr<ID3D11DeviceContext>& context) {

	context->VSSetShader(m_vertexShader.Get( ), 0, 0);
	context->PSSetSamplers(0, 1, m_samplerState.GetAddressOf( ));
	context->PSSetShader(m_pixelShader.Get( ), 0, 0);
	context->PSSetShaderResources(0, 1, m_texArraySRV.GetAddressOf( ));
	
	context->VSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf( ));
	context->PSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf( ));

	// Geometry Shader
	context->GSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf( ));
	context->GSSetShader(m_normalGeometryShader.Get( ), 0, 0);

	context->IASetInputLayout(m_inputLayout.Get( ));

	UINT stride = sizeof(Vector4); // vertex size
	UINT offset = 0;

	context->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf( ), &stride, &offset);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	// POINTLIST는 연결관계가 필요없기 때문에 DrawIndexed() 대신 Draw() 사용
	context->Draw(m_indexCount, 0);

	// Geometry Shader를 사용하지 않는 다른 물체들을 위해 nullptr로 설정
	context->GSSetShader(nullptr, 0, 0);

}

} // namespace kusk