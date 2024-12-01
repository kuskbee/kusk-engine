#include "BillboardPoints.h"

#include <numeric>

namespace kusk {

void BillboardPoints::Initialize(ComPtr<ID3D11Device>& device,
								 ComPtr<ID3D11DeviceContext>& context,
								 const std::vector<Vector4>& points,
								 const float width,
								 std::vector<std::string> filenames) {
	
	m_points = points;
	m_filenames = filenames;
	D3D11Utils::CreateVertexBuffer(device, points, m_vertexBuffer);
	m_indexCount = uint32_t(points.size( ));
	m_constantData.width = width;
	m_constantData.textureCnt = filenames.size();
	D3D11Utils::CreateConstBuffer(device, m_constantData, m_constantBuffer);
	D3D11Utils::CreateTextureArray(device, context, filenames, m_texArray, m_texArraySRV);
}

void BillboardPoints::Render(ComPtr<ID3D11DeviceContext>& context) {

	context->PSSetShaderResources(0, 1, m_texArraySRV.GetAddressOf( ));
	
	context->VSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf( ));
	context->PSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf( ));

	// Geometry Shader
	context->GSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf( ));

	UINT stride = sizeof(Vector4); // vertex size
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf( ), &stride, &offset);

	// POINTLIST는 연결관계가 필요없기 때문에 DrawIndexed() 대신 Draw() 사용
	context->Draw(m_indexCount, 0);
}

void BillboardPoints::UpdateVertexBuffer(ComPtr<ID3D11Device>& device) {
	D3D11Utils::CreateVertexBuffer(device, m_points, m_vertexBuffer);
	m_indexCount = uint32_t(m_points.size( ));
}

void BillboardPoints::UpdateConstBuffer(ComPtr<ID3D11Device>& device, ComPtr<ID3D11DeviceContext>& context) {
	D3D11Utils::UpdateBuffer(device, context, m_constantData, m_constantBuffer);
}

} // namespace kusk