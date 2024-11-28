#include "Model.h"

#include <filesystem>

#include "GeometryGenerator.h"


namespace kusk {

#define MESH_TYPE_STR_SPHERE "sphere"
#define MESH_TYPE_STR_SQUARE "square"
#define MESH_TYPE_STR_SQUARE_GRID "sqaure_grid"
#define MESH_TYPE_STR_CYLINDER "cylinder"
#define MESH_TYPE_STR_BOX "box"

Model::Model(ComPtr<ID3D11Device>& device, ComPtr<ID3D11DeviceContext>& context,
			 const std::string& basePath, const std::string& filename) {
	this->Initialize(device, context, basePath, filename);
}

Model::Model(ComPtr<ID3D11Device>& device, ComPtr<ID3D11DeviceContext>& context,
			 const std::vector<MeshData>& meshes) {
	this->Initialize(device, context, meshes);
}

Model::Model(ComPtr<ID3D11Device>& device, ComPtr<ID3D11DeviceContext>& context,
			 const rapidjson::Value& value) {

	// Constants 데이터, Texture 초기화
	InitializeDataFromJson(device, context, value);
	// 메시 및 D3D 리소스 초기화
	InitializeFromJson(device, context, value);
	// Picking용 Bounding 박스 위치 초기화
	m_boundingSphere.Center = m_worldRow.Translation( );
}

void Model::Initialize(ComPtr<ID3D11Device>& device, ComPtr<ID3D11DeviceContext>& context,
			 const std::string& basePath, const std::string& filename) {

	m_modelingFilePath = basePath + filename;
	auto meshes = GeometryGenerator::ReadFromFile(basePath, filename);
	
	Initialize(device, context, meshes);
}

void Model::Initialize(ComPtr<ID3D11Device>& device, ComPtr<ID3D11DeviceContext>& context,
		const std::vector<MeshData>& meshes) {

	// ConstantBuffer 만들기
	D3D11Utils::CreateConstBuffer(device, m_meshConstsCPU, m_meshConstsGPU);
	D3D11Utils::CreateConstBuffer(device, m_materialConstsCPU, m_materialConstsGPU);

	Vector3 center(0.0f);
	uint64_t vertexCnt = 0;
	for (const auto& meshData : meshes) {
		auto newMesh = std::make_shared<Mesh>( );
		D3D11Utils::CreateVertexBuffer(device, meshData.vertices, newMesh->vertexBuffer);
		newMesh->indexCount = UINT(meshData.indices.size( ));
		newMesh->vertexCount = UINT(meshData.vertices.size( ));
		newMesh->stride = UINT(sizeof(Vertex));
		D3D11Utils::CreateIndexBuffer(device, meshData.indices, newMesh->indexBuffer);

		if (!meshData.albedoTextureFilename.empty( )) {
			D3D11Utils::CreateTexture(device, context, meshData.albedoTextureFilename, true,
				newMesh->albedoTexture, newMesh->albedoSRV);
			m_materialConstsCPU.useAlbedoMap = true;
		}

		if (!meshData.emissiveTextureFilename.empty( )) {
			D3D11Utils::CreateTexture(device, context, meshData.emissiveTextureFilename, true,
				newMesh->emissiveTexture, newMesh->emissiveSRV);
			m_materialConstsCPU.useEmissiveMap = true;
		}

		if (!meshData.normalTextureFilename.empty( )) {
			D3D11Utils::CreateTexture(device, context, meshData.normalTextureFilename, true,
				newMesh->normalTexture, newMesh->normalSRV);
			m_materialConstsCPU.useNormalMap = true;
		}

		if (!meshData.heightTextureFilename.empty( )) {
			D3D11Utils::CreateTexture(device, context, meshData.heightTextureFilename, true,
				newMesh->heightTexture, newMesh->heightSRV);
			m_meshConstsCPU.useHeightMap = true;
		}

		if (!meshData.aoTextureFilename.empty( )) {
			D3D11Utils::CreateTexture(device, context, meshData.aoTextureFilename, true,
				newMesh->aoTexture, newMesh->aoSRV);
			m_materialConstsCPU.useAOMap = true;
		}

		// GLTF 방식으로 Metallic과 Roughness를 한 텍스쳐에 넣음
		// Green : Roughness, Blue : Metallic(Metalness)
		if (!meshData.metallicTextureFilename.empty( ) ||
		   !meshData.roughnessTextureFilename.empty( )) {
			D3D11Utils::CreateMetallicRoughnessTexture(device, context, meshData.metallicTextureFilename,
				meshData.roughnessTextureFilename,
				newMesh->metallicRoughnessTexture,
				newMesh->metallicRoughnessSRV);
		}

		if (!meshData.metallicTextureFilename.empty( )) {
			m_materialConstsCPU.useMetallicMap = true;
		}

		if (!meshData.roughnessTextureFilename.empty( )) {
			m_materialConstsCPU.useRoughnessMap = true;
		}

		newMesh->vertexConstBuffer = m_meshConstsGPU;
		newMesh->pixelConstBuffer = m_materialConstsGPU;

		this->m_meshes.push_back(newMesh);

		// 중심 계산
		for (const auto& vertex : meshData.vertices) {
			center += vertex.position;
		}
		vertexCnt += meshData.vertices.size( );
	}

	// Bounding Sphere 구하기
	center /= static_cast< float >(vertexCnt);
	// 가장 먼 점에서 반지름 계산
	float radius = 0.0f;
	for (const auto& meshData : meshes) {
		for (const auto& vertex : meshData.vertices) {
			float distance = (vertex.position - center).Length( );
			if (distance > radius) {
				radius = distance;
			}
		}
	}
	m_boundingSphere = BoundingSphere(center, radius);
	cout << "center : (" << center.x << "," << center.y << "," << center.z << "), ";
	cout << "radius : " << radius << endl;
}

void Model::InitializeFromJson(ComPtr<ID3D11Device>& device, ComPtr<ID3D11DeviceContext>& context,
									   const rapidjson::Value& value) {

	std::string basePath;
	if (value.HasMember("modeling_file_dir")) {
		basePath = value[ "modeling_file_dir" ].GetString( );
	}

	std::string filename;
	if (value.HasMember("modeling_filename")) {
		filename = value[ "modeling_filename" ].GetString( );
	}

	m_modelingFilePath = basePath + filename;

	// 모델링 파일로 메쉬 초기화
	if (!m_modelingFilePath.empty( )) {
		auto meshes = GeometryGenerator::ReadFromFile(basePath, filename);
		Initialize(device, context, meshes);

		return;
	}

	// 메쉬 데이터로 초기화
	std::string meshType;
	if (value.HasMember("mesh_type")) {
		meshType = value[ "mesh_type" ].GetString( );
	}

	// Sphere
	if (meshType == MESH_TYPE_STR_SPHERE) {
		m_modelCreationParams.type = MESH_TYPE_SPHERE;

		if (value.HasMember("mesh_data")) {
			rapidjson::GenericObject data = value[ "mesh_data" ].GetObj();

			if (data.HasMember("radius")) {
				m_modelCreationParams.radius = data[ "radius" ].GetFloat( );
			}

			if (data.HasMember("num_slices")) {
				m_modelCreationParams.numSlices = data[ "num_slices" ].GetInt( );
			}

			if (data.HasMember("num_stacks")) {
				m_modelCreationParams.numStacks = data[ "num_stacks" ].GetInt( );
			}

			if (data.HasMember("tex_scale")) {
				m_modelCreationParams.tex_scale = JsonManager::ParseVector2(data[ "tex_scale" ]);
			}
		}

		//:CHECK:
		MeshData mesh = GeometryGenerator::MakeSphere(m_modelCreationParams.radius,
												   m_modelCreationParams.numSlices,
												   m_modelCreationParams.numStacks,
												   m_modelCreationParams.tex_scale);
		Initialize(device, context, vector{ mesh });
	}
	// Square
	else if (meshType == MESH_TYPE_STR_SQUARE) {
		m_modelCreationParams.type = MESH_TYPE_SQUARE;

		if (value.HasMember("mesh_data")) {
			rapidjson::GenericObject data = value[ "mesh_data" ].GetObj( );

			if (data.HasMember("scale")) {
				m_modelCreationParams.scale = data[ "scale" ].GetFloat( );
			}

			if (data.HasMember("tex_scale")) {
				m_modelCreationParams.tex_scale = JsonManager::ParseVector2(data[ "tex_scale" ]);
			}
		}

		//:CHECK:
		MeshData mesh = GeometryGenerator::MakeSquare(m_modelCreationParams.scale,
													  m_modelCreationParams.tex_scale);
		Initialize(device, context, vector{ mesh });
	}
	// Square grid
	else if (meshType == MESH_TYPE_STR_SQUARE_GRID) {
		m_modelCreationParams.type = MESH_TYPE_SQUARE_GRID;

		if (value.HasMember("mesh_data")) {
			rapidjson::GenericObject data = value[ "mesh_data" ].GetObj( );

			if (data.HasMember("num_slices")) {
				m_modelCreationParams.numSlices = data[ "num_slices" ].GetInt( );
			}

			if (data.HasMember("num_stacks")) {
				m_modelCreationParams.numStacks = data[ "num_stacks" ].GetInt( );
			}

			if (data.HasMember("scale")) {
				m_modelCreationParams.scale = data[ "scale" ].GetFloat( );
			}

			if (data.HasMember("tex_scale")) {
				m_modelCreationParams.tex_scale = JsonManager::ParseVector2(data[ "tex_scale" ]);
			}
		}
		
		//:CHECK:
		MeshData mesh = GeometryGenerator::MakeSquareGrid(m_modelCreationParams.numSlices,
														  m_modelCreationParams.numStacks, 
														  m_modelCreationParams.scale,
														  m_modelCreationParams.tex_scale);
		Initialize(device, context, vector{ mesh });
	}
	// Cylinder
	else if (meshType == MESH_TYPE_STR_CYLINDER) {
		m_modelCreationParams.type = MESH_TYPE_CYLINDER;

		if (value.HasMember("mesh_data")) {
			rapidjson::GenericObject data = value[ "mesh_data" ].GetObj( );

			if (data.HasMember("bottom_radius")) {
				m_modelCreationParams.bottomRadius = data[ "bottom_radius" ].GetFloat( );
			}

			if (data.HasMember("top_radius")) {
				m_modelCreationParams.topRadius = data[ "top_radius" ].GetFloat( );
			}

			if (data.HasMember("height")) {
				m_modelCreationParams.height = data[ "height" ].GetFloat( );
			}

			if (data.HasMember("num_slices")) {
				m_modelCreationParams.numSlices = data[ "num_slices" ].GetInt( );
			}
		}
		
		//:CHECK:
		MeshData mesh = GeometryGenerator::MakeCylinder(m_modelCreationParams.bottomRadius,
														  m_modelCreationParams.topRadius,
														  m_modelCreationParams.height,
														  m_modelCreationParams.numSlices);
		Initialize(device, context, vector{ mesh });
	}
	// Box
	else if (meshType == MESH_TYPE_STR_BOX) {
		m_modelCreationParams.type = MESH_TYPE_BOX;

		if (value.HasMember("mesh_data")) {
			rapidjson::GenericObject data = value[ "mesh_data" ].GetObj( );

			if (data.HasMember("scale")) {
				m_modelCreationParams.scale = data[ "scale" ].GetFloat( );
			}
		}

		//:CHECK:
		MeshData mesh = GeometryGenerator::MakeBox(m_modelCreationParams.scale);
		Initialize(device, context, vector{ mesh });
	}

	// 텍스쳐만 따로 리소스 생성
	if (!m_albedoTextureFilePath.empty( )) {
		D3D11Utils::CreateTexture(device, context, m_albedoTextureFilePath, true,
			m_meshes[ 0 ]->albedoTexture, m_meshes[ 0 ]->albedoSRV);
		m_materialConstsCPU.useAlbedoMap = true;
	}

	if (!m_emissiveTextureFilePath.empty( )) {
		D3D11Utils::CreateTexture(device, context, m_emissiveTextureFilePath, true,
			m_meshes[ 0 ]->emissiveTexture, m_meshes[ 0 ]->emissiveSRV);
		m_materialConstsCPU.useEmissiveMap = true;
	}

	if (!m_normalTextureFilePath.empty( )) {
		D3D11Utils::CreateTexture(device, context, m_normalTextureFilePath, true,
			m_meshes[ 0 ]->normalTexture, m_meshes[ 0 ]->normalSRV);
		m_materialConstsCPU.useNormalMap = true;
	}

	if (!m_heightTextureFilePath.empty( )) {
		D3D11Utils::CreateTexture(device, context, m_heightTextureFilePath, true,
			m_meshes[ 0 ]->heightTexture, m_meshes[ 0 ]->heightSRV);
		m_meshConstsCPU.useHeightMap = true;
	}

	if (!m_aoTextureFilePath.empty( )) {
		D3D11Utils::CreateTexture(device, context, m_aoTextureFilePath, true,
			m_meshes[ 0 ]->aoTexture, m_meshes[ 0 ]->aoSRV);
		m_materialConstsCPU.useAOMap = true;
	}

	// GLTF 방식으로 Metallic과 Roughness를 한 텍스쳐에 넣음
	// Green : Roughness, Blue : Metallic(Metalness)
	if (!m_metallicTextureFilePath.empty( ) ||
	   !m_roughnessTextureFilePath.empty( )) {
		D3D11Utils::CreateMetallicRoughnessTexture(device, context, m_metallicTextureFilePath,
			m_roughnessTextureFilePath,
			m_meshes[ 0 ]->metallicRoughnessTexture,
			m_meshes[ 0 ]->metallicRoughnessSRV);
	}

	if (!m_metallicTextureFilePath.empty( )) {
		m_materialConstsCPU.useMetallicMap = true;
	}

	if (!m_roughnessTextureFilePath.empty( )) {
		m_materialConstsCPU.useRoughnessMap = true;
	}
}

void Model::InitializeDataFromJson(ComPtr<ID3D11Device>& device, ComPtr<ID3D11DeviceContext>& context, 
										 const rapidjson::Value& value) {

	if (value.HasMember("texture_albedo")) {
		m_albedoTextureFilePath = value[ "texture_albedo" ].GetString( );
	}
	if (value.HasMember("texture_emissive")) {
		m_emissiveTextureFilePath = value[ "texture_emissive" ].GetString( );
	}
	if (value.HasMember("texture_normal")) {
		m_normalTextureFilePath = value[ "texture_normal" ].GetString( );
	}
	if (value.HasMember("texture_height")) {
		m_heightTextureFilePath = value[ "texture_height" ].GetString( );
	}
	if (value.HasMember("texture_ao")) {
		m_aoTextureFilePath = value[ "texture_ao" ].GetString( );
	}
	if (value.HasMember("texture_metallic")) {
		m_metallicTextureFilePath = value[ "texture_metallic" ].GetString( );
	}
	if (value.HasMember("texture_roughness")) {
		m_roughnessTextureFilePath = value[ "texture_roughness" ].GetString( );
	}

	if (value.HasMember("world_row_matrix")) {
		m_worldRow = JsonManager::ParseMatrix(value[ "world_row_matrix" ]);
		m_worldITRow = m_worldRow;
		m_worldITRow.Translation(Vector3(0.0f));
		m_worldITRow = m_worldITRow.Invert( ).Transpose( );

		m_meshConstsCPU.world = m_worldRow.Transpose( );
		m_meshConstsCPU.worldIT = m_worldITRow.Transpose( );
	}

	if (value.HasMember("albedo_factor")) {
		m_materialConstsCPU.albedoFactor = JsonManager::ParseVector3(value[ "albedo_factor" ]);
	}
	if (value.HasMember("roughness_factor")) {
		m_materialConstsCPU.roughnessFactor = value[ "roughness_factor" ].GetFloat( );
	}
	if (value.HasMember("metallic_factor")) {
		m_materialConstsCPU.metallicFactor = value[ "metallic_factor" ].GetFloat( );
	}
	if (value.HasMember("emission_factor")) {
		m_materialConstsCPU.emissionFactor = JsonManager::ParseVector3(value[ "emission_factor" ]);
	}
	if (value.HasMember("height_scale")) {
		m_meshConstsCPU.heightScale = value[ "height_scale" ].GetFloat( );
	}
}

rapidjson::Value Model::ToJson(rapidjson::Document::AllocatorType& allocator) const {

	rapidjson::Value value(rapidjson::kObjectType);
	rapidjson::Value strValue;

	if (m_modelingFilePath.length( ) > 0) {
		std::filesystem::path filePath(m_modelingFilePath);
		std::string dirPath = filePath.parent_path( ).string( ) + "\\";
		std::string fileName = filePath.filename( ).string( );

		strValue.SetString(dirPath.c_str( ), allocator);
		value.AddMember("modeling_file_dir", strValue, allocator);
		strValue.SetString(fileName.c_str( ), allocator);
		value.AddMember("modeling_filename", strValue, allocator);
	}

	MESH_TYPE type = m_modelCreationParams.type;
	if (type == MESH_TYPE_SPHERE) {
		value.AddMember("mesh_type", MESH_TYPE_STR_SPHERE, allocator);

		rapidjson::Value meshData(rapidjson::kObjectType);
		meshData.AddMember("radius", m_modelCreationParams.radius, allocator);
		meshData.AddMember("num_slices", m_modelCreationParams.numSlices, allocator);
		meshData.AddMember("num_stacks", m_modelCreationParams.numStacks, allocator);
		meshData.AddMember("tex_scale",
				JsonManager::Vector2ToJson(m_modelCreationParams.tex_scale, allocator),
				allocator);

		value.AddMember("mesh_data", meshData, allocator);
	}
	else if (type == MESH_TYPE_SQUARE) {
		value.AddMember("mesh_type", MESH_TYPE_STR_SQUARE, allocator);

		rapidjson::Value meshData(rapidjson::kObjectType);
		meshData.AddMember("scale", m_modelCreationParams.scale, allocator);
		meshData.AddMember("tex_scale",
				JsonManager::Vector2ToJson(m_modelCreationParams.tex_scale, allocator),
				allocator);

		value.AddMember("mesh_data", meshData, allocator);
	}
	else if (type == MESH_TYPE_SQUARE_GRID) {
		value.AddMember("mesh_type", MESH_TYPE_STR_SQUARE_GRID, allocator);

		rapidjson::Value meshData(rapidjson::kObjectType);
		meshData.AddMember("num_slices", m_modelCreationParams.numSlices, allocator);
		meshData.AddMember("num_stacks", m_modelCreationParams.numStacks, allocator);
		meshData.AddMember("scale", m_modelCreationParams.scale, allocator);
		meshData.AddMember("tex_scale",
				JsonManager::Vector2ToJson(m_modelCreationParams.tex_scale, allocator),
				allocator);

		value.AddMember("mesh_data", meshData, allocator);
	}
	else if (type == MESH_TYPE_CYLINDER) {
		value.AddMember("mesh_type", MESH_TYPE_STR_CYLINDER, allocator);

		rapidjson::Value meshData(rapidjson::kObjectType);
		meshData.AddMember("bottom_radius", m_modelCreationParams.bottomRadius, allocator);
		meshData.AddMember("top_radius", m_modelCreationParams.topRadius, allocator);
		meshData.AddMember("height", m_modelCreationParams.height, allocator);
		meshData.AddMember("num_slices", m_modelCreationParams.numSlices, allocator);

		value.AddMember("mesh_data", meshData, allocator);
	}
	else if (type == MESH_TYPE_BOX) {
		value.AddMember("mesh_type", MESH_TYPE_STR_BOX, allocator);

		rapidjson::Value meshData(rapidjson::kObjectType);
		meshData.AddMember("scale", m_modelCreationParams.scale, allocator);

		value.AddMember("mesh_data", meshData, allocator);
	}

	if (type != MESH_TYPE_NONE) {
		if (!m_albedoTextureFilePath.empty( )) {
			strValue.SetString(m_albedoTextureFilePath.c_str( ), allocator);
			value.AddMember("texture_albedo", strValue, allocator);
		}

		if (!m_emissiveTextureFilePath.empty( )) {
			strValue.SetString(m_emissiveTextureFilePath.c_str( ), allocator);
			value.AddMember("texture_emissive", strValue, allocator);
		}

		if (!m_normalTextureFilePath.empty( )) {
			strValue.SetString(m_normalTextureFilePath.c_str( ), allocator);
			value.AddMember("texture_normal", strValue, allocator);
		}

		if (!m_heightTextureFilePath.empty( )) {
			strValue.SetString(m_heightTextureFilePath.c_str( ), allocator);
			value.AddMember("texture_height", strValue, allocator);
		}

		if (!m_aoTextureFilePath.empty( )) {
			strValue.SetString(m_aoTextureFilePath.c_str( ), allocator);
			value.AddMember("texture_ao", strValue, allocator);
		}

		if (!m_metallicTextureFilePath.empty( )) {
			strValue.SetString(m_metallicTextureFilePath.c_str( ), allocator);
			value.AddMember("texture_metallic", strValue, allocator);
		}

		if (!m_roughnessTextureFilePath.empty( )) {
			strValue.SetString(m_roughnessTextureFilePath.c_str( ), allocator);
			value.AddMember("texture_roughness", strValue, allocator);
		}

		// 공통
		value.AddMember("world_row_matrix",
						JsonManager::MatrixToJson(m_worldRow, allocator), allocator);
		value.AddMember("albedo_factor",
						JsonManager::Vector3ToJson(m_materialConstsCPU.albedoFactor, allocator), allocator);
		value.AddMember("roughness_factor", m_materialConstsCPU.roughnessFactor, allocator);
		value.AddMember("metallic_factor", m_materialConstsCPU.metallicFactor, allocator);
		value.AddMember("emission_factor",
						JsonManager::Vector3ToJson(m_materialConstsCPU.emissionFactor, allocator), allocator);
		value.AddMember("height_scale", m_meshConstsCPU.heightScale, allocator);
	}

	return value;
}

void Model::UpdateConstantBuffers(ComPtr<ID3D11Device>& device, ComPtr<ID3D11DeviceContext>& context) {
	if (m_isVisible) {
		D3D11Utils::UpdateBuffer(device, context, m_meshConstsCPU, m_meshConstsGPU);
		D3D11Utils::UpdateBuffer(device, context, m_materialConstsCPU, m_materialConstsGPU);
	}
}

void Model::Render(ComPtr<ID3D11DeviceContext>& context) {
	if (m_isVisible) {
		for (const auto& mesh : m_meshes) {
			context->VSSetConstantBuffers(0, 1, mesh->vertexConstBuffer.GetAddressOf( ));
			context->PSSetConstantBuffers(0, 1, mesh->pixelConstBuffer.GetAddressOf( ));

			context->VSSetShaderResources(0, 1, mesh->heightSRV.GetAddressOf( ));

			// 물체 렌더링할 때 여러가지 텍스쳐 사용 (t0 부터 시작)
			vector<ID3D11ShaderResourceView*> resViews = {
				mesh->albedoSRV.Get( ), mesh->normalSRV.Get( ), mesh->aoSRV.Get( ),
				mesh->metallicRoughnessSRV.Get( ), mesh->emissiveSRV.Get( ) };

			context->PSSetShaderResources(0, UINT(resViews.size( )), resViews.data( ));
			context->IASetVertexBuffers(0, 1, mesh->vertexBuffer.GetAddressOf( ), &mesh->stride, &mesh->offset);
			context->IASetIndexBuffer(mesh->indexBuffer.Get( ), DXGI_FORMAT_R32_UINT, 0);
			context->DrawIndexed(mesh->indexCount, 0, 0);
		}
	}
}

void Model::RenderNormals(ComPtr<ID3D11DeviceContext>& context) {
	for (const auto& mesh : m_meshes) {
		context->GSSetConstantBuffers(0, 1, m_meshConstsGPU.GetAddressOf( ));
		context->IASetVertexBuffers(0, 1, mesh->vertexBuffer.GetAddressOf( ),
									&mesh->stride, &mesh->offset);
		context->Draw(mesh->vertexCount, 0);
	}
}

void Model::UpdateWorldRow(const Matrix& worldRow) {
	this->m_worldRow = worldRow;
	this->m_worldITRow = worldRow;
	m_worldITRow.Translation(Vector3(0.0f));
	m_worldITRow = m_worldITRow.Invert( ).Transpose( );

	m_meshConstsCPU.world =	worldRow.Transpose();
	m_meshConstsCPU.worldIT = m_worldITRow.Transpose();

	m_boundingSphere.Center = worldRow.Translation( );
}

} // namespace kusk