#pragma once

#include "ConstantBuffers.h"
#include "D3D11Utils.h"
#include "Mesh.h"
#include "MeshData.h"



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

	void Initialize(ComPtr<ID3D11Device>& device, ComPtr<ID3D11DeviceContext>& context,
		const std::string& basePath, const std::string& filename);
	void Initialize(ComPtr<ID3D11Device>& device, ComPtr<ID3D11DeviceContext>& context,
		const std::vector<MeshData>& meshes);

	void UpdateConstantBuffers(ComPtr<ID3D11Device>& device, ComPtr<ID3D11DeviceContext>& context);

	void Render(ComPtr<ID3D11DeviceContext>& context);

	void RenderNormals(ComPtr<ID3D11DeviceContext>& context);

	void UpdateWorldRow(const Matrix& worldRow);

public:

	BoundingSphere m_boundingSphere;

	Matrix m_worldRow = Matrix( );		// Model(Object) To World 행렬
	Matrix m_worldITRow = Matrix( );	// InverseTranspose

	MeshConstants m_meshConstsCPU;
	MaterialConstants m_materialConstsCPU;

	bool m_drawNormals = false;
	bool m_isVisible = true;
	bool m_castShadow = true;
	bool m_isPickable = true;

	std::vector<shared_ptr<Mesh>> m_meshes;

private:
	ComPtr<ID3D11Buffer> m_meshConstsGPU;
	ComPtr<ID3D11Buffer> m_materialConstsGPU;
};
} // namespace kusk



