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

	// $tessellatedQuad
	/*m_tessellatedQuad.Initialize(m_device);
	m_tessellatedQuad.m_irradianceSRV = m_cubeMapping.m_irradianceSRV;
	m_tessellatedQuad.m_specularResView = m_cubeMapping.m_specularResView;

	// 배경 나무 텍스쳐
	vector<Vector4> points;
	Vector4 p = { -40.0f, 1.0f, 20.0f, 1.0f };
	for (int i = 0; i < 100; i++) {
		points.push_back(p);
		p.x += 2.0f;
	}
	std::vector<std::string> treeTextureFilenames = {
	"./Assets/Textures/TreeBillboards/1.png",
	"./Assets/Textures/TreeBillboards/2.png",
	"./Assets/Textures/TreeBillboards/3.png",
	"./Assets/Textures/TreeBillboards/4.png",
	"./Assets/Textures/TreeBillboards/5.png" };

	m_billboardPoints.Initialize(m_device, m_context, points, 2.4f, L"BillboardPointsPS.hlsl", treeTextureFilenames);*/

	// Shadertoy Media Files
	// https://shadertoyunofficial.wordpress.com/2019/07/23/shadertoy-media-files/

	m_cubeMapping.Initialize(m_device, OUTDOOR_ORGN_DDS, OUTDOOR_SPEC_DDS, OUTDOOR_DIFF_DDS, OUTDOOR_BRDF_DDS);

	// $sphere
	{
		Vector3 center(0.0f, 0.0f, 1.0f);
		float radius = 0.4f;
		MeshData sphere = GeometryGenerator::MakeSphere(radius, 100, 100, {2.0f, 2.0f});
		//
		/*sphere.albedoTextureFilename =
			GREY_POROUS_ROCK_TEXTURE_DIR
			GERY_POROUS_ROCK_TEXTURE_DIFFUSE;
		sphere.normalTextureFilename = 
			GREY_POROUS_ROCK_TEXTURE_DIR
			GERY_POROUS_ROCK_TEXTURE_NORMAL;
		sphere.heightTextureFilename = 
			GREY_POROUS_ROCK_TEXTURE_DIR
			GERY_POROUS_ROCK_TEXTURE_HEIGHT;
		sphere.aoTextureFilename = 
			GREY_POROUS_ROCK_TEXTURE_DIR
			GERY_POROUS_ROCK_TEXTURE_AO;*/

		sphere.albedoTextureFilename =
			WORN_PAINTED_METAL_TEXTURE_DIR
			WORN_PAINTED_METAL_TEXTURE_ALBEDO;
		sphere.normalTextureFilename = 
			WORN_PAINTED_METAL_TEXTURE_DIR
			WORN_PAINTED_METAL_TEXTURE_NORMAL;
		sphere.heightTextureFilename = 
			WORN_PAINTED_METAL_TEXTURE_DIR
			WORN_PAINTED_METAL_TEXTURE_HEIGHT;
		sphere.aoTextureFilename = 
			WORN_PAINTED_METAL_TEXTURE_DIR
			WORN_PAINTED_METAL_TEXTURE_AO;
		sphere.metallicTextureFilename =
			WORN_PAINTED_METAL_TEXTURE_DIR
			WORN_PAINTED_METAL_TEXTURE_METALLIC;
		sphere.roughnessTextureFilename =
			WORN_PAINTED_METAL_TEXTURE_DIR
			WORN_PAINTED_METAL_TEXTURE_ROUGHNESS;

		m_mainSphere.Initialize(m_device, m_context, { sphere });
		m_mainSphere.m_irradianceSRV = m_cubeMapping.m_irradianceSRV;
		m_mainSphere.m_specularSRV = m_cubeMapping.m_specularSRV;
		m_mainSphere.m_brdfSRV = m_cubeMapping.m_brdfSRV;
		m_mainSphere.UpdateModelWorld(Matrix::CreateTranslation(center));
		m_mainSphere.UpdateConstantBuffers(m_device, m_context);

		// 동일한 크기와 위치에 BoundingSphere 만들기
		m_mainBoundingSphere = BoundingSphere(center, radius); 
	}

	// $cursor sphere 
	// - main sphere와의 충돌이 감지되면 월드 공간에 작게 그려지는 구
	{
		MeshData sphere = GeometryGenerator::MakeSphere(0.02f, 10, 10);
		m_cursorSphere.Initialize(m_device, m_context, { sphere });
		m_cursorSphere.m_irradianceSRV = m_cubeMapping.m_irradianceSRV;
		m_cursorSphere.m_specularSRV = m_cubeMapping.m_specularSRV;
		Matrix modelMat = Matrix::CreateTranslation({ 0.0f, 0.0f, 0.0f });
		Matrix invTransposeRow = modelMat;
		invTransposeRow.Translation(Vector3(0.0f));
		invTransposeRow = invTransposeRow.Invert( ).Transpose( );
		m_cursorSphere.m_basicVertexConstData.modelWorld = modelMat.Transpose( );
		m_cursorSphere.m_basicVertexConstData.invTranspose = invTransposeRow.Transpose( );
		m_cursorSphere.m_basicPixelConstData.material.albedo = Vector3(0.0f, 1.0f, 1.0f);
		m_cursorSphere.UpdateConstantBuffers(m_device, m_context);
	}

	// $character
	/* {
		m_meshGroupCharacter.Initialize(
		m_device, m_context, "c:/workspaces/honglab/models/zelda/", "zeldaPosed001.fbx");
		m_meshGroupCharacter.m_irradianceSRV = m_cubeMapping.m_irradianceSRV;
		m_meshGroupCharacter.m_specularSRV = m_cubeMapping.m_specularSRV;
		Matrix modelMat = Matrix::CreateTranslation({ 0.0f, 0.2f, 3.0f });
		Matrix invTransposeRow = modelMat;
		invTransposeRow.Translation(Vector3(0.0f));
		invTransposeRow = invTransposeRow.Invert( ).Transpose( );
		m_meshGroupCharacter.m_basicVertexConstData.modelWorld = modelMat.Transpose( );
		m_meshGroupCharacter.m_basicVertexConstData.invTranspose = invTransposeRow.Transpose( );
		m_meshGroupCharacter.m_basicVertexConstData.useHeightMap = false;
		m_meshGroupCharacter.m_basicPixelConstData.useAlbedoMap = true;
		m_meshGroupCharacter.m_basicPixelConstData.useNormalMap = false;
		m_meshGroupCharacter.m_basicPixelConstData.useAOMap = false;
		m_meshGroupCharacter.UpdateConstantBuffers(m_device, m_context);
	}*/
	
	// $ground
	/*{
		//MeshData ground = GeometryGenerator::MakeSquare(20.0f, {40.0f, 40.0f});
		//MeshData ground = GeometryGenerator::MakeSquareGrid(2048, 2048, 20.0f, { 40.0f, 40.0f });
		MeshData ground = GeometryGenerator::MakeSquare(0.4);
		// HDRI 테스트
		ground.albedoTextureFilename = BRICK_TEXTURE_4K_HDRI;
		ground.albedoTextureFilename =
			BRICK_TEXTURE_DIR
			BRICK_TEXTURE_DIFFUSE;
		ground.normalTextureFilename =
			BRICK_TEXTURE_DIR
			BRICK_TEXTURE_NORMAL;
		ground.heightTextureFilename =
			BRICK_TEXTURE_DIR
			BRICK_TEXTURE_HEIGHT;
		ground.aoTextureFilename =
			BRICK_TEXTURE_DIR
			BRICK_TEXTURE_AO;
		m_meshGroupGround.Initialize(m_device, m_context, { ground });
		m_meshGroupGround.m_irradianceSRV = m_cubeMapping.m_irradianceSRV;
		m_meshGroupGround.m_specularSRV = m_cubeMapping.m_specularSRV;
		m_meshGroupGround.UpdateModelWorld(
			Matrix::CreateScale(2.0f, 1.0f, 1.0f) * 
			Matrix::CreateRotationX(DirectX::XM_PIDIV2 * 0.0f) *
			Matrix::CreateTranslation(0.2f, 0.4f, 1.0f));
		//m_meshGroupGround.UpdateModelWorld(Matrix::CreateRotationX(DirectX::XM_PIDIV2));
		m_meshGroupGround.m_basicPixelConstData.useAlbedoMap = true;
		//m_meshGroupGround.m_basicPixelConstData.useNormalMap = true;
		m_meshGroupGround.m_basicPixelConstData.useAOMap = false;
		m_meshGroupGround.m_basicVertexConstData.useHeightMap= false;
		m_meshGroupGround.m_basicVertexConstData.heightScale= 0.02;
		m_meshGroupGround.m_basicPixelConstData.material.diffuse = Vector3(0.8f);
		m_meshGroupGround.UpdateConstantBuffers(m_device, m_context);
	}*/

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
		if (m_keyPressed[ 81 ]) // q
			m_camera.MoveUp(dt);
		if (m_keyPressed[ 69 ]) // e
			m_camera.MoveUp(-dt);
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
	//		visibleMeshGroup.m_basicPixelConstData.lights[ i ].radiance *= 0.0f;
	//	}
	//	else
	//	{
	//		visibleMeshGroup.m_basicPixelConstData.lights[ i ] = m_lightFromGUI;
	//	}
	//}

	//visibleMeshGroup.m_basicVertexConstData.model = modelRow.Transpose( );
	//visibleMeshGroup.m_basicVertexConstData.view = viewRow.Transpose( );
	//visibleMeshGroup.m_basicVertexConstData.proj = projRow.Transpose( );
	//visibleMeshGroup.m_basicVertexConstData.invTranspose = invTransposeRow.Transpose( );
	//visibleMeshGroup.m_basicPixelConstData.eyeWorld = eyeWorld;
	//visibleMeshGroup.m_basicPixelConstData.material.diffuse = Vector3(m_materialDiffuse);
	//visibleMeshGroup.m_basicPixelConstData.material.specular = Vector3(m_materialSpecular);
	//visibleMeshGroup.UpdateConstantBuffers(m_device, m_context);
#pragma endregion

	// 큐브매핑을 위한 constantBuffers 업데이트
	Matrix viewEnvRow = viewRow;
	viewEnvRow.Translation(Vector3(0.0f)); // 이동 취소
	m_cubeMapping.UpdateVertexConstBuffer(m_device, m_context, viewEnvRow.Transpose( ), projRow.Transpose( ));

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
			m_cursorSphere.m_basicVertexConstData.view = viewRow.Transpose( );
			m_cursorSphere.m_basicVertexConstData.proj = projRow.Transpose( );
			m_cursorSphere.m_basicPixelConstData.eyeWorld = eyeWorld;
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
							  Matrix::CreateTranslation(dragTranslation + translation));
	//Bounding Sphere도 같이 이동
	m_mainBoundingSphere.Center = m_mainSphere.m_modelWorldRow.Translation( );
	m_mainSphere.m_basicVertexConstData.view = viewRow.Transpose( );
	m_mainSphere.m_basicVertexConstData.proj = projRow.Transpose( );
	m_mainSphere.m_basicPixelConstData.eyeWorld = eyeWorld;

	// 포인트 라이트 효과
	Light pointLight;
	pointLight.position = m_lightFromGUI.position;
	pointLight.radiance = Vector3(1.0f); // Strength
	pointLight.fallOffEnd = 20.0f;
	m_mainSphere.m_basicPixelConstData.lights[ 1 ] = pointLight;

	m_mainSphere.UpdateConstantBuffers(m_device, m_context);

	// $tessellatedQuad
	/*m_tessellatedQuad.m_constantData.eyeWorld = eyeWorld;
	m_tessellatedQuad.m_constantData.model = Matrix( );
	m_tessellatedQuad.m_constantData.view = viewRow.Transpose( );
	m_tessellatedQuad.m_constantData.proj = projRow.Transpose( );
	D3D11Utils::UpdateBuffer(m_device, m_context, m_tessellatedQuad.m_constantData, m_tessellatedQuad.m_constantBuffer);*/

	
	/*m_meshGroupGround.m_basicPixelConstData.lights[ 1 ] = pointLight;
	m_mainSphere.m_basicPixelConstData.lights[ 1 ] = pointLight;
	m_cursorSphere.m_basicPixelConstData.lights[ 1 ] = pointLight;

	m_meshGroupGround.m_basicVertexConstData.view = viewRow.Transpose( );
	m_meshGroupGround.m_basicVertexConstData.proj = projRow.Transpose( );
	m_meshGroupGround.m_basicPixelConstData.eyeWorld = eyeWorld;
	m_meshGroupGround.UpdateConstantBuffers(m_device, m_context);

	m_meshGroupCharacter.m_basicVertexConstData.view = viewRow.Transpose( );
	m_meshGroupCharacter.m_basicVertexConstData.proj = projRow.Transpose( );
	m_meshGroupCharacter.m_basicPixelConstData.eyeWorld = eyeWorld;
	m_meshGroupCharacter.UpdateConstantBuffers(m_device, m_context);

	m_billboardPoints.m_constantData.eyeWorld = eyeWorld;
	m_billboardPoints.m_constantData.view = viewRow.Transpose( );
	m_billboardPoints.m_constantData.proj = projRow.Transpose( );
	D3D11Utils::UpdateBuffer(m_device, m_context, m_billboardPoints.m_constantData, m_billboardPoints.m_constantBuffer);*/
}

void KuskApp::Render() {
	
	SetViewport( );
	
	float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	vector<ID3D11RenderTargetView*> renderTargetViews = { m_floatRTV.Get( ) };
	for (size_t i = 0; i < renderTargetViews.size( ); i++) {
		m_context->ClearRenderTargetView(renderTargetViews[ i ], clearColor);
	}
	m_context->OMSetRenderTargets(UINT(renderTargetViews.size( )), renderTargetViews.data( ), m_depthStencilView.Get( ));
	m_context->ClearDepthStencilView(m_depthStencilView.Get(),
									 D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	
	m_context->OMSetDepthStencilState(m_depthStencilState.Get(), 0);

	if (m_drawAsWire)
		m_context->RSSetState(m_wireRasterizerState.Get( ));
	else
		m_context->RSSetState(m_solidRasterizerState.Get( ));

	//m_billboardPoints.Render(m_context);
	//m_tessellatedQuad.Render(m_context);

	m_mainSphere.Render(m_context);

	// 물체들
	/*if (m_visibleMeshIndex == 0) {
		m_mainSphere.Render(m_context);
	}
	else {
		m_meshGroupCharacter.Render(m_context);
	}*/

	if ((m_leftButton || m_rightButton) && m_selected)
		m_cursorSphere.Render(m_context);

	//m_meshGroupGround.Render(m_context);

	// 물체 렌더링 후 큐브매핑
	m_cubeMapping.Render(m_context);

	// MSAA로 Texture2DMS에 렌더링된 결과를 Texture2D로 변환(Resolve)
	m_context->ResolveSubresource(m_resolvedBuffer.Get( ), 0, m_floatBuffer.Get( ), 0, DXGI_FORMAT_R16G16B16A16_FLOAT);

	m_postProcess.Render(m_context);
}

void KuskApp::UpdateGUI() {

	ImGui::SetNextItemOpen(false, ImGuiCond_Once);
	if (ImGui::TreeNode("General")) {
		ImGui::Checkbox("Use FPV", &m_useFirstPersonView);
		ImGui::Checkbox("Wireframe", &m_drawAsWire);
		if (ImGui::Checkbox("MSAA ON", &m_useMSAA)) {
			CreateBuffers( );
		}

		ImGui::TreePop( );
	}

	ImGui::SetNextItemOpen(true, ImGuiCond_Once);
	if (ImGui::TreeNode("Env Map")) {
		int flag = 0;
		flag += ImGui::RadioButton("Env", &m_cubeMapping.m_pixelConstData.textureToDraw, 0);
		ImGui::SameLine( );
		flag += ImGui::RadioButton("Specular", &m_cubeMapping.m_pixelConstData.textureToDraw, 1);
		ImGui::SameLine( );
		flag += ImGui::RadioButton("Irradiance", &m_cubeMapping.m_pixelConstData.textureToDraw, 2);
		flag += ImGui::SliderFloat("Mip Level", &m_cubeMapping.m_pixelConstData.mipLevel, 0.0f, 10.0f);

		if (flag) {
			m_cubeMapping.UpdatePixelConstBuffers(m_device, m_context);
		}

		ImGui::TreePop( );
	}

	ImGui::SetNextItemOpen(false, ImGuiCond_Once);
	if (ImGui::TreeNode("Post Processing")) {
		int flag = 0;

		flag += ImGui::SliderFloat("Bloom Strength", &m_postProcess.m_combineFilter.m_constData.strength, 0.0f, 1.0f);
		flag += ImGui::SliderFloat("Exposure", &m_postProcess.m_combineFilter.m_constData.exposure, 0.0f, 10.0f);
		flag += ImGui::SliderFloat("Gamma", &m_postProcess.m_combineFilter.m_constData.gamma, 0.1f, 5.0f);

		// 편의상 사용자 입력이 인식되면 바로 GPU 버퍼를 업데이트
		if (flag) {
			m_postProcess.m_combineFilter.UpdateConstantBuffers(m_device, m_context);
		}

		ImGui::TreePop( );
	}

	ImGui::SetNextItemOpen(true, ImGuiCond_Once);
	if (ImGui::TreeNode("Point Light")) {
		ImGui::SliderFloat3("Position", &m_lightFromGUI.position.x, -5.0f, 5.0f);
		ImGui::TreePop( );
	}

	ImGui::SetNextItemOpen(true, ImGuiCond_Once);
	if (ImGui::TreeNode("Material")) {
		int flag = 0;
		flag += ImGui::SliderFloat("Metallic", &m_mainSphere.m_basicPixelConstData.material.metallic, 0.0f, 1.0f);
		flag += ImGui::SliderFloat("Roughness", &m_mainSphere.m_basicPixelConstData.material.roughness, 0.0f, 1.0f);
		flag += ImGui::CheckboxFlags("Use AlbedoTexture", &m_mainSphere.m_basicPixelConstData.useAlbedoMap, 1);
		flag += ImGui::CheckboxFlags("Use NormalMapping", &m_mainSphere.m_basicPixelConstData.useNormalMap, 1);
		flag += ImGui::CheckboxFlags("Use AO", &m_mainSphere.m_basicPixelConstData.useAOMap, 1);
		flag += ImGui::CheckboxFlags("Use HeightMapping", &m_mainSphere.m_basicVertexConstData.useHeightMap, 1);
		flag += ImGui::SliderFloat("HeightScale", &m_mainSphere.m_basicVertexConstData.heightScale, 0.0f, 0.1f);
		flag += ImGui::CheckboxFlags("Use MetallicMap", &m_mainSphere.m_basicPixelConstData.useMetallicMap, 1);
		flag += ImGui::CheckboxFlags("Use RoughnessMap", &m_mainSphere.m_basicPixelConstData.useRoughnessMap, 1);
		flag += ImGui::Checkbox("Draw Normals", &m_mainSphere.m_drawNormals);
		
		if (flag) {
			// GUI 입력이 있을 때만 할 일들 추가.. (근데 어처피 Update에서 계속 업데이트 해주기 때문에 패스)
		}

		ImGui::TreePop( );
	}

	/*
	ImGui::SetNextItemOpen(true, ImGuiCond_Once);
	if (ImGui::TreeNode("Sphere")) {
		bool temp = bool(m_mainSphere.m_basicPixelConstData.useAlbedoMap);
		if (ImGui::Checkbox("Use AlbedoTexture", &temp)) {
			m_mainSphere.m_basicPixelConstData.useAlbedoMap = int(temp);
		}

		temp = bool(m_mainSphere.m_basicPixelConstData.useNormalMap);
		if (ImGui::Checkbox("Use NormalMapping", &temp)) {
			m_mainSphere.m_basicPixelConstData.useNormalMap = int(temp);
		}

		temp = bool(m_mainSphere.m_basicPixelConstData.useAOMap);
		if (ImGui::Checkbox("Use AO", &temp)) {
			m_mainSphere.m_basicPixelConstData.useAOMap = int(temp);
		}

		temp = bool(m_mainSphere.m_basicVertexConstData.useHeightMap);
		if (ImGui::Checkbox("Use HeightMapping", &temp)) {
			m_mainSphere.m_basicVertexConstData.useHeightMap = int(temp);
		}

		ImGui::SliderFloat("HeightScale",
						   &m_mainSphere.m_basicVertexConstData.heightScale,
						   0.0f, 0.1f);

		ImGui::TreePop( );
	}
	
	ImGui::SetNextItemOpen(true, ImGuiCond_Once);
	if (ImGui::TreeNode("Ground")) {
		bool temp = bool(m_meshGroupGround.m_basicPixelConstData.useAlbedoMap);
		if (ImGui::Checkbox("Use AlbedoTexture", &temp)) {
			m_meshGroupGround.m_basicPixelConstData.useAlbedoMap = int(temp);
		}

		temp = bool(m_meshGroupGround.m_basicPixelConstData.useNormalMap);
		if (ImGui::Checkbox("Use NormalMapping", &temp)) {
			m_meshGroupGround.m_basicPixelConstData.useNormalMap = int(temp);
		}

		temp = bool(m_meshGroupGround.m_basicPixelConstData.useAOMap);
		if (ImGui::Checkbox("Use AO", &temp)) {
			m_meshGroupGround.m_basicPixelConstData.useAOMap = int(temp);
		}

		temp = bool(m_meshGroupGround.m_basicVertexConstData.useHeightMap);
		if (ImGui::Checkbox("Use HeightMapping", &temp)) {
			m_meshGroupGround.m_basicVertexConstData.useHeightMap = int(temp);
		}

		ImGui::SliderFloat("HeightScale",
						   &m_meshGroupGround.m_basicVertexConstData.heightScale,
						   0.0f, 0.1f);

		ImGui::TreePop( );
	}*/

	// $m_tessellatedQuad
	/*int flag = 0;
	flag += ImGui::SliderFloat4(
		"Edges", &m_tessellatedQuad.m_constantData.edges.x, 1, 8);
	flag += ImGui::SliderFloat2(
		"Inside", &m_tessellatedQuad.m_constantData.inside.x, 1, 8);
	if (flag) {
		D3D11Utils::UpdateBuffer(m_device, m_context,
								 m_tessellatedQuad.m_constantData,
								 m_tessellatedQuad.m_constantBuffer);
	}*/

	//auto& meshGroup = m_visibleMeshIndex == 0 ? m_mainSphere : m_meshGroupCharacter;

	//if (ImGui::RadioButton("Sphere", m_visibleMeshIndex == 0)) {
	//	m_visibleMeshIndex = 0;
	//}
	//ImGui::SameLine( );
	//if (ImGui::RadioButton("Character", m_visibleMeshIndex == 1)) {
	//	m_visibleMeshIndex = 1;
	//}

	//ImGui::SliderFloat("Mipmaps Level", &m_mainSphere.m_basicPixelConstData.mipmapLevel, 0.0f, 10.0f);

	//ImGui::SliderFloat("Rim Strength",
	//			   &meshGroup.m_basicPixelConstData.rimStrength, 0.0f,
	//			   10.0f);
	//ImGui::Checkbox("Use Smoothstep",
	//				&meshGroup.m_basicPixelConstData.useSmoothstep);
	//ImGui::SliderFloat3("Rim Color", &meshGroup.m_basicPixelConstData.rimColor.x,
	//					0.0f, 1.0f);
	//ImGui::SliderFloat("Rim Power", &meshGroup.m_basicPixelConstData.rimPower,
	//				   0.01f, 10.0f);

	//ImGui::Checkbox("Wireframe", &m_drawAsWire);
	//ImGui::Checkbox("Draw Normals", &meshGroup.m_drawNormals);
	//if (ImGui::SliderFloat("Normal scale",	&meshGroup.m_normalVertexConstData.scale, 0.0f, 1.0f)) {
	//	meshGroup.m_drawNormalsDirtyFlag = true;
	//}

	////
	///*ImGui::SliderFloat3("m_modelTranslation", &m_modelTranslation.x, -2.0f, 2.0f);
	//ImGui::SliderFloat3("m_modelRotation", &m_modelRotation.x, -3.14f, 3.14f);
	//ImGui::SliderFloat3("m_modelScaling", &m_modelScaling.x, 0.1f, 2.0f);*/
	//
	//ImGui::SliderFloat3("Material FresnelR0",
	//					&meshGroup.m_basicPixelConstData.material.fresnelR0.x, 0.0f, 1.0f);

	//ImGui::SliderFloat("Material Diffuse", &m_materialDiffuse, 0.0f, 3.0f);
	//ImGui::SliderFloat("Material Specular", &m_materialSpecular, 0.0f, 3.0f);

	//ImGui::SliderFloat("Material Shininess",
	//				   &meshGroup.m_basicPixelConstData.material.shininess, 0.01f, 20.0f);

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

	//ImGui::SliderFloat3("Light Strength", &m_lightFromGUI.radiance.x, 0.0f, 0.1f);

	//ImGui::SliderFloat3("Light Position", &m_lightFromGUI.position.x, -5.0f, 5.0f);

	//ImGui::SliderFloat("Light fallOffStart", &m_lightFromGUI.fallOffStart, 0.0f, 5.0f);

	//ImGui::SliderFloat("Light fallOffEnd", &m_lightFromGUI.fallOffEnd, 0.0f, 10.0f);

	//ImGui::SliderFloat("Light spotPower", &m_lightFromGUI.spotPower, 1.0f, 512.0f);
	
}

} // namespace kusk