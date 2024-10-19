#include "KuskApp.h"

#include <directxtk/DDSTextureLoader.h>
#include <vector>

namespace kusk {

using namespace std;
using namespace DirectX;

KuskApp::KuskApp() : AppBase() {}

bool KuskApp::Initialize() {

	if (!AppBase::Initialize())
		return false;

	m_cubeMapping.Initialize(m_device, 
							L"./CubemapTextures/Stonewall_diffuseIBL.dds",
							L"./CubemapTextures/Stonewall_specularIBL.dds");

	MeshData sphere = GeometryGenerator::MakeSphere(0.3f, 100, 100);
	sphere.textureFilename = "ojwD8.jpg";
	m_meshGroupSphere.Initialize(m_device, { sphere });
	m_meshGroupSphere.m_diffuseResView = m_cubeMapping.m_diffuseResView;
	m_meshGroupSphere.m_specularResView = m_cubeMapping.m_specularResView;

	m_meshGroupCharacter.Initialize(
		m_device, "c:/workspaces/honglab/models/zelda/", "zeldaPosed001.fbx");
	m_meshGroupCharacter.m_diffuseResView = m_cubeMapping.m_diffuseResView;
	m_meshGroupCharacter.m_specularResView = m_cubeMapping.m_specularResView;

	BuildFilters( );

	return true;
}

void KuskApp::Update(float dt) {
	
	using namespace DirectX;

	auto& visibleMeshGroup = m_visibleMeshIndex == 0 ? m_meshGroupSphere : m_meshGroupCharacter;

	auto modelRow = Matrix::CreateScale(m_modelScaling) *
					Matrix::CreateRotationY(m_modelRotation.y) *
					Matrix::CreateRotationX(m_modelRotation.x) *
					Matrix::CreateRotationZ(m_modelRotation.z) *
					Matrix::CreateTranslation(m_modelTranslation);

	auto invTransposeRow = modelRow;
	invTransposeRow.Translation(Vector3(0.0f));
	invTransposeRow = invTransposeRow.Invert( ).Transpose( );

	auto viewRow = Matrix::CreateRotationY(m_viewRot.y) *
				   Matrix::CreateRotationX(m_viewRot.x) *
				   Matrix::CreateTranslation(0.0f, 0.0f, 2.0f);

	const float aspect = AppBase::GetAspectRatio( );
	Matrix projRow = m_usePerspectiveProjection ? XMMatrixPerspectiveFovLH(XMConvertToRadians(m_projFovAngleY), aspect, m_nearZ, m_farZ)
												: XMMatrixOrthographicOffCenterLH(-aspect, aspect, -1.0f, 1.0f, m_nearZ, m_farZ);

	auto eyeWorld = Vector3::Transform(Vector3(0.0f), viewRow.Invert( ));

	// MeshGroup의 ConstantBuffers 업데이트
	for (int i = 0; i < MAX_LIGHTS; i++) {
		// 다른 조명 끄기
		if (i != m_lightType) {
			visibleMeshGroup.m_basicPixelConstantData.lights[ i ].strength *= 0.0f;
		}
		else
		{
			visibleMeshGroup.m_basicPixelConstantData.lights[ i ] = m_lightFromGUI;
		}
	}

	visibleMeshGroup.m_basicVertexConstantData.model = modelRow.Transpose( );
	visibleMeshGroup.m_basicVertexConstantData.view = viewRow.Transpose( );
	visibleMeshGroup.m_basicVertexConstantData.proj = projRow.Transpose( );
	visibleMeshGroup.m_basicVertexConstantData.invTranspose = invTransposeRow.Transpose( );
	visibleMeshGroup.m_basicPixelConstantData.eyeWorld = eyeWorld;
	visibleMeshGroup.m_basicPixelConstantData.material.diffuse = Vector3(m_materialDiffuse);
	visibleMeshGroup.m_basicPixelConstantData.material.specular = Vector3(m_materialSpecular);
	visibleMeshGroup.UpdateConstantBuffers(m_device, m_context);

	// 큐브매핑을 위한 constantBuffers 업데이트
	m_cubeMapping.UpdateConstantBuffers(m_device, m_context, 
		 							    (Matrix::CreateRotationY(m_viewRot.y) * Matrix::CreateRotationX(m_viewRot.x)).Transpose( ),
									    projRow.Transpose( ));

	if (m_dirtyFlag) {
		m_filters[ 1 ]->m_pixelConstData.threshold = m_threshold;
		m_filters[ 1 ]->UpdateConstantBuffers(m_device, m_context);
		m_filters.back( )->m_pixelConstData.strength = m_strength;
		m_filters.back( )->UpdateConstantBuffers(m_device, m_context);

		m_dirtyFlag = 0;
	}

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

	if (m_drawAsWire)
		m_context->RSSetState(m_wireRasterizerState.Get( ));
	else
		m_context->RSSetState(m_solidRasterizerState.Get( ));

	// 큐브매핑
	m_cubeMapping.Render(m_context);
	
	// 물체들
	if (m_visibleMeshIndex == 0) {
		m_meshGroupSphere.Render(m_context);
	}
	else {
		m_meshGroupCharacter.Render(m_context);
	}

	// 후처리 필터
	for (auto& f : m_filters) {
		f->Render(m_context);
	}

}

void KuskApp::BuildFilters( ) {

	m_filters.clear( );

	auto copyFilter =
		make_shared<ImageFilter>(m_device, m_context, L"Sampling", L"Sampling", m_screenWidth, m_screenHeight);
	copyFilter->SetShaderResources({ this->m_shaderResourceView });
	m_filters.push_back(copyFilter);
	
	int resolution = 2;
	while (resolution <= m_down) {
		auto downFilter = make_shared<ImageFilter>(m_device, m_context, L"Sampling", L"Sampling", m_screenWidth / resolution, m_screenHeight / resolution);
		if (resolution == 2) {
			downFilter->SetShaderResources({ this->m_shaderResourceView });
		}
		else {
			downFilter->SetShaderResources({ m_filters.back( )->m_shaderResourceView });
		}
		downFilter->m_pixelConstData.threshold = 0.0f;
		downFilter->UpdateConstantBuffers(m_device, m_context);
		m_filters.push_back(downFilter);

		resolution *= 2;
	}

	resolution = m_down;
	while (resolution >= 1) {
		for (int i = 0; i < 5; i++) {
			auto& prevResource = m_filters.back( )->m_shaderResourceView;
			auto blurXFilter = make_shared<ImageFilter>(m_device, m_context, L"Sampling", L"BlurX", m_screenWidth / resolution, m_screenHeight / resolution);
			blurXFilter->SetShaderResources({ prevResource });
			m_filters.push_back(blurXFilter);

			auto& prevResource2 = m_filters.back( )->m_shaderResourceView;
			auto blurYFilter = make_shared<ImageFilter>(m_device, m_context, L"Sampling", L"BlurY", m_screenWidth / resolution, m_screenHeight / resolution);
			blurYFilter->SetShaderResources({ prevResource2 });
			m_filters.push_back(blurYFilter);
		}

		if (resolution > 1) {
			auto upFilter = make_shared<ImageFilter>(m_device, m_context, L"Sampling", L"Sampling", m_screenWidth / resolution * 2, m_screenHeight / resolution * 2);
			upFilter->SetShaderResources({ m_filters.back( )->m_shaderResourceView });
			upFilter->m_pixelConstData.threshold = 0.0f;
			upFilter->UpdateConstantBuffers(m_device, m_context);
			m_filters.push_back(upFilter);
		}

		resolution /= 2;
	}

	auto combineFilter = make_shared<ImageFilter>(m_device, m_context, L"Sampling", L"Combine", m_screenWidth, m_screenHeight);
	combineFilter->SetShaderResources({ m_filters.back( )->m_shaderResourceView, copyFilter->m_shaderResourceView });
	combineFilter->SetRenderTargets({ this->m_renderTargetView });
	combineFilter->m_pixelConstData.threshold = 0.0f;
	combineFilter->m_pixelConstData.strength = m_strength;
	combineFilter->UpdateConstantBuffers(m_device, m_context);
	m_filters.push_back(combineFilter);

}

void KuskApp::UpdateGUI() {
	
	auto& meshGroup = m_visibleMeshIndex == 0 ? m_meshGroupSphere : m_meshGroupCharacter;

	if (ImGui::RadioButton("Sphere", m_visibleMeshIndex == 0)) {
		m_visibleMeshIndex = 0;
	}
	ImGui::SameLine( );
	if (ImGui::RadioButton("Character", m_visibleMeshIndex == 1)) {
		m_visibleMeshIndex = 1;
	}

	/*ImGui::SliderFloat("Rim Strength",
				   &meshGroup.m_basicPixelConstantData.rimStrength, 0.0f,
				   10.0f);
	ImGui::Checkbox("Use Smoothstep",
					&meshGroup.m_basicPixelConstantData.useSmoothstep);
	ImGui::SliderFloat3("Rim Color", &meshGroup.m_basicPixelConstantData.rimColor.x,
						0.0f, 1.0f);
	ImGui::SliderFloat("Rim Power", &meshGroup.m_basicPixelConstantData.rimPower,
					   0.01f, 10.0f);*/

	m_dirtyFlag +=
		ImGui::SliderFloat("Bloom Threshold", &m_threshold, 0.0f, 1.0f);
	m_dirtyFlag +=
		ImGui::SliderFloat("Bloom Strength", &m_strength, 0.0f, 3.0f);

	ImGui::Checkbox("Use Texture",
					&meshGroup.m_basicPixelConstantData.useTexture);
	ImGui::Checkbox("Wireframe", &m_drawAsWire);
	ImGui::Checkbox("Draw Normals", &meshGroup.m_drawNormals);
	if (ImGui::SliderFloat("Normal scale",
		&meshGroup.m_normalVertexConstantData.scale, 0.0f,
		1.0f)) {
		meshGroup.m_drawNormalsDirtyFlag = true;
	}

	//ImGui::SliderFloat3("m_modelTranslation", &m_modelTranslation.x, -2.0f, 2.0f);
	ImGui::SliderFloat3("m_modelRotation", &m_modelRotation.x, -3.14f, 3.14f);
	//ImGui::SliderFloat3("m_modelScaling", &m_modelScaling.x, 0.1f, 2.0f);
	ImGui::SliderFloat3("m_viewRot", &m_viewRot.x, -3.14f, 3.14f);
	ImGui::SliderFloat3("Material FresnelR0",
						&meshGroup.m_basicPixelConstantData.material.fresnelR0.x, 0.0f, 1.0f);

	ImGui::SliderFloat("Material Diffuse", &m_materialDiffuse, 0.0f, 3.0f);
	ImGui::SliderFloat("Material Specular", &m_materialSpecular, 0.0f, 3.0f);

	ImGui::SliderFloat("Material Shininess",
					   &meshGroup.m_basicPixelConstantData.material.shininess, 0.01f, 20.0f);

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