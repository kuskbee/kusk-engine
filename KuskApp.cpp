#include "KuskApp.h"

#include <DirectXCollision.h> // 구와 광선 충돌 계산에 사용
#include <directxtk/DDSTextureLoader.h>
#include <vector>

#include "textureResources.h"
#include "GeometryGenerator.h"
#include "GraphicsCommon.h"

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

	AppBase::InitCubemaps(OUTDOOR_BASE_PATH, OUTDOOR_ORGN_DDS, OUTDOOR_SPEC_DDS, OUTDOOR_DIFF_DDS, OUTDOOR_BRDF_DDS);

	// 후처리용 화면 사각형
	{
		MeshData meshData = GeometryGenerator::MakeSquare( );
		m_screenSquare = make_shared<Model>(m_device, m_context, vector{ meshData });
	}

	// 환경 박스 초기화
	{
		MeshData skyboxMesh = GeometryGenerator::MakeBox(40.0f);
		std::reverse(skyboxMesh.indices.begin( ), skyboxMesh.indices.end( ));
		m_skybox = make_shared<Model>(m_device, m_context, vector{ skyboxMesh });
	}

	// 바닥(거울)
	{
		auto mesh = GeometryGenerator::MakeSquare(5.0f);
		//mesh.albedoTextureFilename = BLENDER_UV_GRID_2K_TEXTURE;
		m_ground = make_shared<Model>(m_device, m_context, vector{ mesh });
		m_ground->m_materialConstsCPU.albedoFactor = Vector3(0.1f);
		m_ground->m_materialConstsCPU.emissionFactor = Vector3(0.0f);
		m_ground->m_materialConstsCPU.metallicFactor = 0.5f;
		m_ground->m_materialConstsCPU.roughnessFactor = 0.3f;

		Vector3 position = Vector3(0.0f, -0.5f, 2.0f);
		m_ground->UpdateWorldRow(
			Matrix::CreateRotationX(3.141592f * 0.5f) *
			Matrix::CreateTranslation(position));

		m_mirrorPlane = SimpleMath::Plane(position, Vector3(0.0f, 1.0f, 0.0f));
		m_mirror = m_ground; // 바닥에 거울처럼 반사 구현
		// m_basicList.push_back(m_ground); // 거울은 리스트에 등록 X
	}

	// $box - test용
	{
		vector<MeshData> meshes = { GeometryGenerator::MakeBox(0.15f) };
		Vector3 center(1.5f, 0.25f, 2.0f);
		m_box = make_shared<Model>(m_device, m_context, meshes);
		m_box->UpdateWorldRow(Matrix::CreateTranslation(center));

		m_basicList.push_back(m_box); // 리스트에 등록
	}


	// $mainObj
	{
		//auto meshes = GeometryGenerator::ReadFromFile(DAMAGED_HELMET_MODEL_DIR, DAMAGED_HELMAT_MODEL_FILENAME);

		auto meshes = GeometryGenerator::ReadFromFile(VAGRANT_KNIGHTS_MODEL_DIR, VAGRANT_KNIGHTS_MODEL_FILENAME, true);
		
		//vector<MeshData> meshes = { GeometryGenerator::MakeSphere(0.4f, 50, 50) };
		/*auto meshes = GeometryGenerator::ReadFromFile(ARMORED_FEMALE_SOLDIER_MODEL_DIR, ARMORED_FEMALE_SOLDIER_MODEL_FILENAME, true);
		meshes[ 0 ].albedoTextureFilename = ARMORED_FEMALE_SOLDIER_MODEL_DIR + string("angel_armor_albedo.jpg");
		meshes[ 0 ].emissiveTextureFilename = ARMORED_FEMALE_SOLDIER_MODEL_DIR + string("angel_armor_e.jpg");
		meshes[ 0 ].metallicTextureFilename = ARMORED_FEMALE_SOLDIER_MODEL_DIR + string("angel_armor_metalness.jpg");
		meshes[ 0 ].normalTextureFilename = ARMORED_FEMALE_SOLDIER_MODEL_DIR + string("angel_armor_normal.jpg");
		meshes[ 0 ].roughnessTextureFilename = ARMORED_FEMALE_SOLDIER_MODEL_DIR + string("angel_armor_roughness.jpg");*/

		Vector3 center(0.0f, 0.0f, 2.0f);
		m_mainObj = make_shared<Model>(m_device, m_context, meshes);
		m_mainObj->m_materialConstsCPU.invertNormalMapY = true; // GLTF는 true로
		m_mainObj->m_materialConstsCPU.albedoFactor = Vector3(1.0f);
		m_mainObj->m_materialConstsCPU.roughnessFactor = 0.3f;
		m_mainObj->m_materialConstsCPU.metallicFactor = 0.8f;
		m_mainObj->m_materialConstsCPU.emissionFactor = Vector3(0.0f);
		m_mainObj->UpdateWorldRow(Matrix::CreateTranslation(center));

		m_basicList.push_back(m_mainObj); // 리스트에 등록

		// 동일한 크기와 위치에 BoundingSphere 만들기
		m_mainBoundingSphere = BoundingSphere(center, 0.4f);
	}

	// 추가 물체1 (파란 구)
	{
		MeshData mesh = GeometryGenerator::MakeSphere(0.2f, 200, 200);
		Vector3 center(0.5f, 0.5f, 2.0f);
		auto obj = make_shared<Model>(m_device, m_context, vector{ mesh });
		obj->UpdateWorldRow(Matrix::CreateTranslation(center));
		obj->m_materialConstsCPU.albedoFactor = Vector3(0.1f, 0.1f, 1.0f);
		obj->m_materialConstsCPU.roughnessFactor = 0.2f;
		obj->m_materialConstsCPU.metallicFactor = 0.6f;
		obj->m_materialConstsCPU.emissionFactor = Vector3(0.0f);
		obj->UpdateConstantBuffers(m_device, m_context);

		m_basicList.push_back(obj);
	}

	// 추가 물체2 (빨간 박스)
	{
		MeshData mesh = GeometryGenerator::MakeBox(0.2f);
		Vector3 center(0.0f, 0.5f, 2.5f);
		auto obj = make_shared<Model>(m_device, m_context, vector{ mesh });
		obj->UpdateWorldRow(Matrix::CreateTranslation(center));
		obj->m_materialConstsCPU.albedoFactor = Vector3(1.0f, 0.2f, 0.2f);
		obj->m_materialConstsCPU.roughnessFactor = 0.5f;
		obj->m_materialConstsCPU.metallicFactor = 0.9f;
		obj->m_materialConstsCPU.emissionFactor = Vector3(0.0f);
		obj->UpdateConstantBuffers(m_device, m_context);

		m_basicList.push_back(obj);
	}

	// 조명 설정
	{
		// 조명 0은 고정
		m_globalConstsCPU.lights[ 0 ].radiance = Vector3(5.0f);
		m_globalConstsCPU.lights[ 0 ].position = Vector3(0.0f, 1.5f, 1.1f);
		m_globalConstsCPU.lights[ 0 ].direction = Vector3(0.0f, -1.0f, 0.0f);
		m_globalConstsCPU.lights[ 0 ].spotPower = 3.0f;
		m_globalConstsCPU.lights[ 0 ].radius = 0.02f;
		m_globalConstsCPU.lights[ 0 ].type =
			LIGHT_SPOT | LIGHT_SHADOW; // Spot with shadow

		// 조명 1의 위치와 방향은 Update()에서 설정
		m_globalConstsCPU.lights[ 1 ].radiance = Vector3(5.0f);
		m_globalConstsCPU.lights[ 1 ].spotPower = 3.0f;
		m_globalConstsCPU.lights[ 1 ].fallOffEnd = 20.0f;
		m_globalConstsCPU.lights[ 1 ].radius = 0.02f;
		m_globalConstsCPU.lights[ 1 ].type =
			LIGHT_SPOT | LIGHT_SHADOW; // Spot with shadow

		// 조명 2는 꺼놓음
		m_globalConstsCPU.lights[ 2 ].type = LIGHT_OFF;
	}

	// 조명 위치 표시
	{
		for (int i = 0; i < MAX_LIGHTS; i++) {
			MeshData sphere = GeometryGenerator::MakeSphere(1.0f, 20, 20);
			m_lightSphere[ i ] = make_shared<Model>(m_device, m_context, vector{ sphere });
			m_lightSphere[ i ]->UpdateWorldRow(Matrix::CreateTranslation(m_globalConstsCPU.lights[ i ].position));
			m_lightSphere[ i ]->m_materialConstsCPU.albedoFactor = Vector3(0.0f);
			m_lightSphere[ i ]->m_materialConstsCPU.emissionFactor = Vector3(1.0f, 1.0f, 0.0f);
			m_lightSphere[ i ]->m_castShadow = false; // 조명 표시 물체들은 그림자 X

			if (m_globalConstsCPU.lights[ i ].type == 0) {
				m_lightSphere[ i ]->m_isVisible = false;
			}
			m_basicList.push_back(m_lightSphere[ i ]); // 리스트에 등록
		}
	}

	// 커서 표시 
	// - main sphere와의 충돌이 감지되면 월드 공간에 작게 그려지는 구
	{
		MeshData sphere = GeometryGenerator::MakeSphere(0.01f, 10, 10);
		m_cursorSphere = make_shared<Model>(m_device, m_context, vector{ sphere });
		m_cursorSphere->m_isVisible = false; // 마우스가 눌렸을 때만 보임
		m_cursorSphere->m_castShadow = false; // 그림자 X
		m_cursorSphere->m_materialConstsCPU.albedoFactor = Vector3(0.0f);
		m_cursorSphere->m_materialConstsCPU.emissionFactor = Vector3(0.0f, 1.0f, 1.0f);
		
		m_basicList.push_back(m_cursorSphere); // 리스트에 등록
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

void KuskApp::UpdateLights(float dt) {
	// 회전하는 light[1] 업데이트 (Dev=Deviation 편차)
	static Vector3 lightDev = Vector3(1.0f, 0.0f, 0.0f);
	if (m_lightRotate) {
		lightDev = Vector3::Transform(lightDev, Matrix::CreateRotationY(dt * 3.141592f * 0.5f));
	}
	m_globalConstsCPU.lights[ 1 ].position = Vector3(0.0f, 1.1f, 2.0f) + lightDev;
	Vector3 focusPosition = Vector3(0.0f, -0.5f, 1.7f);
	m_globalConstsCPU.lights[ 1 ].direction = focusPosition - m_globalConstsCPU.lights[ 1 ].position;
	m_globalConstsCPU.lights[ 1 ].direction.Normalize( );

	// 그림자맵을 만들기 위한 시점
	for (int i = 0; i < MAX_LIGHTS; i++) {
		const auto& light = m_globalConstsCPU.lights[ i ];
		if (light.type & LIGHT_SHADOW) {

			Vector3 up = Vector3(0.0f, 1.0f, 0.0f);
			if (abs(up.Dot(light.direction) + 1.0f) < 1e-5)
				up = Vector3(1.0f, 0.0f, 0.0f);

			// 그림자맵을 만들 때 필요
			Matrix lightViewRow = XMMatrixLookAtLH(
				light.position, light.position + light.direction, up);

			Matrix lightProjRow = XMMatrixPerspectiveFovLH(
				XMConvertToRadians(120.0f), 1.0f, 0.1f, 10.0f);

			m_shadowGlobalConstsCPU[ i ].eyeWorld = light.position;
			m_shadowGlobalConstsCPU[ i ].view = lightViewRow.Transpose( );
			m_shadowGlobalConstsCPU[ i ].proj = lightProjRow.Transpose( );
			m_shadowGlobalConstsCPU[ i ].invProj = lightProjRow.Invert( ).Transpose( );
			m_shadowGlobalConstsCPU[ i ].viewProj = (lightViewRow * lightProjRow).Transpose( );

			// LIGHT_FRUSTUM_WIDTH 확인
		   // Vector4 eye(0.0f, 0.0f, 0.0f, 1.0f);
		   // Vector4 xLeft(-1.0f, -1.0f, 0.0f, 1.0f);
		   // Vector4 xRight(1.0f, 1.0f, 0.0f, 1.0f);
		   // eye = Vector4::Transform(eye, lightProjRow);
		   // xLeft = Vector4::Transform(xLeft, lightProjRow.Invert());
		   // xRight = Vector4::Transform(xRight, lightProjRow.Invert());
		   // xLeft /= xLeft.w;
		   // xRight /= xRight.w;
		   // cout << "LIGHT_FRUSTUM_WIDTH = " << xRight.x - xLeft.x << endl;

			D3D11Utils::UpdateBuffer(m_device, m_context, m_shadowGlobalConstsCPU[ i ], m_shadowGlobalConstsGPU[ i ]);

			// 그림자를 실제로 렌더링할 때 필요
			m_globalConstsCPU.lights[ i ].viewProj = m_shadowGlobalConstsCPU[i].viewProj;
			m_globalConstsCPU.lights[ i ].invProj = m_shadowGlobalConstsCPU[i].invProj;
		}
	}
}

void KuskApp::Update(float dt) {
	
	// 카메라의 이동
	m_camera.UpdateKeyboard(dt, m_keyPressed);

	// 반사 행렬 추가
	const Vector3 eyeWorld = m_camera.GetEyePos( );
	const Matrix reflectRow = Matrix::CreateReflection(m_mirrorPlane);
	const Matrix viewRow = m_camera.GetViewRow( );
	const Matrix projRow = m_camera.GetProjRow( );
	
	// 조명 업데이트
	UpdateLights(dt);

	// 공용 ConstantsBuffer 업데이트
	AppBase::UpdateGlobalConstants(eyeWorld, viewRow, projRow, reflectRow, m_mirrorPlane);
	
	// 거울은 따로 처리
	m_mirror->UpdateConstantBuffers(m_device, m_context);

	// 조명의 위치 반영
	for (int i = 0; i < MAX_LIGHTS; i++) {
		m_lightSphere[ i ]->UpdateWorldRow(
			Matrix::CreateScale(std::max(0.01f, m_globalConstsCPU.lights[i].radius)) * 
			Matrix::CreateTranslation(m_globalConstsCPU.lights[ i ].position));
	}

	// 마우스 이동/회전 반영
	if (m_leftButton || m_rightButton) {
		Quaternion q;
		Vector3 dragTranslation;
		Vector3 pickPoint;
		if (UpdateMouseControl(m_mainBoundingSphere, q, dragTranslation, pickPoint)) {
			Vector3 translation = m_mainObj->m_worldRow.Translation( );
			m_mainObj->m_worldRow.Translation(Vector3(0.0f));
			m_mainObj->UpdateWorldRow(
				m_mainObj->m_worldRow * Matrix::CreateFromQuaternion(q) *
				Matrix::CreateTranslation(dragTranslation + translation));
			m_mainBoundingSphere.Center = m_mainObj->m_worldRow.Translation( );

			// 충돌 지점에 작은 구 그리기
			m_cursorSphere->m_isVisible = true;
			m_cursorSphere->UpdateWorldRow(Matrix::CreateTranslation(pickPoint));
		}
		else {
			m_cursorSphere->m_isVisible = false;
		}
	}
	else {
		m_cursorSphere->m_isVisible = false;
	}

	// 거울 외의 물체들은 for문으로 처리
	for (auto& i : m_basicList) {
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
	
	AppBase::SetMainViewport( );

	// 모든 샘플러들을 공통으로 사용
	m_context->VSSetSamplers(0, UINT(Graphics::sampleStates.size( )), Graphics::sampleStates.data( ));
	m_context->PSSetSamplers(0, UINT(Graphics::sampleStates.size( )), Graphics::sampleStates.data( ));

	// 공용 텍스쳐들 "Common.hlsli"에서 register(t10)부터 시작
	vector<ID3D11ShaderResourceView*> commonSRVs = {
		m_envSRV.Get( ), m_specularSRV.Get( ), m_irradianceSRV.Get( ), m_brdfSRV.Get( ) };
	m_context->PSSetShaderResources(10, UINT(commonSRVs.size( )), commonSRVs.data( ));
	
	const float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	vector<ID3D11RenderTargetView*> rtvs = { m_floatRTV.Get( ) };
	
	// Depth Only Pass (RTV 생략 가능)
	m_context->OMSetRenderTargets(0, NULL, m_depthOnlyDSV.Get( ));
	m_context->ClearDepthStencilView(m_depthOnlyDSV.Get( ), D3D11_CLEAR_DEPTH, 1.0f, 0);
	
	AppBase::SetPipelineState(Graphics::depthOnlyPSO);
	AppBase::SetGlobalConsts(m_globalConstsGPU);
	for (auto& i : m_basicList)
		i->Render(m_context);

	m_skybox->Render(m_context);
	m_mirror->Render(m_context);

	// 그림자맵 만들기
	AppBase::SetShadowViewport( ); // 그림자맵 해상도
	AppBase::SetPipelineState(Graphics::depthOnlyPSO);
	for (int i = 0; i < MAX_LIGHTS; i++) {
		if (m_globalConstsCPU.lights[ i ].type & LIGHT_SHADOW) {
			// RTV 생략 가능
			m_context->OMSetRenderTargets(0, NULL, m_shadowDSVs[ i ].Get( ));
			m_context->ClearDepthStencilView(m_shadowDSVs[ i ].Get( ), D3D11_CLEAR_DEPTH, 1.0f, 0);
			AppBase::SetGlobalConsts(m_shadowGlobalConstsGPU[ i ]);
			for (auto& o : m_basicList) {
				if (o->m_castShadow && o->m_isVisible) {
					o->Render(m_context);
				}
			}
			m_skybox->Render(m_context);
			m_mirror->Render(m_context);
		}
	}
	
	// 다시 렌더링 해상도로 되돌리기
	AppBase::SetMainViewport( );

	/* 거울 1. 거울은 빼고 원래대로 그리기 */
	for (size_t i = 0; i < rtvs.size( ); i++) {
		m_context->ClearRenderTargetView(rtvs[ i ], clearColor);
	}
	m_context->OMSetRenderTargets(UINT(rtvs.size( )), rtvs.data( ), m_depthStencilView.Get( ));

	// 그림자맵들도 공용 텍스쳐들 이후에 추가
	// 유의 : 마지막 shadowDSV를 RenderTarget에서 해제한 후 설정
	vector<ID3D11ShaderResourceView*> shadowSRVs;
	for (int i = 0; i < MAX_LIGHTS; i++) {
		shadowSRVs.push_back(m_shadowSRVs[ i ].Get( ));
	}
	m_context->PSSetShaderResources(15, UINT(shadowSRVs.size( )), shadowSRVs.data( ));

	m_context->ClearDepthStencilView(m_depthStencilView.Get( ), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	AppBase::SetPipelineState(m_drawAsWire ? Graphics::defaultWirePSO
										   : Graphics::defaultSolidPSO);
	AppBase::SetGlobalConsts(m_globalConstsGPU);

	for (auto& i : m_basicList) {
		i->Render(m_context);
	}

	// 거울 반사를 그릴 필요가 없으면 불투명 거울만 그리기
	if (m_mirrorAlpha == 1.0f) {
		m_mirror->Render(m_context);
	}
	
	AppBase::SetPipelineState(Graphics::normalsPSO);
	for (auto& i : m_basicList) {
		if (i->m_drawNormals) {
			i->RenderNormals(m_context);
		}
	}

	AppBase::SetPipelineState(m_drawAsWire ? Graphics::skyboxWirePSO
										   : Graphics::skyboxSolidPSO);

	m_skybox->Render(m_context);

	//m_billboardPoints.Render(m_context);
	//m_tessellatedQuad.Render(m_context);

	if (m_mirrorAlpha < 1.0f) { // 거울을 그려야하는 상황

		/* 거울 2. 거울 위치만 StencilBuffer에 1로 표기 */
		AppBase::SetPipelineState(Graphics::stencilMaskPSO);

		m_mirror->Render(m_context);

		/* 거울 3. 거울 위치에 반사된 물체들을 렌더링 */
		AppBase::SetPipelineState(m_drawAsWire ? Graphics::reflectWirePSO
											   : Graphics::reflectSolidPSO);
		AppBase::SetGlobalConsts(m_reflectGlobalConstsGPU);

		m_context->ClearDepthStencilView(m_depthStencilView.Get( ), D3D11_CLEAR_DEPTH, 1.0f, 0);

		// 반사된 위치에 그리기
		for (auto& i : m_basicList) {
			i->Render(m_context);
		}

		AppBase::SetPipelineState(m_drawAsWire ? Graphics::reflectSkyboxWirePSO
											   : Graphics::reflectSkyboxSolidPSO);

		m_skybox->Render(m_context);


		/* 거울 4. 거울 자체의 재질을 "Blend"로 그림 */
		AppBase::SetPipelineState(m_drawAsWire ? Graphics::mirrorBlendWirePSO
											   : Graphics::mirrorBlendSolidPSO);
		AppBase::SetGlobalConsts(m_globalConstsGPU);

		m_mirror->Render(m_context);

	} // end of if (m_mirrorAlpha < 1.0f)

	// MSAA로 Texture2DMS에 렌더링된 결과를 Texture2D로 변환(Resolve)
	m_context->ResolveSubresource(m_resolvedBuffer.Get( ), 0, m_floatBuffer.Get( ), 0, DXGI_FORMAT_R16G16B16A16_FLOAT);
	
	// PostEffects
	AppBase::SetPipelineState(Graphics::postEffectsPSO);
	vector<ID3D11ShaderResourceView*> postEffectsSRVs = {
		m_resolvedSRV.Get( ), m_depthOnlySRV.Get( ) };	// 20번에 넣어줌.
	AppBase::SetGlobalConsts(m_globalConstsGPU);

	// 그림자맵 확인용 임시
	/*AppBase::SetGlobalConsts(m_shadowGlobalConstsGPU[ 1 ]);
	vector<ID3D11ShaderResourceView*> postEffectsSRVs = {
		m_resolvedSRV.Get( ), m_shadowSRVs[1].Get()};*/
	
	m_context->PSSetShaderResources(20, UINT(postEffectsSRVs.size( )), postEffectsSRVs.data( ));
	m_context->OMSetRenderTargets(1, m_postEffectsRTV.GetAddressOf( ), NULL);
	m_context->PSSetConstantBuffers(3, 1, m_postEffectsConstsGPU.GetAddressOf( ));
	m_screenSquare->Render(m_context);

	// 단순 이미지 처리와 블룸
	AppBase::SetPipelineState(Graphics::postProcessingPSO);
	m_postProcess.Render(m_context);
}

void KuskApp::UpdateGUI() {

	ImGui::SetNextItemOpen(false, ImGuiCond_Once);
	if (ImGui::TreeNode("General")) {
		ImGui::Checkbox("Use FPV", &m_camera.m_useFirstPersonView);
		ImGui::Checkbox("Wireframe", &m_drawAsWire);
		if (ImGui::Checkbox("MSAA ON", &m_useMSAA)) {
			CreateBuffers( );
		}

		ImGui::TreePop( );
	}

	ImGui::SetNextItemOpen(true, ImGuiCond_Once);
	if (ImGui::TreeNode("Skybox")) {
		ImGui::SliderFloat("Strength", &m_globalConstsCPU.strengthIBL, 0.0f,
						  5.0f);
		ImGui::RadioButton("Env", &m_globalConstsCPU.textureToDraw, 0);
		ImGui::SameLine( );
		ImGui::RadioButton("Specular", &m_globalConstsCPU.textureToDraw, 1);
		ImGui::SameLine( );
		ImGui::RadioButton("Irradiance", &m_globalConstsCPU.textureToDraw, 2);
		ImGui::SliderFloat("EnvLodBias", &m_globalConstsCPU.envLodBias, 0.0f,
						   10.0f);

		ImGui::TreePop( );
	}

	ImGui::SetNextItemOpen(true, ImGuiCond_Once);
	if (ImGui::TreeNode("Post Effects")) {
		int flag = 0;
		flag += ImGui::RadioButton("Render", &m_postEffectsConstsCPU.mode, 1);
		ImGui::SameLine( );
		flag += ImGui::RadioButton("Depth", &m_postEffectsConstsCPU.mode, 2);
		flag += ImGui::SliderFloat(
			"DepthScale", &m_postEffectsConstsCPU.depthScale, 0.0, 1.0);
		flag += ImGui::SliderFloat("Fog", &m_postEffectsConstsCPU.fogStrength,
								   0.0, 10.0);

		if (flag)
			D3D11Utils::UpdateBuffer(m_device, m_context,
									 m_postEffectsConstsCPU,
									 m_postEffectsConstsGPU);

		ImGui::TreePop( );
	}

	ImGui::SetNextItemOpen(false, ImGuiCond_Once);
	if (ImGui::TreeNode("Post Processing")) {
		int flag = 0;

		flag += ImGui::SliderFloat("Bloom Strength", &m_postProcess.m_combineFilter.m_constData.strength, 0.0f, 1.0f);
		flag += ImGui::SliderFloat("Exposure", &m_postProcess.m_combineFilter.m_constData.option1, 0.0f, 10.0f);
		flag += ImGui::SliderFloat("Gamma", &m_postProcess.m_combineFilter.m_constData.option2, 0.1f, 5.0f);

		// 편의상 사용자 입력이 인식되면 바로 GPU 버퍼를 업데이트
		if (flag) {
			m_postProcess.m_combineFilter.UpdateConstantBuffers(m_device, m_context);
		}

		ImGui::TreePop( );
	}

	ImGui::SetNextItemOpen(true, ImGuiCond_Once);
	if (ImGui::TreeNode("Mirror")) {
		ImGui::SliderFloat("Alpha", &m_mirrorAlpha, 0.0f, 1.0f);
		const float blendColor[ 4 ] = { m_mirrorAlpha, m_mirrorAlpha,
									 m_mirrorAlpha, 1.0f };
		if (m_drawAsWire)
			Graphics::mirrorBlendWirePSO.SetBlendFactor(blendColor);
		else
			Graphics::mirrorBlendSolidPSO.SetBlendFactor(blendColor);

		ImGui::SliderFloat("Metallic", &m_mirror->m_materialConstsCPU.metallicFactor, 0.0f, 1.0f);
		ImGui::SliderFloat("Roughness", &m_mirror->m_materialConstsCPU.roughnessFactor, 0.0f, 1.0f);

		ImGui::TreePop( );
	}

	ImGui::SetNextItemOpen(true, ImGuiCond_Once);
	if (ImGui::TreeNode("Light")) { 
		//ImGui::SliderFloat3("Position", &m_globalConstsCPU.lights[0].position.x, -5.0f, 5.0f);
		ImGui::SliderFloat("Halo Radius", &m_globalConstsCPU.lights[1].haloRadius, 0.0f, 2.0f);
		ImGui::SliderFloat("Halo Strength", &m_globalConstsCPU.lights[1].haloStrength, 0.0f, 1.0f);
		ImGui::SliderFloat("Radius", &m_globalConstsCPU.lights[1].radius, 0.0f, 0.5f);
		ImGui::TreePop( );
	}

	// $mainObj
	ImGui::SetNextItemOpen(true, ImGuiCond_Once);
	if (ImGui::TreeNode("Material")) {
		ImGui::SliderFloat("LodBias", &m_globalConstsCPU.lodBias, 0.0f, 10.0f);

		int flag = 0;

		flag += ImGui::SliderFloat(
			"Metallic", &m_mainObj->m_materialConstsCPU.metallicFactor, 0.0f, 1.0f);
		flag += ImGui::SliderFloat(
			"Roughness", &m_mainObj->m_materialConstsCPU.roughnessFactor, 0.0f, 1.0f);
		flag += ImGui::CheckboxFlags(
			"AlbedoTexture", &m_mainObj->m_materialConstsCPU.useAlbedoMap, 1);
		flag += ImGui::CheckboxFlags(
			"EmissiveTexture", &m_mainObj->m_materialConstsCPU.useEmissiveMap, 1);
		flag += ImGui::CheckboxFlags(
			"Use NormalMapping", &m_mainObj->m_materialConstsCPU.useNormalMap, 1);
		flag += ImGui::CheckboxFlags(
			"Use AO", &m_mainObj->m_materialConstsCPU.useAOMap, 1);
		flag += ImGui::CheckboxFlags(
			"Use HeightMapping", &m_mainObj->m_meshConstsCPU.useHeightMap, 1);
		flag += ImGui::SliderFloat(
			"HeightScale", &m_mainObj->m_meshConstsCPU.heightScale, 0.0f, 0.1f);
		flag += ImGui::CheckboxFlags(
			"Use MetallicMap", &m_mainObj->m_materialConstsCPU.useMetallicMap, 1);
		flag += ImGui::CheckboxFlags(
			"Use RoughnessMap", &m_mainObj->m_materialConstsCPU.useRoughnessMap, 1);

		if (flag) {
			m_mainObj->UpdateConstantBuffers(m_device, m_context);
		}

		ImGui::Checkbox("Draw Normals", &m_mainObj->m_drawNormals);

		ImGui::SetNextItemOpen(false, ImGuiCond_Once);
		if (ImGui::TreeNode("Rim Effect")) {
			ImGui::SliderFloat("Rim Strength",
						   &m_mainObj->m_materialConstsCPU.rimStrength, 0.0f, 10.0f);
			ImGui::Checkbox("Use Smoothstep",
							&m_mainObj->m_materialConstsCPU.useSmoothstep);
			ImGui::SliderFloat3("Rim Color", &m_mainObj->m_materialConstsCPU.rimColor.x, 0.0f, 1.0f);
			ImGui::SliderFloat("Rim Power", &m_mainObj->m_materialConstsCPU.rimPower, 0.01f, 10.0f);
			ImGui::TreePop( );
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

}

} // namespace kusk