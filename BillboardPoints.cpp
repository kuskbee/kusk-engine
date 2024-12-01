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

rapidjson::Value BillboardPoints::BillboardToJSON(rapidjson::Document::AllocatorType& allocator)
{
	rapidjson::Value value(rapidjson::kObjectType);
	rapidjson::Value strValue;

	value.AddMember("albedo", JsonManager::Vector3ToJson(m_constantData.albedoFactor, allocator), allocator);
	value.AddMember("roughness", m_constantData.roughnessFactor, allocator);
	value.AddMember("metallic", m_constantData.metallicFactor, allocator);
	value.AddMember("width", m_constantData.width, allocator);

	rapidjson::Value files(rapidjson::kArrayType);
	for (std::string& s : m_filenames) {
		strValue.SetString(s.c_str( ), allocator);
		files.PushBack(strValue, allocator);
	}
	value.AddMember("texture_list", files, allocator);

	rapidjson::Value points(rapidjson::kArrayType);
	for (Vector4& v : m_points) {
		points.PushBack(JsonManager::Vector4ToJson(v, allocator), allocator);
	}
	value.AddMember("points", points, allocator);

	return value;
}
void BillboardPoints::InitializeFromJSON(ComPtr<ID3D11Device>& device,
						ComPtr<ID3D11DeviceContext>& context,
						rapidjson::Value& value)
{
	if (value.HasMember("albedo")) {
		m_constantData.albedoFactor = JsonManager::ParseVector3(value[ "albedo" ]);
	}
	if (value.HasMember("roughness")) {
		m_constantData.roughnessFactor = value[ "roughness" ].GetFloat( );
	}
	if (value.HasMember("metallic")) {
		m_constantData.metallicFactor = value[ "metallic" ].GetFloat( );
	}
	if (value.HasMember("width")) {
		m_constantData.width = value[ "width" ].GetFloat( );
	}
	if (value.HasMember("texture_list")) {
		rapidjson::GenericArray files = value[ "texture_list" ].GetArray( );

		for (rapidjson::Value& file : files) {
			m_filenames.push_back(file.GetString( ));
		}
		
	}
	if (value.HasMember("points")) {
		rapidjson::GenericArray points = value[ "points" ].GetArray( );

		for (rapidjson::Value& point : points) {
			m_points.push_back(JsonManager::ParseVector4(point));
		}
	}

	Initialize(device, context, m_points, m_constantData.width, m_filenames);
}

} // namespace kusk