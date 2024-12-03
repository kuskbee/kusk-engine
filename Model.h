#pragma once

#include "ConstantBuffers.h"
#include "D3D11Utils.h"
#include "Mesh.h"
#include "MeshData.h"
#include "JsonManager.h"
#include "MouseControlState.h"


// 참고: DirectX-Graphics-Samples
// https://github.com/microsoft/DirectX-Graphics-Samples/blob/master/MiniEngine/Model/Model.h

namespace kusk
{
using DirectX::BoundingSphere;

class Model
{
public:
	Model( ) {}
	Model(ComPtr<ID3D11Device>& device, ComPtr<ID3D11DeviceContext>& context,
		const std::string& basePath, const std::string& filename);
	Model(ComPtr<ID3D11Device>& device, ComPtr<ID3D11DeviceContext>& context,
		const std::vector<MeshData>& meshes);
	Model(ComPtr<ID3D11Device>& device, ComPtr<ID3D11DeviceContext>& context,
		const rapidjson::Value& value);

	void Initialize(ComPtr<ID3D11Device>& device, ComPtr<ID3D11DeviceContext>& context,
		const std::string& basePath, const std::string& filename);
	void Initialize(ComPtr<ID3D11Device>& device, ComPtr<ID3D11DeviceContext>& context,
		const std::vector<MeshData>& meshes);

	// Json
	void InitializeFromJson(ComPtr<ID3D11Device>& device, ComPtr<ID3D11DeviceContext>& context,
		const rapidjson::Value& value);
	void InitializeDataFromJson(ComPtr<ID3D11Device>& device, ComPtr<ID3D11DeviceContext>& context,
		const rapidjson::Value& value);
	rapidjson::Value ToJson(rapidjson::Document::AllocatorType& allocator) const;

	//
	void UpdateConstantBuffers(ComPtr<ID3D11Device>& device, ComPtr<ID3D11DeviceContext>& context);

	void Render(ComPtr<ID3D11DeviceContext>& context);

	void RenderNormals(ComPtr<ID3D11DeviceContext>& context);

	void UpdateWorldRow(const Matrix& worldRow);

public:

	BoundingSphere m_originBoundingSphere;
	BoundingSphere m_boundingSphere;

	Matrix m_worldRow = Matrix( );		// Model(Object) To World 행렬
	Matrix m_worldITRow = Matrix( );	// InverseTranspose

	MeshConstants m_meshConstsCPU;
	MaterialConstants m_materialConstsCPU;

	bool m_drawNormals = false;
	bool m_isVisible = true;
	bool m_castShadow = true;
	bool m_isPickable = true;
	bool m_isMirror = false;
	bool m_isFixed = false;

	std::vector<shared_ptr<Mesh>> m_meshes;

	// JSON 저장용
	ModelCreationParams m_modelCreationParams;
	std::string m_modelingFilePath;
	
	// Mesh가 하나 있는 Model만 아래 내용 저장.
	// (Mesh가 여러 개일 경우 Modelling 파일로 부터 가져온다고 간주)
	std::string m_albedoTextureFilePath;
	std::string m_emissiveTextureFilePath;
	std::string m_normalTextureFilePath;
	std::string m_heightTextureFilePath;
	std::string m_aoTextureFilePath; // Ambient Occlusion
	std::string m_metallicTextureFilePath;
	std::string m_roughnessTextureFilePath;

	// Picking 상태 저장
	MouseControlState m_mouseState;

private:
	ComPtr<ID3D11Buffer> m_meshConstsGPU;
	ComPtr<ID3D11Buffer> m_materialConstsGPU;
};
} // namespace kusk



