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

	// 조명 설정
	{
		m_light.position = Vector3(0.0f, 0.5f, 1.7f);
		m_light.radiance = Vector3(5.0f);
		m_light.fallOffEnd = 20.0f;
	}

	// 거울
	{
		auto mesh = GeometryGenerator::MakeSquare(0.48f);
		m_mirror = make_shared<BasicMeshGroup>(m_device, m_context, vector{ mesh });
		m_mirror->m_basicPixelConstData.material.albedo = Vector3(0.3f);
		m_mirror->m_basicPixelConstData.material.emission = Vector3(0.0f);
		m_mirror->m_basicPixelConstData.material.metallic = 0.7f;
		m_mirror->m_basicPixelConstData.material.roughness = 0.2f;

		m_mirror->UpdateModelWorld(
			Matrix::CreateScale(1.0f, 1.5f, 1.0f) *
			Matrix::CreateRotationY(3.141592f * 0.5f) *
			Matrix::CreateTranslation(0.5f, 0.25f, 2.0f));

		m_mirrorPlane = SimpleMath::Plane(Vector3(0.5f, 0.25f, 2.0f), Vector3(-1.0f, 0.0f, 0.0f));
	}

	// 바닥
	{
		auto mesh = GeometryGenerator::MakeSquare(2.0f);
		mesh.albedoTextureFilename = BLENDER_UV_GRID_2K_TEXTURE;
		m_ground = make_shared<BasicMeshGroup>(m_device, m_context, vector{ mesh });
		m_ground->m_basicPixelConstData.material.albedo = Vector3(0.2f);
		m_ground->m_basicPixelConstData.material.emission = Vector3(0.0f);
		m_ground->m_basicPixelConstData.material.metallic = 0.2f;
		m_ground->m_basicPixelConstData.material.roughness = 0.8f;
		m_ground->UpdateModelWorld(
			Matrix::CreateRotationX(3.141592f * 0.5f) *
			Matrix::CreateTranslation(0.0f, -0.5f, 2.0f));

		m_basicList.push_back(m_ground); // 리스트에 등록
	}

	// $mainObj
	{
		//auto meshes = GeometryGenerator::ReadFromFile(DAMAGED_HELMET_MODEL_DIR, DAMAGED_HELMAT_MODEL_FILENAME);
		
		//vector<MeshData> meshes = { GeometryGenerator::MakeBox(0.15f) };
		auto meshes = GeometryGenerator::ReadFromFile(ARMORED_FEMALE_SOLDIER_MODEL_DIR, ARMORED_FEMALE_SOLDIER_MODEL_FILENAME);
		
		meshes[ 0 ].albedoTextureFilename = ARMORED_FEMALE_SOLDIER_MODEL_DIR + string("angel_armor_albedo.jpg");
		meshes[ 0 ].emissiveTextureFilename = ARMORED_FEMALE_SOLDIER_MODEL_DIR + string("angel_armor_e.jpg");
		meshes[ 0 ].metallicTextureFilename = ARMORED_FEMALE_SOLDIER_MODEL_DIR + string("angel_armor_metalness.jpg");
		meshes[ 0 ].normalTextureFilename = ARMORED_FEMALE_SOLDIER_MODEL_DIR + string("angel_armor_normal.jpg");
		meshes[ 0 ].roughnessTextureFilename = ARMORED_FEMALE_SOLDIER_MODEL_DIR + string("angel_armor_roughness.jpg");

		Vector3 center(0.0f, 0.0f, 2.0f);
		m_mainObj = make_shared<BasicMeshGroup>(m_device, m_context, meshes);
		m_mainObj->m_basicPixelConstData.invertNormalMapY = false; // GLTF는 true로
		m_mainObj->m_irradianceSRV = m_cubeMapping.m_irradianceSRV;
		m_mainObj->m_specularSRV = m_cubeMapping.m_specularSRV;
		m_mainObj->m_brdfSRV = m_cubeMapping.m_brdfSRV;
		m_mainObj->UpdateModelWorld(Matrix::CreateTranslation(center));
		m_mainObj->UpdateConstantBuffers(m_device, m_context);

		m_basicList.push_back(m_mainObj); // 리스트에 등록

		// 동일한 크기와 위치에 BoundingSphere 만들기
		m_mainBoundingSphere = BoundingSphere(center, 0.4f);
	}

	// 조명 위치 표시
	{
		MeshData sphere = GeometryGenerator::MakeSphere(0.01f, 10, 10);
		m_lightSphere = make_shared<BasicMeshGroup>(m_device, m_context, vector{ sphere });
		m_lightSphere->UpdateModelWorld(Matrix::CreateTranslation(m_light.position));
		m_lightSphere->m_basicPixelConstData.material.albedo = Vector3(0.0f);
		m_lightSphere->m_basicPixelConstData.material.emission = Vector3(1.0f, 1.0f, 0.0f);
		m_lightSphere->UpdateConstantBuffers(m_device, m_context);

		m_basicList.push_back(m_lightSphere); // 리스트에 등록
	}

	// 커서 표시 
	// - main sphere와의 충돌이 감지되면 월드 공간에 작게 그려지는 구
	{
		MeshData sphere = GeometryGenerator::MakeSphere(0.01f, 10, 10);
		m_cursorSphere.Initialize(m_device, m_context, vector{ sphere });
		m_cursorSphere.UpdateModelWorld(Matrix::CreateTranslation(Vector3(0.0f)));
		m_cursorSphere.m_basicPixelConstData.material.albedo = Vector3(0.0f);
		m_cursorSphere.m_basicPixelConstData.material.emission = Vector3(0.0f, 1.0f, 1.0f);
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

	// 반사 행렬 추가
	Vector3 eyeWorld = m_camera.GetEyePos( );
	Matrix reflectionRow = Matrix::CreateReflection(m_mirrorPlane);
	Matrix viewRow = m_camera.GetViewRow( );
	Matrix projRow = m_camera.GetProjRow( );

	AppBase::UpdateEyeViewProjBuffers(eyeWorld, viewRow, projRow, reflectionRow);
	
	// 큐브매핑을 위한 constantBuffers 업데이트
	m_cubeMapping.UpdateViewProjConstBuffer(m_device, m_context, viewRow, projRow, reflectionRow);

	// mainSphere의 회전 및 이동 계산용
	static float prevRatio = 0.0f;
	Quaternion q = Quaternion::CreateFromAxisAngle(Vector3(1.0f, 0.0f, 0.0f), 0.0f);
	static Vector3 prevPos(0.0f);
	static Vector3 prevVector(0.0f);
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
			m_cursorSphere.UpdateConstantBuffers(m_device, m_context);

			// mainSphere를 어떻게 회전시킬지 결정
			if (m_leftButton)
			{
				if (m_dragStartFlag) { // 드래그를 시작하는 경우
					m_dragStartFlag = false;

					prevVector = pickPoint - Vector3(m_mainBoundingSphere.Center);
					prevVector.Normalize( );
				}
				else {
					Vector3 currentVector = pickPoint - Vector3(m_mainBoundingSphere.Center);
					currentVector.Normalize( );
					float theta = acos(prevVector.Dot(currentVector));
					// 마우스가 조금이라도 움직였을 경우에만 회전시키기
					if (theta > 3.141592f / 180.0f * 3.0f) {
						Vector3 axis = prevVector.Cross(currentVector);
						axis.Normalize( );
						q = SimpleMath::Quaternion::CreateFromAxisAngle(axis, theta);
						prevVector = currentVector;
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

	// 거울은 따로 처리
	m_mirror->m_basicPixelConstData.lights[ 1 ] = m_light;
	m_mirror->UpdateConstantBuffers(m_device, m_context);

	// 조명의 위치 반영
	m_lightSphere->UpdateModelWorld(Matrix::CreateTranslation(m_light.position));

	// 마우스 이동/회전 반영
	Vector3 translation = m_mainObj->m_modelWorldRow.Translation( );
	m_mainObj->m_modelWorldRow.Translation(Vector3(0.0f));
	m_mainObj->UpdateModelWorld(m_mainObj->m_modelWorldRow* Matrix::CreateFromQuaternion(q)*
								Matrix::CreateTranslation(dragTranslation + translation));
	m_mainBoundingSphere.Center = m_mainObj->m_modelWorldRow.Translation( );

	// 거울 외의 물체들은 for문으로 처리
	for (auto& i : m_basicList) {
		// 조명 설정 (쉐이더의 Light 배열에서 인덱스 1이 포인트 라이트)
		i->m_basicPixelConstData.lights[ 1 ] = m_light;
		i->UpdateConstantBuffers(m_device, m_context);
	}
	
	// $tessellatedQuad
	/*m_tessellatedQuad.m_constantData.eyeWorld = eyeWorld;
	m_tessellatedQuad.m_constantData.model = Matrix( );
	m_tessellatedQuad.m_constantData.view = viewRow.Transpose( );
	m_tessellatedQuad.m_constantData.proj = projRow.Transpose( );
	D3D11Utils::UpdateBuffer(m_device, m_context, m_tessellatedQuad.m_constantData, m_tessellatedQuad.m_constantBuffer);*/

	
	/*D3D11Utils::UpdateBuffer(m_device, m_context, m_billboardPoints.m_constantData, m_billboardPoints.m_constantBuffer);*/
}

void KuskApp::Render() {
	
	SetViewport( );
	
	const float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	vector<ID3D11RenderTargetView*> renderTargetViews = { m_floatRTV.Get( ) };
	for (size_t i = 0; i < renderTargetViews.size( ); i++) {
		m_context->ClearRenderTargetView(renderTargetViews[ i ], clearColor);
	}
	m_context->OMSetRenderTargets(UINT(renderTargetViews.size( )), renderTargetViews.data( ), m_depthStencilView.Get( ));
	
	/* 거울 1. 원래대로 한 번 그림 */

	// 기본 BlendState 사용
	m_context->OMSetBlendState(NULL, NULL, 0xffffffff);
	m_context->ClearDepthStencilView(m_depthStencilView.Get( ), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	m_context->OMSetDepthStencilState(m_drawDSS.Get(), 1);

	m_context->RSSetState(m_drawAsWire ? m_wireRS.Get( ) : m_solidRS.Get());

	// 거울은 빼고 그림.
	for (auto& i : m_basicList) {
		i->Render(m_context, AppBase::m_eyeViewProjConstBuffer, m_useEnv);
	}

	if ((m_leftButton || m_rightButton) && m_selected)
		m_cursorSphere.Render(m_context, AppBase::m_eyeViewProjConstBuffer, false);

	if (m_useEnv) {
		m_cubeMapping.Render(m_context, false);
	}

	//m_billboardPoints.Render(m_context);
	//m_tessellatedQuad.Render(m_context);


	/* 거울 2. 거울 위치만 StencilBuffer에 1로 표기 */

	// STENCIL만 클리어
	// 거울을 가리는 물체가 있을 수도 있어서 Depth는 CLEAR 안함
	// 앞 단계의 m_drawDSS에서 모두 KEEP을 사용했기 때문에
	// Stencil도 CLEAR 불필요
	// m_context->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_STENCIL, 1.0f, 0);

	// 두 번째 UINT StencilRef = 1 사용
	// ClearDepthStencilView(..., 0) 에서와는 다른 숫자 사용)
	m_context->OMSetDepthStencilState(m_maskDSS.Get( ), 1);

	// 거울을 그릴 때 색은 필요 없기 때문에 간단한 PS 사용 가능 :TODO:
	m_mirror->Render(m_context, AppBase::m_eyeViewProjConstBuffer, m_useEnv);


	/* 거울 3. 거울 위치에 반사된 물체들을 렌더링 */
	m_context->ClearDepthStencilView(m_depthStencilView.Get( ), D3D11_CLEAR_DEPTH, 1.0f, 0);
	m_context->OMSetDepthStencilState(m_drawMaskedDSS.Get( ), 1);

	// 반사되면 삼각형 정점들의 순서(Winding)가 반대로 -> 반시계
	m_context->RSSetState(m_drawAsWire ? m_wireCCWRS.Get( ) : m_solidCCWRS.Get( ));

	// 반사된 위치에 그리기
	for (auto& i : m_basicList) {
		i->Render(m_context, AppBase::m_mirrorEyeViewProjConstBuffer, m_useEnv);
	}

	// 환경맵도 뒤집어서 그리기
	if (m_useEnv) {
		m_cubeMapping.Render(m_context, true);
	}


	/* 거울 4. 거울 자체의 재질을 "Blend"로 그림 */

	const float t = m_mirrorAlpha;
	const FLOAT blendColor[ 4 ] = { t, t, t, 1.0f };
	m_context->OMSetBlendState(m_mirrorBS.Get( ), blendColor, 0xffffffff);
	
	// 다시 시계 방향
	m_context->RSSetState(m_drawAsWire ? m_wireRS.Get( ) : m_solidRS.Get( ));
	
	// 거울 그리기
	m_mirror->Render(m_context, AppBase::m_eyeViewProjConstBuffer, m_useEnv);

	// 후처리는 Blend X
	m_context->OMSetBlendState(NULL, NULL, 0xffffffff);

	/* 이후 원래 하던 후처리 */

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
		ImGui::Checkbox("Use Env", &m_useEnv);

		int flag = 0;
		flag += ImGui::RadioButton("Env", &m_cubeMapping.m_pixelConstData.textureToDraw, 0);
		ImGui::SameLine( );
		flag += ImGui::RadioButton("Specular", &m_cubeMapping.m_pixelConstData.textureToDraw, 1);
		ImGui::SameLine( );
		flag += ImGui::RadioButton("Irradiance", &m_cubeMapping.m_pixelConstData.textureToDraw, 2);
		flag += ImGui::SliderFloat("Mip Level", &m_cubeMapping.m_pixelConstData.mipLevel, 0.0f, 10.0f);

		if (flag) {
			m_cubeMapping.UpdatePixelConstBuffer(m_device, m_context);
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
	if (ImGui::TreeNode("Mirror")) {
		int flag = 0;
		flag += ImGui::SliderFloat("Alpha", &m_mirrorAlpha, 0.0f, 1.0f);
		flag += ImGui::SliderFloat(
			"Metallic", &m_mirror->m_basicPixelConstData.material.metallic,
			0.0f, 1.0f);
		flag += ImGui::SliderFloat(
			"Roughness", &m_mirror->m_basicPixelConstData.material.roughness,
			0.0f, 1.0f);
		if (flag) {
			m_mirror->UpdateConstantBuffers(m_device, m_context);
		}
		ImGui::TreePop( );
	}

	ImGui::SetNextItemOpen(true, ImGuiCond_Once);
	if (ImGui::TreeNode("Point Light")) {
		ImGui::SliderFloat3("Position", &m_light.position.x, -5.0f, 5.0f);
		ImGui::TreePop( );
	}

	// $mainObj
	ImGui::SetNextItemOpen(true, ImGuiCond_Once);
	if (ImGui::TreeNode("Material")) {
		int flag = 0;
		flag += ImGui::SliderFloat("Metallic", &m_ground->m_basicPixelConstData.material.metallic, 0.0f, 1.0f);
		flag += ImGui::SliderFloat("Roughness", &m_ground->m_basicPixelConstData.material.roughness, 0.0f, 1.0f);
		flag += ImGui::CheckboxFlags("Use AlbedoTexture", &m_mainObj->m_basicPixelConstData.useAlbedoMap, 1);
		flag += ImGui::CheckboxFlags("Use EmissiveTexture", &m_mainObj->m_basicPixelConstData.useEmissiveMap, 1);
		flag += ImGui::CheckboxFlags("Use NormalMapping", &m_mainObj->m_basicPixelConstData.useNormalMap, 1);
		flag += ImGui::CheckboxFlags("Use AO", &m_mainObj->m_basicPixelConstData.useAOMap, 1);
		flag += ImGui::CheckboxFlags("Use HeightMapping", &m_mainObj->m_basicVertexConstData.useHeightMap, 1);
		flag += ImGui::SliderFloat("HeightScale", &m_mainObj->m_basicVertexConstData.heightScale, 0.0f, 0.1f);
		flag += ImGui::CheckboxFlags("Use MetallicMap", &m_mainObj->m_basicPixelConstData.useMetallicMap, 1);
		flag += ImGui::CheckboxFlags("Use RoughnessMap", &m_mainObj->m_basicPixelConstData.useRoughnessMap, 1);
		flag += ImGui::Checkbox("Draw Normals", &m_mainObj->m_drawNormals);

		if (flag) {
			// GUI 입력이 있을 때만 할 일들 추가.. (근데 어처피 Update에서 계속 업데이트 해주기 때문에 패스)
		}

		ImGui::TreePop( );
	}
	

	/*
		
	ImGui::SetNextItemOpen(true, ImGuiCond_Once);
	if (ImGui::TreeNode("Ground")) {
		bool temp = bool(m_ground.m_basicPixelConstData.useAlbedoMap);
		if (ImGui::Checkbox("Use AlbedoTexture", &temp)) {
			m_ground.m_basicPixelConstData.useAlbedoMap = int(temp);
		}

		temp = bool(m_ground.m_basicPixelConstData.useNormalMap);
		if (ImGui::Checkbox("Use NormalMapping", &temp)) {
			m_ground.m_basicPixelConstData.useNormalMap = int(temp);
		}

		temp = bool(m_ground.m_basicPixelConstData.useAOMap);
		if (ImGui::Checkbox("Use AO", &temp)) {
			m_ground.m_basicPixelConstData.useAOMap = int(temp);
		}

		temp = bool(m_ground.m_basicVertexConstData.useHeightMap);
		if (ImGui::Checkbox("Use HeightMapping", &temp)) {
			m_ground.m_basicVertexConstData.useHeightMap = int(temp);
		}

		ImGui::SliderFloat("HeightScale",
						   &m_ground.m_basicVertexConstData.heightScale,
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

	ImGui::SetNextItemOpen(false, ImGuiCond_Once);
	if (ImGui::TreeNode("Rim Effect")) {

		ImGui::SliderFloat("Rim Strength",
					   &m_mainObj->m_basicPixelConstData.rimStrength, 0.0f,
					   10.0f);
		ImGui::Checkbox("Use Smoothstep",
						&m_mainObj->m_basicPixelConstData.useSmoothstep);
		ImGui::SliderFloat3("Rim Color", &m_mainObj->m_basicPixelConstData.rimColor.x,
							0.0f, 1.0f);
		ImGui::SliderFloat("Rim Power", &m_mainObj->m_basicPixelConstData.rimPower,
						   0.01f, 10.0f);
		ImGui::TreePop( );
	}

}

} // namespace kusk