#include "KuskApp.h"

#include <DirectXCollision.h> // 구와 광선 충돌 계산에 사용
#include <directxtk/DDSTextureLoader.h>
#include <vector>

#include "textureResources.h"

namespace kusk {

using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;

KuskApp::KuskApp() : AppBase() {}

bool KuskApp::Initialize() {

	if (!AppBase::Initialize())
		return false;

	// 포인트로 빌보드 만들기
	vector<Vector4> points;
	Vector4 p = { -4.0f, 1.0f, 2.0f, 1.0f };
	for (int i = 0; i < 5; i++) {
		points.push_back(p);
		p.x += 1.8f;
	}
	m_billboardPoints.Initialize(m_device, points);

	m_cubeMapping.Initialize(m_device, SKYBOX_ORGN_DDS, SKYBOX_DIFF_DDS, SKYBOX_SPEC_DDS);

	// $sphere
	{
		Vector3 center(0.0f, 0.3f, 4.0f);
		float radius = 0.3f;
		MeshData sphere = GeometryGenerator::MakeSphere(radius, 100, 100);
		sphere.textureFilename = EARTH_TEXTURE;
		m_mainSphere.Initialize(m_device, { sphere });
		m_mainSphere.m_diffuseResView = m_cubeMapping.m_diffuseResView;
		m_mainSphere.m_specularResView = m_cubeMapping.m_specularResView;

		// 위치 초기화
		m_mainSphere.UpdateModelWorld(Matrix::CreateTranslation(center));
		m_mainSphere.m_basicPixelConstantData.useTexture = true;
		m_mainSphere.m_basicPixelConstantData.material.diffuse = Vector3(1.0f);
		m_mainSphere.m_basicPixelConstantData.material.specular = Vector3(0.3f);
		m_mainSphere.UpdateConstantBuffers(m_device, m_context);

		// 동일한 크기와 위치에 BoundingSphere 만들기
		m_mainBoundingSphere = BoundingSphere(center, radius); 
	}

	// $cursor sphere 
	// - main sphere와의 충돌이 감지되면 월드 공간에 작게 그려지는 구
	{
		MeshData sphere = GeometryGenerator::MakeSphere(0.05f, 10, 10);
		m_cursorSphere.Initialize(m_device, { sphere });
		m_cursorSphere.m_diffuseResView = m_cubeMapping.m_diffuseResView;
		m_cursorSphere.m_specularResView = m_cubeMapping.m_specularResView;

		// 위치 초기화
		m_cursorSphere.UpdateModelWorld(Matrix::CreateTranslation({ 0.0f, 0.0f, 0.0f }));
		m_cursorSphere.m_basicPixelConstantData.useTexture = false;
		m_cursorSphere.m_basicPixelConstantData.material.diffuse = Vector3(0.0f, 1.0f, 1.0f);
		m_cursorSphere.m_basicPixelConstantData.material.specular = Vector3(0.0f);
		m_cursorSphere.UpdateConstantBuffers(m_device, m_context);
	}

	// $character
	{
		m_meshGroupCharacter.Initialize(
		m_device, "c:/workspaces/honglab/models/zelda/", "zeldaPosed001.fbx");
		m_meshGroupCharacter.m_diffuseResView = m_cubeMapping.m_diffuseResView;
		m_meshGroupCharacter.m_specularResView = m_cubeMapping.m_specularResView;
		Matrix modelMat = Matrix::CreateTranslation({ 0.0f, 0.2f, 3.0f });
		Matrix invTransposeRow = modelMat;
		invTransposeRow.Translation(Vector3(0.0f));
		invTransposeRow = invTransposeRow.Invert( ).Transpose( );
		m_meshGroupCharacter.m_basicVertexConstantData.modelWorld = modelMat.Transpose( );
		m_meshGroupCharacter.m_basicVertexConstantData.invTranspose = invTransposeRow.Transpose( );
		m_meshGroupCharacter.m_basicPixelConstantData.useTexture = true;
		m_meshGroupCharacter.m_basicPixelConstantData.material.diffuse = Vector3(1.0f);
		m_meshGroupCharacter.m_basicPixelConstantData.material.specular = Vector3(1.0f);
		m_meshGroupCharacter.UpdateConstantBuffers(m_device, m_context);
	}
	
	// $ground
	{
		MeshData ground = GeometryGenerator::MakeSquare(4.0f);
		ground.textureFilename = BLENDER_UV_GRID_2K_TEXTURE;
		m_meshGroupGround.Initialize(m_device, { ground });
		m_meshGroupGround.m_diffuseResView = m_cubeMapping.m_diffuseResView;
		m_meshGroupGround.m_specularResView = m_cubeMapping.m_specularResView;
		m_meshGroupGround.UpdateModelWorld(Matrix::CreateRotationX(DirectX::XM_PIDIV2));
		m_meshGroupGround.m_basicPixelConstantData.useTexture = true;
		m_meshGroupGround.m_basicPixelConstantData.material.diffuse = Vector3(1.0f);
		m_meshGroupGround.UpdateConstantBuffers(m_device, m_context);
	}

	BuildFilters( );

	return true;
}

void KuskApp::Update(float dt) {
	
	// 카메라의 이동
	if (m_useFirstPersonView) {
		if (m_keyPressed[ 87 ]) // w
			m_camera.MoveForward(dt);
		if (m_keyPressed[ 83 ]) // s
			m_camera.MoveForward(-dt);
		if (m_keyPressed[ 68 ]) // d
			m_camera.MoveRight(dt);
		if (m_keyPressed[ 65 ]) // a
			m_camera.MoveRight(-dt);
	}

	Matrix viewRow = m_camera.GetViewRow( );
	Matrix projRow = m_camera.GetProjRow( );
	Vector3 eyeWorld = m_camera.GetEyePos( );

#pragma region visibleMeshGroup_Transform_COMMENTED_OUT
	//auto& visibleMeshGroup = m_visibleMeshIndex == 0 ? m_mainSphere : m_meshGroupCharacter;

	//auto modelRow = Matrix::CreateScale(m_modelScaling) *
	//				Matrix::CreateRotationY(m_modelRotation.y) *
	//				Matrix::CreateRotationX(m_modelRotation.x) *
	//				Matrix::CreateRotationZ(m_modelRotation.z) *
	//				Matrix::CreateTranslation(m_modelTranslation);

	//auto invTransposeRow = modelRow;
	//invTransposeRow.Translation(Vector3(0.0f));
	//invTransposeRow = invTransposeRow.Invert( ).Transpose( );

	//const float aspect = AppBase::GetAspectRatio( );

	//// MeshGroup의 ConstantBuffers 업데이트
	//for (int i = 0; i < MAX_LIGHTS; i++) {
	//	// 다른 조명 끄기
	//	if (i != m_lightType) {
	//		visibleMeshGroup.m_basicPixelConstantData.lights[ i ].strength *= 0.0f;
	//	}
	//	else
	//	{
	//		visibleMeshGroup.m_basicPixelConstantData.lights[ i ] = m_lightFromGUI;
	//	}
	//}

	//visibleMeshGroup.m_basicVertexConstantData.model = modelRow.Transpose( );
	//visibleMeshGroup.m_basicVertexConstantData.view = viewRow.Transpose( );
	//visibleMeshGroup.m_basicVertexConstantData.proj = projRow.Transpose( );
	//visibleMeshGroup.m_basicVertexConstantData.invTranspose = invTransposeRow.Transpose( );
	//visibleMeshGroup.m_basicPixelConstantData.eyeWorld = eyeWorld;
	//visibleMeshGroup.m_basicPixelConstantData.material.diffuse = Vector3(m_materialDiffuse);
	//visibleMeshGroup.m_basicPixelConstantData.material.specular = Vector3(m_materialSpecular);
	//visibleMeshGroup.UpdateConstantBuffers(m_device, m_context);
#pragma endregion

	// 큐브매핑을 위한 constantBuffers 업데이트
	m_cubeMapping.UpdateConstantBuffers(m_device, m_context, viewRow.Transpose( ), projRow.Transpose( ));

	// 다른 물체들 Constant Buffer 업데이트
	m_meshGroupGround.m_basicVertexConstantData.view = viewRow.Transpose( );
	m_meshGroupGround.m_basicVertexConstantData.proj = projRow.Transpose( );
	m_meshGroupGround.m_basicPixelConstantData.eyeWorld = eyeWorld;
	m_meshGroupGround.UpdateConstantBuffers(m_device, m_context);

	m_meshGroupCharacter.m_basicVertexConstantData.view = viewRow.Transpose( );
	m_meshGroupCharacter.m_basicVertexConstantData.proj = projRow.Transpose( );
	m_meshGroupCharacter.m_basicPixelConstantData.eyeWorld = eyeWorld;
	m_meshGroupCharacter.UpdateConstantBuffers(m_device, m_context);

	m_billboardPoints.m_constantData.eyeWorld = eyeWorld;
	m_billboardPoints.m_constantData.view = viewRow.Transpose( );
	m_billboardPoints.m_constantData.proj = projRow.Transpose( );
	D3D11Utils::UpdateBuffer(m_device, m_context, m_billboardPoints.m_constantData, m_billboardPoints.m_constantBuffer);

	// mainSphere의 회전 및 이동 계산용
	static float prevRatio = 0.0f;
	Quaternion q = Quaternion::CreateFromAxisAngle(Vector3(1.0f, 0.0f, 0.0f), 0.0f);
	static Vector3 prevPos(0.0f);
	Vector3 dragTranslation(0.0f);

	// 마우스 선택했을 때만 업데이트
	if (m_leftButton || m_rightButton)
	{
		// OnMouseMove에서 m_mouseNdcX, m_mouseNdxY 저장

		// ViewFrustum에서 가까운 면 위의 커서 위치 (z값 유의)
		Vector3 cursorNdcNear = Vector3(m_cursorNdcX, m_cursorNdcY, 0.0f);
		
		// ViewFrustum에서 먼 면 위의 커서 위치 (z값 유의)
		Vector3 cursorNdcFar = Vector3(m_cursorNdcX, m_cursorNdcY, 1.0f);

		// NDC 커서 위치를 월드 좌표계로 역변환 해주는 행렬
		Matrix inverseProjView = (viewRow * projRow).Invert( );

		// ViewFrustum 안에서 PickingRay의 방향 구하기
		Vector3 cursorWorldNear = Vector3::Transform(cursorNdcNear, inverseProjView);
		Vector3 cursorWorldFar = Vector3::Transform(cursorNdcFar, inverseProjView);
		Vector3 dir = cursorWorldFar - cursorWorldNear;
		dir.Normalize( );

		// 광선을 만들고 충돌 감지
		SimpleMath::Ray curRay = SimpleMath::Ray(cursorWorldNear, dir);
		float dist = 0.0f;
		m_selected = curRay.Intersects(m_mainBoundingSphere, dist);

		if (m_selected) {

			Vector3 pickPoint = cursorWorldNear + dist * dir;

			// 충돌 지점에 작은 구 그리기
			m_cursorSphere.UpdateModelWorld(Matrix::CreateTranslation(pickPoint));
			m_cursorSphere.m_basicVertexConstantData.view = viewRow.Transpose( );
			m_cursorSphere.m_basicVertexConstantData.proj = projRow.Transpose( );
			m_cursorSphere.m_basicPixelConstantData.eyeWorld = eyeWorld;
			m_cursorSphere.UpdateConstantBuffers(m_device, m_context);

			// mainSphere를 어떻게 회전시킬지 결정
			if (m_leftButton)
			{
				if (m_dragStartFlag) { // 드래그를 시작하는 경우
					m_dragStartFlag = false;

					prevPos = pickPoint - Vector3(m_mainBoundingSphere.Center);
				}
				else {
					Vector3 currentPos = pickPoint - Vector3(m_mainBoundingSphere.Center);

					// 마우스가 조금이라도 움직였을 경우에만 회전시키기
					if ((currentPos - prevPos).Length( ) > 1e-3) {
						q = Quaternion::FromToRotation(prevPos, currentPos);
						prevPos = currentPos;
					}
				}
			}
			// mainSphere를 어떻게 이동시킬지 결정
			else if (m_rightButton)
			{
				if (m_dragStartFlag) { // 드래그를 시작하는 경우
					m_dragStartFlag = false;

					prevRatio = dist / (cursorWorldFar - cursorWorldNear).Length( );
					prevPos = pickPoint;
				}
				else {
					Vector3 newPos = cursorWorldNear + prevRatio * (cursorWorldFar - cursorWorldNear);
					dragTranslation = newPos - prevPos;
					prevPos = newPos;
				}
			}
		}
	}

	// 원점의 위치를 옮기지 않기 위해 Translation 추출
	Vector3 translation = m_mainSphere.m_modelWorldRow.Translation( );
	m_mainSphere.m_modelWorldRow.Translation(Vector3(0.0f));
	m_mainSphere.UpdateModelWorld(m_mainSphere.m_modelWorldRow *
							  Matrix::CreateFromQuaternion(q) *
							  Matrix::CreateTranslation(dragTranslation) *
							  Matrix::CreateTranslation(translation));
	//Bounding Sphere도 같이 이동
	m_mainBoundingSphere.Center = m_mainSphere.m_modelWorldRow.Translation( );
	m_mainSphere.m_basicVertexConstantData.view = viewRow.Transpose( );
	m_mainSphere.m_basicVertexConstantData.proj = projRow.Transpose( );
	m_mainSphere.m_basicPixelConstantData.eyeWorld = eyeWorld;
	m_mainSphere.UpdateConstantBuffers(m_device, m_context);

	if (m_dirtyFlag) {
		assert(m_filters.size( ) > 1);
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

	m_billboardPoints.Render(m_context);

	// 물체들
	if (m_visibleMeshIndex == 0) {
		m_mainSphere.Render(m_context);
	}
	else {
		m_meshGroupCharacter.Render(m_context);
	}

	if ((m_leftButton || m_rightButton) && m_selected)
		m_cursorSphere.Render(m_context);

	m_meshGroupGround.Render(m_context);

	// 물체 렌더링 후 큐브매핑
	m_cubeMapping.Render(m_context);

	// 후처리 필터
	if (m_usePostProcessing) {
		for (auto& f : m_filters) {
			f->Render(m_context);
		}
	}
	
}

void KuskApp::BuildFilters( ) {

	m_filters.clear( );

	auto copyFilter =
		make_shared<ImageFilter>(m_device, m_context, L"Sampling", L"Sampling", m_screenWidth, m_screenHeight);
	copyFilter->SetShaderResources({ this->m_shaderResourceView });
	m_filters.push_back(copyFilter);
	
	// 해상도를 낮춰서 다운 샘플링
	int resolution = 2;
	while (resolution <= m_down) {
		auto downFilter = make_shared<ImageFilter>(m_device, m_context, L"Sampling", L"Sampling", m_screenWidth / resolution, m_screenHeight / resolution);
		if (resolution == 2) {
			downFilter->SetShaderResources({ this->m_shaderResourceView });
			downFilter->m_pixelConstData.threshold = m_threshold;
		}
		else {
			downFilter->SetShaderResources({ m_filters.back( )->m_shaderResourceView });
			downFilter->m_pixelConstData.threshold = 0.0f;
		}
		downFilter->UpdateConstantBuffers(m_device, m_context);
		m_filters.push_back(downFilter);

		resolution *= 2;
	}

	resolution = m_down;
	while (resolution >= 1) {
		for (int i = 0; i < m_repeat; i++) {
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

	ImGui::Checkbox("Use FPV", &m_useFirstPersonView);
	ImGui::Checkbox("Use PostProc", &m_usePostProcessing);
	
	auto& meshGroup = m_visibleMeshIndex == 0 ? m_mainSphere : m_meshGroupCharacter;

	if (ImGui::RadioButton("Sphere", m_visibleMeshIndex == 0)) {
		m_visibleMeshIndex = 0;
	}
	ImGui::SameLine( );
	if (ImGui::RadioButton("Character", m_visibleMeshIndex == 1)) {
		m_visibleMeshIndex = 1;
	}

	ImGui::SliderFloat("Rim Strength",
				   &meshGroup.m_basicPixelConstantData.rimStrength, 0.0f,
				   10.0f);
	ImGui::Checkbox("Use Smoothstep",
					&meshGroup.m_basicPixelConstantData.useSmoothstep);
	ImGui::SliderFloat3("Rim Color", &meshGroup.m_basicPixelConstantData.rimColor.x,
						0.0f, 1.0f);
	ImGui::SliderFloat("Rim Power", &meshGroup.m_basicPixelConstantData.rimPower,
					   0.01f, 10.0f);

	// 후 처리 필터 사용시에만 옵션 보여주기
	if (m_usePostProcessing)
	{
		m_dirtyFlag +=
			ImGui::SliderFloat("Bloom Threshold", &m_threshold, 0.0f, 1.0f);
		m_dirtyFlag +=
			ImGui::SliderFloat("Bloom Strength", &m_strength, 0.0f, 3.0f);
	}
	
	ImGui::Checkbox("Use Texture",
					&meshGroup.m_basicPixelConstantData.useTexture);
	ImGui::Checkbox("Wireframe", &m_drawAsWire);
	ImGui::Checkbox("Draw Normals", &meshGroup.m_drawNormals);
	if (ImGui::Checkbox("Draw Normals", &meshGroup.m_drawNormals) 
		|| ImGui::SliderFloat("Normal scale",	&meshGroup.m_normalVertexConstantData.scale, 0.0f, 1.0f)) {
		meshGroup.m_drawNormalsDirtyFlag = true;
	}

	//
	/*ImGui::SliderFloat3("m_modelTranslation", &m_modelTranslation.x, -2.0f, 2.0f);
	ImGui::SliderFloat3("m_modelRotation", &m_modelRotation.x, -3.14f, 3.14f);
	ImGui::SliderFloat3("m_modelScaling", &m_modelScaling.x, 0.1f, 2.0f);*/
	
	ImGui::SliderFloat3("Material FresnelR0",
						&meshGroup.m_basicPixelConstantData.material.fresnelR0.x, 0.0f, 1.0f);

	ImGui::SliderFloat("Material Diffuse", &m_materialDiffuse, 0.0f, 3.0f);
	ImGui::SliderFloat("Material Specular", &m_materialSpecular, 0.0f, 3.0f);

	ImGui::SliderFloat("Material Shininess",
					   &meshGroup.m_basicPixelConstantData.material.shininess, 0.01f, 20.0f);

	if (ImGui::RadioButton("Directional Light", m_lightType == 0)) {
		m_lightType = 0;
	}
	ImGui::SameLine( );
	if (ImGui::RadioButton("Point Light", m_lightType == 1)) {
		m_lightType = 1;
	}
	ImGui::SameLine( );
	if (ImGui::RadioButton("Spot Light", m_lightType == 2)) {
		m_lightType = 2;
	}

	ImGui::SliderFloat3("Light Strength", &m_lightFromGUI.strength.x, 0.0f, 0.1f);

	ImGui::SliderFloat3("Light Position", &m_lightFromGUI.position.x, -5.0f, 5.0f);

	ImGui::SliderFloat("Light fallOffStart", &m_lightFromGUI.fallOffStart, 0.0f, 5.0f);

	ImGui::SliderFloat("Light fallOffEnd", &m_lightFromGUI.fallOffEnd, 0.0f, 10.0f);

	ImGui::SliderFloat("Light spotPower", &m_lightFromGUI.spotPower, 1.0f, 512.0f);
	
}

} // namespace kusk