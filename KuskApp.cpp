#include "KuskApp.h"

#include <directxtk/DDSTextureLoader.h>
#include <vector>

namespace kusk {

using namespace std;
using namespace DirectX;

KuskApp::KuskApp() : AppBase(), m_basicPixelConstantBufferData() {}

void KuskApp::InitializeCubeMapping( ) {
	// texassemble.exe cube -w 2048 -h 2048 -o saintpeters.dds posx.jpg negx.jpg
	// posy.jpg negy.jpg posz.jpg negz.jpg
	// texassemble.exe cube -w 256 -h 256 -o
	// skybox.dds right.jpg left.jpg top.jpg bottom.jpg front.jpg back.jpg -y
	// https://github.com/Microsoft/DirectXTex/wiki/Texassemble

	auto skyboxFilename = L"./CubemapTextures/skybox.dds";
	auto nightPathFilename = L"./CubemapTextures/HumusTextures/NightPath.dds";
	
	auto atribumDiffuseFilename = L"./CubemapTextures/Atrium_diffuseIBL.dds";
	auto atribumSpecularFilename = L"./CubemapTextures/Atrium_specularIBL.dds";

	auto stonewallSpecularFilename = L"./CubemapTextures/Stonewall_specularIBL.dds";
	auto stonewallDiffuseFilename = L"./CubemapTextures/Stonewall_diffuseIBL.dds";

	auto sanFilename = L"./CubemapTextures/Output/output.dds";
	auto sanDiffuseFilename = L"./CubemapTextures/Output/output_diffuse.dds";
	auto sanSpecularFilename = L"./CubemapTextures/Output/output_specular.dds";


	// .dds 파일 읽어들여서 초기화
	CreateCubemapTexture(atribumDiffuseFilename, m_cubeMapping.diffuseResourceView);
	CreateCubemapTexture(atribumSpecularFilename, m_cubeMapping.specularResourceView);

	m_cubeMapping.cubeMesh = std::make_shared<Mesh>( );

	m_basicVertexConstantBufferData.model = Matrix( );
	m_basicVertexConstantBufferData.view = Matrix( );
	m_basicVertexConstantBufferData.proj = Matrix( );
	ComPtr<ID3D11Buffer> vertexConstantBuffer;
	ComPtr<ID3D11Buffer> pixelConstantBuffer;
	AppBase::CreateConstantBuffer(m_basicVertexConstantBufferData,
								  m_cubeMapping.cubeMesh->vertexConstantBuffer);
	AppBase::CreateConstantBuffer(m_basicPixelConstantBufferData,
								  m_cubeMapping.cubeMesh->pixelConstantBuffer);

	// 커다란 박스 초기화
	// - 세상이 커다란 박스 안에 갇혀있는 구조
	// - D3D11_CULL_MODE::D3D11_CULL_NONE 또는 삼각형 뒤집기
	// - 예시) std::reverse(myvector.begin(), myvector.end())
	//MeshData cubeMeshData = GeometryGenerator::MakeSphere(20.0f, 3, 3);
	//MeshData cubeMeshData = GeometryGenerator::MakeBox(100.0f);
	MeshData cubeMeshData = GeometryGenerator::MakeBox(20.0f);
	std::reverse(cubeMeshData.indices.begin( ), cubeMeshData.indices.end( ));

	AppBase::CreateVertexBuffer(cubeMeshData.vertices,
								m_cubeMapping.cubeMesh->vertexBuffer);
	m_cubeMapping.cubeMesh->indexCount = UINT(cubeMeshData.indices.size( ));
	AppBase::CreateIndexBuffer(cubeMeshData.indices,
							   m_cubeMapping.cubeMesh->indexBuffer);

	// 쉐이더 초기화

	// 다른 쉐이더와 동일한 InputLayout
	// 실제로는 "POSITION"만 사용
	vector<D3D11_INPUT_ELEMENT_DESC> basicInputElements = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 4 * 3, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 4 * 3 + 4 * 3, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	AppBase::CreateVertexShaderAndInputLayout(L"CubeMappingVertexShader.hlsl",
											 basicInputElements, m_cubeMapping.vertexShader, m_cubeMapping.inputLayout);
	AppBase::CreatePixelShader(L"CubeMappingPixelShader.hlsl", m_cubeMapping.pixelShader);

	// 기타
	// - 텍스쳐 샘플러도 다른 텍스쳐와 같이 사용

}

bool KuskApp::Initialize() {

	if (!AppBase::Initialize())
		return false;

	InitializeCubeMapping( );

	// Texture sampler 만들기
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Create the Sample State
	m_device->CreateSamplerState(&sampDesc, m_samplerState.GetAddressOf( ));

	// Geometry 정의
	// 젤다 모델 다운로드 경로
	// https://f3d.app/doc/GALLERY.html
	// you can download them here. 클릭

	vector<MeshData> meshes = { GeometryGenerator::MakeSphere(0.3f, 100, 100) };
	meshes[ 0 ].textureFilename = "ojwD8.jpg";

	// auto meshes =
	// GeometryGenerator::ReadFromFile("C:/workspaces/portfolio/models/",
	// "monkey2.obj");
	
	/*auto meshes =
	 GeometryGenerator::ReadFromFile("C:/workspaces/portfolio/models/zelda/",
	 "zeldaPosed001.fbx");*/

	// GLTF 샘플들
	// https://github.com/KhronosGroup/glTF-Sample-Models

	/*auto meshes = GeometryGenerator::ReadFromFile(
		"C:/workspaces/portfolio/models/glTF-Sample-Models/2.0/DamagedHelmet/"
		"glTF/",
		"DamagedHelmet.gltf");*/

	 /*auto meshes =
	     GeometryGenerator::ReadFromFile("C:/workspaces/portfolio/models/glTF-Sample-Models/2.0/ABeautifulGame/glTF/",
	     "ABeautifulGame.gltf");*/

	// auto meshes =
	// GeometryGenerator::ReadFromFile("C:/workspaces/portfolio/models/glTF-Sample-Models/2.0/ToyCar/glTF/",
	//                                               "ToyCar.gltf");

	//vector<MeshData> meshes = {
	//	GeometryGenerator::ReadFromFile("./", "stanford_dragon.stl") };

	// ConstantBuffer 만들기 (하나 만들어서 공유)
	m_basicVertexConstantBufferData.model = Matrix();
	m_basicVertexConstantBufferData.view = Matrix();
	m_basicVertexConstantBufferData.proj = Matrix();
	ComPtr<ID3D11Buffer> vertexConstantBuffer;
	ComPtr<ID3D11Buffer> pixelConstantBuffer;
	AppBase::CreateConstantBuffer(m_basicVertexConstantBufferData, vertexConstantBuffer);
	
	AppBase::CreateConstantBuffer(m_basicPixelConstantBufferData, pixelConstantBuffer);

	for (const auto& meshData : meshes) {
		auto newMesh = std::make_shared<Mesh>( );
		AppBase::CreateVertexBuffer(meshData.vertices, newMesh->vertexBuffer);
		newMesh->indexCount = UINT(meshData.indices.size( ));
		AppBase::CreateIndexBuffer(meshData.indices, newMesh->indexBuffer);

		if (!meshData.textureFilename.empty( )) {
			cout << meshData.textureFilename << endl;
			AppBase::CreateTexture(meshData.textureFilename, newMesh->texture,
								 newMesh->textureResourceView);
		}

		newMesh->vertexConstantBuffer = vertexConstantBuffer;
		newMesh->pixelConstantBuffer = pixelConstantBuffer;

		this->m_meshes.push_back(newMesh);
	}


	// Make Shaders
	vector<D3D11_INPUT_ELEMENT_DESC> basicInputElements = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 4 * 3, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 4 * 3 + 4 * 3, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	AppBase::CreateVertexShaderAndInputLayout(L"BasicVertexShader.hlsl", basicInputElements,
												m_basicVertexShader, m_basicInputLayout);

	AppBase::CreatePixelShader(L"BasicPixelShader.hlsl", m_basicPixelShader);

	// 노멀 벡터 그리기
	// 단순화를 위해 InputLayout은 BasicVertexShader와 같이 사용.
	m_normalLines = std::make_shared<Mesh>( );

	std::vector<Vertex> normalVertices;
	std::vector<uint32_t> normalIndices;

	// 여러 메쉬의 normal 들을 하나로 합치기
	size_t offset = 0;
	for (const auto& meshData : meshes) {
		for (size_t i = 0; i < meshData.vertices.size( ); i++) {
			Vertex v = meshData.vertices[ i ];

			v.texcoord.x = 0.0f; // 시작점
			normalVertices.push_back(v);

			v.texcoord.x = 1.0f; // 끝점
			normalVertices.push_back(v);

			normalIndices.push_back(uint32_t(2 * (i + offset)));
			normalIndices.push_back(uint32_t(2 * (i + offset) + 1));
		}
		offset += meshData.vertices.size( );
	}
	
	AppBase::CreateVertexBuffer(normalVertices, m_normalLines->vertexBuffer);
	m_normalLines->indexCount = UINT(normalIndices.size());
	AppBase::CreateIndexBuffer(normalIndices, m_normalLines->indexBuffer);
	AppBase::CreateConstantBuffer(m_normalVertexConstantBufferData, m_normalLines->vertexConstantBuffer);

	AppBase::CreateVertexShaderAndInputLayout(
		L"NormalVertexShader.hlsl", basicInputElements, m_normalVertexShader,
		m_basicInputLayout);

	AppBase::CreatePixelShader(L"NormalPixelShader.hlsl", m_normalPixelShader);

	return true;
}

void KuskApp::Update(float dt) {
	
	using namespace DirectX;
	
	// 모델의 변환
	m_basicVertexConstantBufferData.model = Matrix::CreateScale(m_modelScaling) * 
								 Matrix::CreateRotationY(m_modelRotation.y) *
								 Matrix::CreateRotationX(m_modelRotation.x) *
								 Matrix::CreateRotationZ(m_modelRotation.z) *
								 Matrix::CreateTranslation(m_modelTranslation);
	m_basicVertexConstantBufferData.model = m_basicVertexConstantBufferData.model.Transpose();

	m_basicVertexConstantBufferData.invTranspose = m_basicVertexConstantBufferData.model;
	m_basicVertexConstantBufferData.invTranspose.Translation(Vector3(0.0f));
	m_basicVertexConstantBufferData.invTranspose = m_basicVertexConstantBufferData.invTranspose.Transpose( ).Invert( );

	// 시점 변환
	//XMMatrixLookToLH(m_viewEyePos, m_viewEyeDir, m_viewUp);
	m_basicVertexConstantBufferData.view =
		Matrix::CreateRotationY(m_viewRot.y) *
		Matrix::CreateRotationX(m_viewRot.x) *
		Matrix::CreateTranslation(0.0f, 0.0f, 2.0f);

	m_basicPixelConstantBufferData.eyeWorld = Vector3::Transform(
		Vector3(0.0f), m_basicVertexConstantBufferData.view.Invert( ));
		
	m_basicVertexConstantBufferData.view = m_basicVertexConstantBufferData.view.Transpose();

	// 프로젝션
	const float aspect = AppBase::GetAspectRatio( );
	if (m_usePerspectiveProjection) {
		m_basicVertexConstantBufferData.proj =
			XMMatrixPerspectiveFovLH(XMConvertToRadians(m_projFovAngleY), aspect, m_nearZ, m_farZ);
	}
	else {
		m_basicVertexConstantBufferData.proj =
			XMMatrixOrthographicOffCenterLH(-aspect, aspect, -1.0f, 1.0f, m_nearZ, m_farZ);
	}
	m_basicVertexConstantBufferData.proj = m_basicVertexConstantBufferData.proj.Transpose();

	// Constant를 CPU에서 GPU로복사
	// buffer를 공유하기 때문에 하나만 복사
	if (m_meshes[ 0 ]) {
		AppBase::UpdateBuffer(m_basicVertexConstantBufferData, m_meshes[0]->vertexConstantBuffer);
	}
	
	m_basicPixelConstantBufferData.material.diffuse = Vector3(m_materialDiffuse);
	m_basicPixelConstantBufferData.material.specular = Vector3(m_materialSpecular);

	// 여러 개 조명 사용 예시
	for (int i = 0; i < MAX_LIGHTS; i++) {
		// 다른 조명 끄기
		if (i != m_lightType) {
			m_basicPixelConstantBufferData.light[ i ].strength *= 0.0f;
		}
		else {
			m_basicPixelConstantBufferData.light[ i ] = m_lightFromGUI;
		}
	}

	// buffer를 공유하기 때문에 하나만 복사
	if (m_meshes[ 0 ]) {
		AppBase::UpdateBuffer(m_basicPixelConstantBufferData, m_meshes[0]->pixelConstantBuffer);
	}

	// 노멀 벡터 그리기
	if (m_drawNormals && m_drawNormalsDirtyFlag)
	{
		AppBase::UpdateBuffer(m_normalVertexConstantBufferData, m_normalLines->vertexConstantBuffer);
		m_drawNormalsDirtyFlag = false;
	}

	// 큐브매핑을 위한 constantBuffers
	m_basicVertexConstantBufferData.model = Matrix( );
	// Transpose()도 생략 가능

	AppBase::UpdateBuffer(m_basicVertexConstantBufferData,
						  m_cubeMapping.cubeMesh->vertexConstantBuffer);
}

void KuskApp::Render() {
	
	SetViewport( );
	
	float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	m_context->ClearRenderTargetView(m_renderTargetView.Get(), clearColor);
	m_context->ClearDepthStencilView(m_depthStencilView.Get(),
									 D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	//
	m_context->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), m_depthStencilView.Get());
	m_context->OMSetDepthStencilState(m_depthStencilState.Get(), 0);

	// Vertex/Index Buffer setting
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	// 큐브 매핑
	m_context->IASetInputLayout(m_cubeMapping.inputLayout.Get( ));
	m_context->IASetVertexBuffers(0, 1, m_cubeMapping.cubeMesh->vertexBuffer.GetAddressOf( ), &stride, &offset);
	m_context->IASetIndexBuffer(m_cubeMapping.cubeMesh->indexBuffer.Get( ), DXGI_FORMAT_R32_UINT, 0);
	m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_context->VSSetShader(m_cubeMapping.vertexShader.Get( ), 0, 0);
	m_context->VSSetConstantBuffers(0, 1, m_cubeMapping.cubeMesh->vertexConstantBuffer.GetAddressOf( ));

	ID3D11ShaderResourceView* views[ 2 ] = {
		m_cubeMapping.diffuseResourceView.Get( ),
		m_cubeMapping.specularResourceView.Get( ),
	};
	m_context->PSSetShaderResources(0, 2, views);
	m_context->PSSetShader(m_cubeMapping.pixelShader.Get( ), 0, 0);
	m_context->PSSetSamplers(0, 1, m_samplerState.GetAddressOf( ));
	m_context->DrawIndexed(m_cubeMapping.cubeMesh->indexCount, 0, 0);

	// 물체들
	m_context->VSSetShader(m_basicVertexShader.Get( ), 0, 0);

	m_context->PSSetSamplers(0, 1, m_samplerState.GetAddressOf( ));
	m_context->PSSetShader(m_basicPixelShader.Get( ), 0, 0);

	if (m_drawAsWire)
		m_context->RSSetState(m_wireRasterizerState.Get( ));
	else
		m_context->RSSetState(m_solidRasterizerState.Get( ));

	// 버텍스/인덱스 버퍼 설정
	for (const auto& mesh : m_meshes) {
		m_context->VSSetConstantBuffers(0, 1, mesh->vertexConstantBuffer.GetAddressOf( ));
		ID3D11ShaderResourceView* resViews[ 3 ] = {
			mesh->textureResourceView.Get( ),
			m_cubeMapping.diffuseResourceView.Get( ),
			m_cubeMapping.specularResourceView.Get( ),
		};
		m_context->PSSetShaderResources(0, 3, resViews);
		m_context->PSSetConstantBuffers(0, 1, mesh->pixelConstantBuffer.GetAddressOf( ));
		
		m_context->IASetInputLayout(m_basicInputLayout.Get( ));
		m_context->IASetVertexBuffers(0, 1, mesh->vertexBuffer.GetAddressOf( ), &stride, &offset);
		m_context->IASetIndexBuffer(mesh->indexBuffer.Get( ), DXGI_FORMAT_R32_UINT, 0);
		m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		m_context->DrawIndexed(mesh->indexCount, 0, 0);
	}

	// 노멀 벡터 그리기
	if (m_drawNormals) {
		m_context->VSSetShader(m_normalVertexShader.Get( ), 0, 0);
		ID3D11Buffer* pptr[ 2 ] = {
			m_meshes[ 0 ]->vertexConstantBuffer.Get(),
			m_normalLines->vertexConstantBuffer.Get( ),
		};
		m_context->VSSetConstantBuffers(0, 2, pptr);
		m_context->PSSetShader(m_normalPixelShader.Get( ), 0, 0);
		m_context->IASetVertexBuffers(0, 1, m_normalLines->vertexBuffer.GetAddressOf( ), &stride, &offset);
		m_context->IASetIndexBuffer(m_normalLines->indexBuffer.Get( ), DXGI_FORMAT_R32_UINT, 0);
		m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
		m_context->DrawIndexed(m_normalLines->indexCount, 0, 0);
	}

}

void KuskApp::UpdateGUI() {
	/*ImGui::SliderFloat("Rim Strength",
					   &m_basicPixelConstantBufferData.rimStrength, 0.0f,
					   10.0f);
	ImGui::Checkbox("Use Smoothstep",
					&m_basicPixelConstantBufferData.useSmoothstep);
	ImGui::SliderFloat3("Rim Color", &m_basicPixelConstantBufferData.rimColor.x,
						0.0f, 1.0f);
	ImGui::SliderFloat("Rim Power", &m_basicPixelConstantBufferData.rimPower,
					   0.01f, 10.0f);*/

	ImGui::Checkbox("Use Texture", &m_basicPixelConstantBufferData.useTexture);
	ImGui::Checkbox("Wireframe", &m_drawAsWire);
	ImGui::Checkbox("Draw Normals", &m_drawNormals);

	if (ImGui::SliderFloat("Normal scale", &m_normalVertexConstantBufferData.scale, 0.0f, 1.0f))
	{
		m_drawNormalsDirtyFlag = true;
	}

	//ImGui::SliderFloat3("m_modelTranslation", &m_modelTranslation.x, -2.0f, 2.0f);
	ImGui::SliderFloat3("m_modelRotation", &m_modelRotation.x, -3.14f, 3.14f);
	//ImGui::SliderFloat3("m_modelScaling", &m_modelScaling.x, 0.1f, 2.0f);
	ImGui::SliderFloat3("m_viewRot", &m_viewRot.x, -3.14f, 3.14f);

	ImGui::SliderFloat("Material Diffuse", &m_materialDiffuse, 0.0f, 3.0f);
	ImGui::SliderFloat("Material Specular", &m_materialSpecular, 0.0f, 3.0f);

	ImGui::SliderFloat("Material Shininess",
					   &m_basicPixelConstantBufferData.material.shininess, 0.01f, 20.0f);

	//if (ImGui::RadioButton("Directional Light", m_lightType == 0)) {
	//	m_lightType = 0;
	//}
	//ImGui::SameLine( );
	//if (ImGui::RadioButton("Point Light", m_lightType == 1)) {
	//	m_lightType = 1;
	//}
	//ImGui::SameLine( );
	//if (ImGui::RadioButton("Spot Light", m_lightType == 2)) {
	//	m_lightType = 2;
	//}

	//ImGui::SliderFloat3("Light Position", &m_lightFromGUI.position.x, -5.0f, 5.0f);

	//ImGui::SliderFloat("Light fallOffStart", &m_lightFromGUI.fallOffStart, 0.0f, 5.0f);

	//ImGui::SliderFloat("Light fallOffEnd", &m_lightFromGUI.fallOffEnd, 0.0f, 10.0f);

	//ImGui::SliderFloat("Light spotPower", &m_lightFromGUI.spotPower, 1.0f, 512.0f);
}

} // namespace kusk