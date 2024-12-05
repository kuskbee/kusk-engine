#include "KuskApp.h"

#include <DirectXCollision.h> // 구와 광선 충돌 계산에 사용
#include <directxtk/DDSTextureLoader.h>
#include <vector>

// file browser
#include <windows.h>
#include <commdlg.h>
#include <filesystem>

#include "textureResources.h"
#include "GeometryGenerator.h"
#include "GraphicsCommon.h"

namespace kusk {

#define SPHERE_PARAMS "Sphere Parameters"
#define SQUARE_PARAMS "Square Parameters"
#define SQUARE_GRID_PARAMS "Square Grid Parameters"
#define CYLINDER_PARAMS "Cylinder Parameters"
#define BOX_PARAMS "Box Parameters"
#define MODELING_FILE_PARAMS "Modeling File Parameters"

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
	m_tessellatedQuad.m_specularResView = m_cubeMapping.m_specularResView;*/

	// Shadertoy Media Files
	// https://shadertoyunofficial.wordpress.com/2019/07/23/shadertoy-media-files/

	AppBase::InitCubemaps(OUTDOOR_ORGN_DDS, OUTDOOR_SPEC_DDS, OUTDOOR_DIFF_DDS, OUTDOOR_BRDF_DDS);

	// 후처리용 화면 사각형
	{
		MeshData meshData = GeometryGenerator::MakeSquare( );
		m_screenSquare = make_shared<Model>(m_device, m_context, vector{ meshData });
		m_screenSquare->m_isPickable = false;
	}

	// 환경 박스 초기화
	{
		MeshData skyboxMesh = GeometryGenerator::MakeBox(40.0f);
		std::reverse(skyboxMesh.indices.begin( ), skyboxMesh.indices.end( ));
		m_skybox = make_shared<Model>(m_device, m_context, vector{ skyboxMesh });
		m_skybox->m_isPickable = false;
	}

	// 바닥(거울)
	if(false)
	{
		auto mesh = GeometryGenerator::MakeSquare(5.0f);
		//mesh.albedoTextureFilename = BLENDER_UV_GRID_2K_TEXTURE;
		m_ground = make_shared<Model>(m_device, m_context, vector{ mesh });
		m_ground->m_isPickable = false;
		m_ground->m_materialConstsCPU.albedoFactor = Vector3(0.1f);
		m_ground->m_materialConstsCPU.emissionFactor = Vector3(0.0f);
		m_ground->m_materialConstsCPU.metallicFactor = 0.5f;
		m_ground->m_materialConstsCPU.roughnessFactor = 0.3f;

		//
		m_ground->m_modelCreationParams.type = MESH_TYPE_SQUARE;
		m_ground->m_modelCreationParams.scale = 5.0f;
		m_ground->m_modelCreationParams.tex_scale = Vector2(1.0f);

		Vector3 position = Vector3(0.0f, -0.5f, 2.0f);
		m_ground->UpdateWorldRow(
			Matrix::CreateRotationX(3.141592f * 0.5f) *
			Matrix::CreateTranslation(position));

		m_mirrorPlane = SimpleMath::Plane(position, Vector3(0.0f, 1.0f, 0.0f));
		m_mirror = m_ground; // 바닥에 거울처럼 반사 구현
		// m_basicList.push_back(m_ground); // 거울은 리스트에 등록 X
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
			m_lightSphere[ i ]->m_isPickable = false; // 피킹 X

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
		m_cursorSphere->m_isPickable = false; // 피킹 X
		m_cursorSphere->m_materialConstsCPU.albedoFactor = Vector3(0.0f);
		m_cursorSphere->m_materialConstsCPU.emissionFactor = Vector3(0.0f, 1.0f, 1.0f);
		
		m_basicList.push_back(m_cursorSphere); // 리스트에 등록
	}

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

void KuskApp::UpdateMousePicking( ) {
	// ImGui가 마우스 입력을 처리 중이면 Picking 건너뜀
	if (ImGui::GetIO( ).WantCaptureMouse) {
		return; 
	}

	if (m_leftButton || m_rightButton) {

		Quaternion finalQtnion;
		Vector3 finalDragTrsl;
		Vector3 finalPickPoint;
		float minDistance = D3D11_FLOAT32_MAX;
		int minModelIndex = -1;

		for (size_t i = 0; i < m_basicList.size( ); i++) {
			if (!m_basicList[ i ]->m_isPickable) continue;

			Quaternion q;
			Vector3 dragTranslation;
			Vector3 pickPoint;
			float distance = 0.0f;
			if (UpdateMouseControl(m_basicList[ i ]->m_boundingSphere, 
								   q, dragTranslation, pickPoint, distance,
								   m_basicList[ i ]->m_mouseState)) {
				if (minDistance > distance) {
					minDistance = distance;
					finalQtnion = q;
					finalDragTrsl = dragTranslation;
					finalPickPoint = pickPoint;
					minModelIndex = int(i);
				}
			}
		}

		if (minModelIndex >= 0) {
			auto& pickedModel = m_basicList[ minModelIndex ];
			if (false == pickedModel->m_isFixed) {
				Vector3 translation = pickedModel->m_worldRow.Translation( );
				pickedModel->m_worldRow.Translation(Vector3(0.0f));
				pickedModel->UpdateWorldRow(
					pickedModel->m_worldRow * Matrix::CreateFromQuaternion(finalQtnion) *
					Matrix::CreateTranslation(finalDragTrsl + translation));
			}
			
			// 충돌 지점에 작은 구 그리기
			m_cursorSphere->m_isVisible = true;
			m_cursorSphere->UpdateWorldRow(Matrix::CreateTranslation(finalPickPoint));

			if (m_selectedModelIndex >= 0 && m_selectedModelIndex != minModelIndex) {
				m_basicList[ m_selectedModelIndex ]->m_materialConstsCPU.isSelected = false;
			}
			m_selectedModelIndex = minModelIndex;
			m_basicList[ m_selectedModelIndex ]->m_materialConstsCPU.isSelected = true;
		}
		else {
			m_cursorSphere->m_isVisible = false;
			if (m_selectedModelIndex >= 0) {
				m_basicList[ m_selectedModelIndex ]->m_materialConstsCPU.isSelected = false;
				m_selectedModelIndex = -1;
			}
		}
	}
	else {
		for (size_t i = 0; i < m_basicList.size( ); i++) {
			m_basicList[ i ]->m_mouseState.isDragging = false;
		}

		m_cursorSphere->m_isVisible = false;
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
	if(m_mirror)
		m_mirror->UpdateConstantBuffers(m_device, m_context);

	// 조명의 위치 반영
	for (int i = 0; i < MAX_LIGHTS; i++) {
		m_lightSphere[ i ]->UpdateWorldRow(
			Matrix::CreateScale(std::max(0.01f, m_globalConstsCPU.lights[i].radius)) * 
			Matrix::CreateTranslation(m_globalConstsCPU.lights[ i ].position));
	}

	// 마우스 이동/회전 반영, 물체 선택 처리
	UpdateMousePicking( );

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

	/*for (int i = 0; i < m_billboardPointsList.size( ); i++) {
		m_billboardPointsList[i]->m_constantData.eyeWorld = eyeWorld;
		m_billboardPointsList[i]->m_constantData.view = viewRow.Transpose( );
		m_billboardPointsList[i]->m_constantData.proj = projRow.Transpose( );
		D3D11Utils::UpdateBuffer(m_device, m_context, 
			m_billboardPointsList[ i ]->m_constantData, 
			m_billboardPointsList[ i ]->m_constantBuffer);
	}*/
	
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
	if (m_mirror) {
		m_mirror->Render(m_context);
	}

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
			if (m_mirror) {
				m_mirror->Render(m_context);
			}
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
	if (m_mirror && m_mirrorAlpha == 1.0f) {
		m_mirror->Render(m_context);
	}

	AppBase::SetPipelineState(m_drawAsWire ? Graphics::billboardPointsWirePSO
										   : Graphics::billboardPointsSolidPSO);
	for(auto& b : m_billboardPointsList)
		b->Render(m_context);

	AppBase::SetPipelineState(Graphics::normalsPSO);
	for (auto& i : m_basicList) {
		if (i->m_drawNormals) {
			i->RenderNormals(m_context);
		}
	}

	AppBase::SetPipelineState(m_drawAsWire ? Graphics::skyboxWirePSO
										   : Graphics::skyboxSolidPSO);
	m_skybox->Render(m_context);

	//m_tessellatedQuad.Render(m_context);

	if (m_mirror && m_mirrorAlpha < 1.0f) { // 거울을 그려야하는 상황

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

		AppBase::SetPipelineState(m_drawAsWire ? Graphics::reflectBillboardPointsWirePSO
											   : Graphics::reflectBillboardPointsSolidPSO);
		for (auto& b : m_billboardPointsList)
			b->Render(m_context); //

		AppBase::SetPipelineState(m_drawAsWire ? Graphics::reflectSkyboxWirePSO
											   : Graphics::reflectSkyboxSolidPSO);

		m_skybox->Render(m_context);


		/* 거울 4. 거울 자체의 재질을 "Blend"로 그림 */
		AppBase::SetPipelineState(m_drawAsWire ? Graphics::mirrorBlendWirePSO
											   : Graphics::mirrorBlendSolidPSO);
		AppBase::SetGlobalConsts(m_globalConstsGPU);

		m_mirror->Render(m_context);

	} // end of if (m_mirror && m_mirrorAlpha < 1.0f)

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

Vector3 ExtractScale(const Matrix& matrix) {
	Vector3 scaleX(matrix._11, matrix._12, matrix._13);
	Vector3 scaleY(matrix._21, matrix._22, matrix._23);
	Vector3 scaleZ(matrix._31, matrix._32, matrix._33);

	// 축 벡터의 길이를 스케일로 반환
	return Vector3(scaleX.Length(), scaleY.Length(), scaleZ.Length());
}

Matrix RemoveScaleMatrix(const Matrix& matrix) {
	Vector3 scaleX(matrix._11, matrix._12, matrix._13);
	Vector3 scaleY(matrix._21, matrix._22, matrix._23);
	Vector3 scaleZ(matrix._31, matrix._32, matrix._33);

	// 각 축 벡터를 정규화하여 스케일 제거
	scaleX.Normalize( );
	scaleY.Normalize( );
	scaleZ.Normalize( );

	// 정규화된 회전 행렬 구성
	Matrix rotationMatrix = Matrix::Identity;
	rotationMatrix._11 = scaleX.x; rotationMatrix._12 = scaleX.y; rotationMatrix._13 = scaleX.z;
	rotationMatrix._21 = scaleY.x; rotationMatrix._22 = scaleY.y; rotationMatrix._23 = scaleY.z;
	rotationMatrix._31 = scaleZ.x; rotationMatrix._32 = scaleZ.y; rotationMatrix._33 = scaleZ.z;

	// 이동 성분 복사
	rotationMatrix._41 = matrix._41;
	rotationMatrix._42 = matrix._42;
	rotationMatrix._43 = matrix._43;

	return rotationMatrix;
}


void KuskApp::UpdateGUI() {

	// JSON LOAD / SAVE
	ImGui::SetNextItemOpen(true, ImGuiCond_Once);
	if (ImGui::TreeNode("Scene Json DATA")) {
		static const std::string filterName = "Json data files";
		static const std::string filterExts = "*.json";
		static const std::string defualtExts = "json";

		if (ImGui::Button("LOAD JSON")) {
			std::string filePath = OpenFileDialog(filterName, filterExts);
			if (!filePath.empty( )) {
				LoadSceneDataFromJSON(filePath);
			}
		}
		ImGui::SameLine( );
		if (ImGui::Button("SAVE JSON")) {
			std::string filePath = SaveFileDialog(filterName, filterExts, defualtExts);
			if (!filePath.empty( )) {
				SaveSceneDataAsJSON(filePath);
			}
		}

		ImGui::TreePop( );
	}

	ImGui::SetNextItemOpen(false, ImGuiCond_Once);
	if (ImGui::TreeNode("General")) {
		ImGui::Checkbox("Use FPV", &m_camera.m_useFirstPersonView);
		ImGui::Checkbox("Wireframe", &m_drawAsWire);
		if (ImGui::Checkbox("MSAA ON", &m_useMSAA)) {
			CreateBuffers( );
		}
		ImGui::SliderFloat("LodBias", &m_globalConstsCPU.lodBias, 0.0f, 10.0f);

		ImGui::TreePop( );
	}

	ImGui::SetNextItemOpen(false, ImGuiCond_Once);
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

		static const std::string filterName = "DDS files";
		static const std::string filterExts = "*.dds;";

		// $envFile
		if (!m_cubemapTextureEnvFilePath.empty( ))
			ImGui::Text("env texture: %s", m_cubemapTextureEnvFilePath.c_str( ));
		else
			ImGui::Text("env texture: NONE");

		if (ImGui::Button("Register Env Texture")) {
			std::string filePath = OpenFileDialog(filterName, filterExts);
			if (!filePath.empty( ))
				m_cubemapTextureEnvFilePath = filePath;
		}

		// $specularFile
		if (!m_cubemapTextureSpecularFilePath.empty( ))
			ImGui::Text("specular texture: %s", m_cubemapTextureSpecularFilePath.c_str( ));
		else
			ImGui::Text("specular texture: NONE");

		if (ImGui::Button("Register Specular Texture")) {
			std::string filePath = OpenFileDialog(filterName, filterExts);
			if (!filePath.empty( ))
				m_cubemapTextureSpecularFilePath = filePath;
		}

		// $irradianceFile
		if (!m_cubemapTextureIrradianceFilePath.empty( ))
			ImGui::Text("irradiance texture: %s", m_cubemapTextureIrradianceFilePath.c_str( ));
		else
			ImGui::Text("irradiance texture: NONE");

		if (ImGui::Button("Register Irradiance Texture")) {
			std::string filePath = OpenFileDialog(filterName, filterExts);
			if (!filePath.empty( ))
				m_cubemapTextureIrradianceFilePath = filePath;
		}

		// $BrdfFile
		if (!m_cubemapTextureBrdfFilePath.empty( ))
			ImGui::Text("brdf texture: %s", m_cubemapTextureBrdfFilePath.c_str( ));
		else
			ImGui::Text("brdf texture: NONE");

		if (ImGui::Button("Register BRDF Texture")) {
			std::string filePath = OpenFileDialog(filterName, filterExts);
			if (!filePath.empty( ))
				m_cubemapTextureBrdfFilePath = filePath;
		}

		// $confirm
		if (ImGui::Button("Apply Registered Textures to Cubemaps")) {
			AppBase::InitCubemaps(
				JsonManager::UTF8ToWString(m_cubemapTextureEnvFilePath), 
				JsonManager::UTF8ToWString(m_cubemapTextureSpecularFilePath), 
				JsonManager::UTF8ToWString(m_cubemapTextureIrradianceFilePath), 
				JsonManager::UTF8ToWString(m_cubemapTextureBrdfFilePath));
		}

		ImGui::TreePop( );
	}

	ImGui::SetNextItemOpen(false, ImGuiCond_Once);
	if (ImGui::TreeNode("Billboard")) {
		if (ImGui::Button("Billboard Edit")) {
			m_showBillboardEditPopup = true;
		}

		ImGui::TreePop( );
	}

	ImGui::SetNextItemOpen(false, ImGuiCond_Once);
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

	// 거울 관련
	if (m_mirror) {
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

			int flag = 0;
			Vector3 position = m_mirror->m_worldRow.Translation( );
			flag += ImGui::SliderFloat3("position", &position.x, -10.0f, 10.0f);
			flag += ImGui::SliderFloat3("rotation", &m_mirrorRotation.x, -3.14f, 3.14f);
			flag += ImGui::SliderFloat3("scale", &m_mirrorScale.x, 0.01f, 50.0f);

			if (flag) {
				m_mirror->UpdateWorldRow(
					Matrix::CreateScale(Vector3(m_mirrorScale))*
					Matrix::CreateRotationX(m_mirrorRotation.x)*
					Matrix::CreateRotationY(m_mirrorRotation.y)*
					Matrix::CreateRotationZ(m_mirrorRotation.z)*
					Matrix::CreateTranslation(position));

				// Plane 갱신
				Vector3 normal = Vector3(0.0f, 0.0f, -1.0f);
				normal = Vector3::TransformNormal(normal, m_mirror->m_worldITRow);
				normal.Normalize( );
				Vector3 worldPosition = Vector3::Transform(Vector3(0.0f, 0.0f, 0.0f), m_mirror->m_worldRow);
				float d = -normal.Dot(worldPosition); // -(Ax+By+Cz) = D
				m_mirrorPlane = DirectX::SimpleMath::Plane(normal.x, normal.y, normal.z, d);

			}

			// 삭제 버튼
			if (ImGui::Button("Delete")) {
				m_mirror.reset( );
			}

			ImGui::TreePop( );
		}
	}
	
	ImGui::SetNextItemOpen(false, ImGuiCond_Once);
	if (ImGui::TreeNode("Light")) { 

		for (int i = 0; i < MAX_LIGHTS; i++) {
			std::string light_tab = std::to_string(i) + " Light";
			if (ImGui::TreeNode(light_tab.c_str())) {
				bool isLightOn = m_globalConstsCPU.lights[ i ].type == LIGHT_OFF ? false : true;
				int type = m_globalConstsCPU.lights[ i ].type & ~LIGHT_SHADOW;
				if (ImGui::Checkbox("Light ON/OFF", &isLightOn)) {
					m_lightSphere[ i ]->m_isVisible = isLightOn;
				}
				if (!isLightOn)
					type = LIGHT_OFF;
				
				if (isLightOn) {
					if (type == LIGHT_OFF) type = LIGHT_SPOT; // default
					ImGui::RadioButton("Directional Light", &type, LIGHT_DIRECTIONAL);
					ImGui::SameLine( );
					ImGui::RadioButton("Point Light", &type, LIGHT_POINT);
					ImGui::SameLine( );
					ImGui::RadioButton("Spot Light", &type, LIGHT_SPOT);

					bool isShadow = m_globalConstsCPU.lights[ i ].type & LIGHT_SHADOW;
					ImGui::Checkbox("Shadow ON/OFF", &isShadow);
					if (isShadow)
						type |= LIGHT_SHADOW;

					if (i != 1) {
						ImGui::SliderFloat3("Position", &m_globalConstsCPU.lights[ i ].position.x, -5.0f, 5.0f);
					}
					ImGui::SliderFloat3("Radiance", &m_globalConstsCPU.lights[ i ].radiance.x, 0.0f, 5.0f);
					ImGui::SliderFloat("Radius", &m_globalConstsCPU.lights[ i ].radius, 0.0f, 0.5f);
					ImGui::SliderFloat("Spot Power", &m_globalConstsCPU.lights[ i ].spotPower, 0.0f, 6.0f);
					ImGui::SliderFloat("Fall Off Start", &m_globalConstsCPU.lights[ i ].fallOffStart, 0.0f, 20.0f);
					ImGui::SliderFloat("Fall Off End", &m_globalConstsCPU.lights[ i ].fallOffEnd, 0.0f, 50.0f);
					ImGui::Separator( );
					ImGui::SliderFloat("Halo Radius", &m_globalConstsCPU.lights[ i ].haloRadius, 0.0f, 2.0f);
					ImGui::SliderFloat("Halo Strength", &m_globalConstsCPU.lights[ i ].haloStrength, 0.0f, 1.0f);
					
				}

				m_globalConstsCPU.lights[ i ].type = type;

				ImGui::TreePop( );
			}
		}
		ImGui::TreePop( );
	}

	if (m_selectedModelIndex >= 0) {
		auto& selectedObj = m_basicList[ m_selectedModelIndex ];
		ImGui::SetNextItemOpen(true, ImGuiCond_Once);
		if (ImGui::TreeNode("Selected Object's Material")) {
			ImGui::Checkbox("isFixed", &selectedObj->m_isFixed);

			int flag = 0;
			Vector3 position = selectedObj->m_worldRow.Translation( );
			flag += ImGui::SliderFloat3("position", &position.x, -10.0f, 10.0f);

			Vector3 scale = ExtractScale(selectedObj->m_worldRow);
			flag += ImGui::SliderFloat3("scale", &scale.x, 0.01f, 10.0f);
			
			Vector3 rotation = Vector3(0.0f);
			if (ImGui::Button("X Axis +45")) {
				flag++; rotation.x = XM_PIDIV4;
			}
			ImGui::SameLine( );
			if (ImGui::Button("X Axis -45")) {
				flag++; rotation.x = -XM_PIDIV4;
			}
			if (ImGui::Button("Y Axis +45")) {
				flag++; rotation.y = XM_PIDIV4;
			}
			ImGui::SameLine( );
			if (ImGui::Button("Y Axis -45")) {
				flag++; rotation.y = -XM_PIDIV4;
			}
			if (ImGui::Button("Z Axis +45")) {
				flag++; rotation.z = XM_PIDIV4;
			}
			ImGui::SameLine( );
			if (ImGui::Button("Z Axis -45")) {
				flag++; rotation.z = -XM_PIDIV4;
			}
			if (flag) {
				selectedObj->m_worldRow.Translation(Vector3(0.0f));
				Matrix normalized = RemoveScaleMatrix(selectedObj->m_worldRow);
				selectedObj->UpdateWorldRow(Matrix::CreateScale(scale) * 
					normalized *
					Matrix::CreateRotationY(rotation.y)*
					Matrix::CreateRotationX(rotation.x)*
					Matrix::CreateRotationZ(rotation.z)*
					Matrix::CreateTranslation(position));
			}

			flag = 0;
			flag += ImGui::SliderFloat3(
				"Albedo", &selectedObj->m_materialConstsCPU.albedoFactor.x, 0.0f, 1.0f);
			flag += ImGui::SliderFloat(
				"Metallic", &selectedObj->m_materialConstsCPU.metallicFactor, 0.0f, 1.0f);
			flag += ImGui::SliderFloat(
				"Roughness", &selectedObj->m_materialConstsCPU.roughnessFactor, 0.0f, 1.0f);
			flag += ImGui::CheckboxFlags(
				"AlbedoTexture", &selectedObj->m_materialConstsCPU.useAlbedoMap, 1);
			flag += ImGui::CheckboxFlags(
				"EmissiveTexture", &selectedObj->m_materialConstsCPU.useEmissiveMap, 1);
			flag += ImGui::CheckboxFlags(
				"Use NormalMapping", &selectedObj->m_materialConstsCPU.useNormalMap, 1);
			flag += ImGui::CheckboxFlags(
				"Use AO", &selectedObj->m_materialConstsCPU.useAOMap, 1);
			flag += ImGui::CheckboxFlags(
				"Use HeightMapping", &selectedObj->m_meshConstsCPU.useHeightMap, 1);
			flag += ImGui::SliderFloat(
				"HeightScale", &selectedObj->m_meshConstsCPU.heightScale, 0.0f, 0.1f);
			flag += ImGui::CheckboxFlags(
				"Use MetallicMap", &selectedObj->m_materialConstsCPU.useMetallicMap, 1);
			flag += ImGui::CheckboxFlags(
				"Use RoughnessMap", &selectedObj->m_materialConstsCPU.useRoughnessMap, 1);

			if (flag) {
				selectedObj->UpdateConstantBuffers(m_device, m_context);
			}

			ImGui::Checkbox("Draw Normals", &selectedObj->m_drawNormals);

			if (ImGui::Button("Delete this object")) {
				int idx = m_selectedModelIndex;
				m_selectedModelIndex = -1;

				auto target = m_basicList[ idx ];
				m_basicList.erase(m_basicList.begin( ) + idx);
				// m_savedList에서 동일한 객체 삭제
				m_savedList.erase(
					std::remove(m_savedList.begin( ), m_savedList.end( ), target),
					m_savedList.end( )
				);
			}

			if (ImGui::Button("Copy this object")) {
				int idx = m_selectedModelIndex;
				auto target = m_basicList[ idx ];
				auto copied = make_shared<Model>(m_device, m_context, target);
				m_basicList.push_back(copied);
				m_savedList.push_back(copied);
			}

			ImGui::TreePop( );
		}

		// 메쉬가 하나일 때만 texture 수정할 수 있도록 표시
		if (selectedObj->m_meshes.size( ) == 1) {
			static const std::string filterName = "Image files";
			static const std::string filterExts = "*.jpg;*.jpeg;*.png";

			ImGui::SetNextItemOpen(true, ImGuiCond_Once);
			if (ImGui::TreeNode("Selected Object's Textures")) {
				auto& mesh = selectedObj->m_meshes[ 0 ];

				// $albedo
				if (!selectedObj->m_albedoTextureFilePath.empty( )) {
					ImGui::Text("albedo texture: %s", selectedObj->m_albedoTextureFilePath.c_str( ));
				}
				else {
					ImGui::Text("albedo texture: NONE");
				}

				if (ImGui::Button("Change Albedo Texture")) {
					std::string filePath = OpenFileDialog(filterName, filterExts);
					if (!filePath.empty( )) {
						selectedObj->m_albedoTextureFilePath = filePath;
						D3D11Utils::CreateTexture(m_device, m_context, filePath, true, mesh->albedoTexture, mesh->albedoSRV);
						selectedObj->m_materialConstsCPU.useAlbedoMap = true;
					}
				}

				// $emissive
				if (!selectedObj->m_emissiveTextureFilePath.empty( )) {
					ImGui::Text("emissive texture: %s", selectedObj->m_emissiveTextureFilePath.c_str( ));
				}
				else {
					ImGui::Text("emissive texture: NONE");
				}

				if (ImGui::Button("Change Emissive Texture")) {
					std::string filePath = OpenFileDialog(filterName, filterExts);
					if (!filePath.empty( )) {
						selectedObj->m_emissiveTextureFilePath = filePath;
						D3D11Utils::CreateTexture(m_device, m_context, filePath, true, mesh->emissiveTexture, mesh->emissiveSRV);
						selectedObj->m_materialConstsCPU.useEmissiveMap = true;
					}
				}

				// $normal
				if (!selectedObj->m_normalTextureFilePath.empty( )) {
					ImGui::Text("normal texture: %s", selectedObj->m_normalTextureFilePath.c_str( ));
				}
				else {
					ImGui::Text("normal texture: NONE");
				}

				if (ImGui::Button("Change Normal Texture")) {
					std::string filePath = OpenFileDialog(filterName, filterExts);
					if (!filePath.empty( )) {
						selectedObj->m_normalTextureFilePath = filePath;
						D3D11Utils::CreateTexture(m_device, m_context, filePath, true, mesh->normalTexture, mesh->normalSRV);
						selectedObj->m_materialConstsCPU.useNormalMap = true;
					}
				}

				// $height
				if (!selectedObj->m_heightTextureFilePath.empty( )) {
					ImGui::Text("height texture: %s", selectedObj->m_heightTextureFilePath.c_str( ));
				}
				else {
					ImGui::Text("height texture: NONE");
				}

				if (ImGui::Button("Change Height Texture")) {
					std::string filePath = OpenFileDialog(filterName, filterExts);
					if (!filePath.empty( )) {
						selectedObj->m_heightTextureFilePath = filePath;
						D3D11Utils::CreateTexture(m_device, m_context, filePath, true, mesh->heightTexture, mesh->heightSRV);
						selectedObj->m_meshConstsCPU.useHeightMap = true;
					}
				}

				// $ambient occlusion
				if (!selectedObj->m_aoTextureFilePath.empty( )) {
					ImGui::Text("AO texture: %s", selectedObj->m_aoTextureFilePath.c_str( ));
				}
				else {
					ImGui::Text("AO texture: NONE");
				}

				if (ImGui::Button("Change AO Texture")) {
					std::string filePath = OpenFileDialog(filterName, filterExts);
					if (!filePath.empty( )) {
						selectedObj->m_aoTextureFilePath = filePath;
						D3D11Utils::CreateTexture(m_device, m_context, filePath, true, mesh->aoTexture, mesh->aoSRV);
						selectedObj->m_materialConstsCPU.useAOMap = true;
					}
				}

				// $metallic
				if (!selectedObj->m_metallicTextureFilePath.empty( )) {
					ImGui::Text("metallic texture: %s", selectedObj->m_metallicTextureFilePath.c_str( ));
				}
				else {
					ImGui::Text("metallic texture: NONE");
				}

				if (ImGui::Button("Change Metallic Texture")) {
					std::string filePath = OpenFileDialog(filterName, filterExts);
					if (!filePath.empty( )) {
						selectedObj->m_metallicTextureFilePath = filePath;
						D3D11Utils::CreateMetallicRoughnessTexture(m_device, m_context, filePath, selectedObj->m_roughnessTextureFilePath,
																	mesh->metallicRoughnessTexture,
																	mesh->metallicRoughnessSRV);
						selectedObj->m_materialConstsCPU.useMetallicMap = true;
					}
				}

				// $roughness
				if (!selectedObj->m_roughnessTextureFilePath.empty( )) {
					ImGui::Text("roughness texture: %s", selectedObj->m_roughnessTextureFilePath.c_str( ));
				}
				else {
					ImGui::Text("roughness texture: NONE");
				}

				if (ImGui::Button("Change Roughness Texture")) {
					std::string filePath = OpenFileDialog(filterName, filterExts);
					if (!filePath.empty( )) {
						selectedObj->m_roughnessTextureFilePath = filePath;
						D3D11Utils::CreateMetallicRoughnessTexture(m_device, m_context, selectedObj->m_metallicTextureFilePath, filePath,
																	mesh->metallicRoughnessTexture,
																	mesh->metallicRoughnessSRV);
						selectedObj->m_materialConstsCPU.useRoughnessMap = true;
					}
				}

				ImGui::TreePop( );
			}
		}
	}

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

void KuskApp::UpdateObjectCreationFrameGUI( ) {

	ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO( ).DisplaySize.x - 10, 10), ImGuiCond_Always, ImVec2(1.0f, 0.0f));

	ImGui::Begin("Object Creation Menu");
	if (ImGui::Button("Make Sphere")) {
		m_currentPopup = SPHERE_PARAMS;
		ImGui::OpenPopup(m_currentPopup.c_str( ));
	}
	if (ImGui::Button("Make Square")) {
		m_currentPopup = SQUARE_PARAMS;
		ImGui::OpenPopup(m_currentPopup.c_str( ));
	}
	if (ImGui::Button("Make Square Grid")) {
		m_currentPopup = SQUARE_GRID_PARAMS;
		ImGui::OpenPopup(m_currentPopup.c_str( ));
	}
	if (ImGui::Button("Make Cylinder")) {
		m_currentPopup = CYLINDER_PARAMS;
		ImGui::OpenPopup(m_currentPopup.c_str( ));
	}
	if (ImGui::Button("Make Box")) {
		m_currentPopup = BOX_PARAMS;
		ImGui::OpenPopup(m_currentPopup.c_str( ));
	}
	if (ImGui::Button("Make Model from a modeling file")) {
		m_currentPopup = MODELING_FILE_PARAMS;
		ImGui::OpenPopup(m_currentPopup.c_str( ));
	}

	if (m_currentPopup == SPHERE_PARAMS) {
		ShowPopup(SPHERE_PARAMS, [&]( ) {
			ImGui::SliderFloat("Radius", &m_modelParams.radius, 0.1f, 1.0f, "%.2f");
			ImGui::SliderInt("Num Slices", &m_modelParams.numSlices, 2, 200);
			ImGui::SliderInt("Num Stacks", &m_modelParams.numStacks, 2, 200);
			ImGui::SliderFloat2("Texture Scale", &m_modelParams.tex_scale.x, 1.0f, 10.0f);
		}, 
		[&]( ) {
			m_modelParams.type = MESH_TYPE_SPHERE;
			CreateSphere(m_modelParams.radius, m_modelParams.numSlices, m_modelParams.numStacks, m_modelParams.tex_scale);
		});
	}
	else if (m_currentPopup == SQUARE_PARAMS) {
		ShowPopup(SQUARE_PARAMS, [&]( ) {
			ImGui::SliderFloat("Scale", &m_modelParams.scale, 0.1f, 50.0f, "%.2f");
			ImGui::SliderFloat2("Texture Scale", &m_modelParams.tex_scale.x, 1.0f, 10.0f);
			if (!m_mirror) {
				ImGui::Checkbox("isMirror", &m_modelParams.isMirror);
			}
		},
		[&]( ) {
			m_modelParams.type = MESH_TYPE_SQUARE;
			CreateSquare(m_modelParams.scale, m_modelParams.tex_scale, m_modelParams.isMirror);
			
			m_modelParams.isMirror = false; // 다음 모델 생성을 위해 초기화
		});
	}
	else if (m_currentPopup == SQUARE_GRID_PARAMS) {
		ShowPopup(SQUARE_GRID_PARAMS, [&]( ) {
			ImGui::SliderInt("Num Slices", &m_modelParams.numSlices, 2, 200);
			ImGui::SliderInt("Num Stacks", &m_modelParams.numStacks, 2, 200);
			ImGui::SliderFloat("Scale", &m_modelParams.scale, 0.1f, 1.0f, "%.2f");
			ImGui::SliderFloat2("Texture Scale", &m_modelParams.tex_scale.x, 1.0f, 10.0f);
		},
		[&]( ) {
			m_modelParams.type = MESH_TYPE_SQUARE_GRID;
			CreateSquareGrid(m_modelParams.numSlices, m_modelParams.numStacks,
			m_modelParams.scale, m_modelParams.tex_scale);
		});
	}
	else if (m_currentPopup == CYLINDER_PARAMS) {
		ShowPopup(CYLINDER_PARAMS, [&]( ) {
			ImGui::SliderFloat("Bottom Radius", &m_modelParams.bottomRadius, 0.1f, 2.0f, "%.2f");
			ImGui::SliderFloat("Top Radius", &m_modelParams.topRadius, 0.1f, 2.0f, "%.2f");
			ImGui::SliderFloat("Height", &m_modelParams.height, 0.1f, 2.0f, "%.2f");
			ImGui::SliderInt("Num Slices", &m_modelParams.numSlices, 2, 200);
		},
		[&]( ) {
			m_modelParams.type = MESH_TYPE_CYLINDER;
			CreateCylinder(m_modelParams.bottomRadius, m_modelParams.topRadius,
			m_modelParams.height, m_modelParams.numSlices);
		});
	}
	else if (m_currentPopup == BOX_PARAMS) {
		ShowPopup(BOX_PARAMS, [&]( ) {
			ImGui::SliderFloat("Scale", &m_modelParams.scale, 0.1f, 1.0f, "%.2f");
		},
		[&]( ) {
			m_modelParams.type = MESH_TYPE_BOX;
			CreateBox(m_modelParams.scale);
		});
	}
	else if (m_currentPopup == MODELING_FILE_PARAMS) {
		ShowPopup(MODELING_FILE_PARAMS, [&]( ) {

			if (!m_modelParams.selectedFilePath.empty( )) {
				ImGui::Text("Selected File: %s", m_modelParams.selectedFilePath.c_str( ));
			}

			if (ImGui::Button("Open File Explorer")) {
				std::string filePath = OpenFileDialog("Model Files", "*.fbx;*.gltf");
				if (!filePath.empty( )) {
					m_modelParams.selectedFilePath = filePath; // 선택된 파일 경로 저장
				}
			}
			
			ImGui::BeginDisabled(m_modelParams.selectedFilePath.empty( ));
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.7f, 0.3f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2f, 0.5f, 0.2f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.4f, 0.1f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
			if (ImGui::Button("Confirm")) {
				CreateModelFromFile(m_modelParams.selectedFilePath);
				m_modelParams.selectedFilePath.clear( );

				m_currentPopup.clear( );
				ImGui::CloseCurrentPopup( );
			}
			ImGui::EndDisabled( );
			ImGui::PopStyleColor(4);
			ImGui::SameLine( );

			if (ImGui::Button("Close")) {
				m_currentPopup.clear( );
				ImGui::CloseCurrentPopup( );
			}
			
			
		});
	}

	ImGui::End( );
}

void KuskApp::UpdatePopupGUI( ) {

	// 빌보드 편집 Popup
	if (m_showBillboardEditPopup) {
		ImGui::Begin("Add Billboard", &m_showBillboardEditPopup);
		static const std::string filterName = "Image files";
		static const std::string filterExts = "*.jpg;*.jpeg;*.png";

		if (m_billboardTextureList.size( )) {
			for (size_t i = 0; i < m_billboardTextureList.size( ); ++i) {
				ImGui::PushID(int(i)); // 고유 ID를 푸시하여 버튼 간 충돌 방지

				std::filesystem::path filePath(m_billboardTextureList[ i ]);
				std::string fileName = filePath.filename( ).string( );
				ImGui::Text("File[%d] : %s", i, fileName.c_str( ));
				ImGui::SameLine( );

				// 삭제 버튼
				if (ImGui::Button("Delete")) {
					m_billboardTextureList.erase(m_billboardTextureList.begin( ) + i);
					ImGui::PopID( ); // 삭제되었으므로 ID 스택 정리 후 루프 재진입
					break;
				}

				ImGui::PopID( ); // ID 스택 정리
			}
		}
		else {
			ImGui::Text("The texture file list is empty."
						"\nUnable to create a billboard. "
						"\nPlease register your texture files.");
		}
		if (ImGui::Button("Add Texture")) {
			std::string filePath = OpenFileDialog(filterName, filterExts);
			if (!filePath.empty( )) {
				m_billboardTextureList.push_back(filePath);
			}
		}
		if (m_billboardTextureList.size( )) {
			if (ImGui::Button("Create Billboard")) {
				CreateBillboardPoints( );
			}
		}
		ImVec2 windowPos = ImGui::GetWindowPos( );
		ImVec2 windowSize = ImGui::GetWindowSize( );
		ImGui::End( );

		// 생성된 빌보드 편집 
		ImGui::SetNextWindowPos(ImVec2(windowPos.x, windowPos.y + windowSize.y + 10), ImGuiCond_Always);
		ImGui::Begin("Edit BillboardPoints");
		static int selectedIndex = 0;
		int flag = 0;
		if (!m_billboardPointsList.empty( )) {
			std::vector<std::string> itemStrings; 
			itemStrings.reserve(m_billboardPointsList.size( ));
			for (size_t i = 0; i < m_billboardPointsList.size( ); ++i) {
				itemStrings.push_back(("Billboard " + std::to_string(i)));
			}

			if (ImGui::BeginCombo("Select Billboard", itemStrings[ selectedIndex ].c_str( ))) {
				for (size_t i = 0; i < itemStrings.size( ); ++i) {
					const bool isSelected = (selectedIndex == i);
					if (ImGui::Selectable(itemStrings[ i ].c_str( ), isSelected)) {
						selectedIndex = (int)i;
					}

					if (isSelected) {
						ImGui::SetItemDefaultFocus( ); // 기본 포커스 설정
					}
				}
				ImGui::EndCombo( );
			}

			if (selectedIndex < m_billboardPointsList.size( )) {
				auto selectedBillboard = m_billboardPointsList[ selectedIndex ];
				flag += ImGui::SliderFloat("width", &selectedBillboard->m_constantData.width, 0.1f, 4.0f, "%.2f");
				flag += ImGui::SliderFloat3("albedo", &selectedBillboard->m_constantData.albedoFactor.x, 0.0f, 1.0f, "%.2f");
				flag += ImGui::SliderFloat("roughness", &selectedBillboard->m_constantData.roughnessFactor, 0.0f, 1.0f, "%.2f");
				flag += ImGui::SliderFloat("metallic", &selectedBillboard->m_constantData.metallicFactor, 0.0f, 1.0f, "%.2f");

				ImGui::Text(":Points:");
				for (size_t i = 0; i < selectedBillboard->m_points.size( ); ++i) {
					ImGui::PushID(int(i));
					std::string name = "point " + std::to_string(i);
					Vector3 tempPoint = { selectedBillboard->m_points[ i ].x, 
									selectedBillboard->m_points[ i ].y, 
									selectedBillboard->m_points[ i ].z };
					if (ImGui::SliderFloat3(name.c_str( ), &tempPoint.x, -5.0f, 5.0f, "%.2f")) {
						selectedBillboard->m_points[ i ].x = tempPoint.x;
						selectedBillboard->m_points[ i ].y = tempPoint.y;
						selectedBillboard->m_points[ i ].z = tempPoint.z;
						selectedBillboard->UpdateVertexBuffer(m_device);
					}
					ImGui::SameLine( );

					if (ImGui::Button("Delete")) {
						selectedBillboard->m_points.erase(selectedBillboard->m_points.begin( ) + i);
						selectedBillboard->UpdateVertexBuffer(m_device); // 다시 vertexbuffer 만들기
						ImGui::PopID( );
						break;
					}
					ImGui::PopID( );
				}
				if (flag)
					selectedBillboard->UpdateConstBuffer(m_device, m_context);

				if (ImGui::Button("Add point")) {
					selectedBillboard->m_points.push_back({ 0.0f, 1.0f, 2.0f, 1.0f });
					selectedBillboard->UpdateVertexBuffer(m_device); // 다시 vertexbuffer 만들기
				}
			}

		}
		else {
			ImGui::Text("No Billboard Points available.");
		}
		ImGui::End( );
	}
	
}

//:TODO: 적당한 곳에 정의를 옮기자..
void KuskApp::CreateSphere(float radius, int numSlices, int numStacks, Vector2& texScale) {
	MeshData mesh = GeometryGenerator::MakeSphere(radius, numSlices, numStacks, texScale);
	auto obj = make_shared<Model>(m_device, m_context, vector{ mesh });
	obj->m_modelCreationParams = m_modelParams;

	m_basicList.push_back(obj);
	m_savedList.push_back(obj);
}

void KuskApp::CreateSquare(float scale, Vector2& texScale, bool isMirror) {

	MeshData mesh = GeometryGenerator::MakeSquare(scale, texScale);
	auto obj = make_shared<Model>(m_device, m_context, vector{ mesh });
	obj->m_modelCreationParams = m_modelParams;

	if (false == isMirror) {
		m_basicList.push_back(obj);
		m_savedList.push_back(obj);
	}
	else {
		m_mirror = obj;
		m_mirrorPlane = DirectX::SimpleMath::Plane(Vector3(0.0f), Vector3(0.0f, 0.0f, -1.0f));
	}
}

void KuskApp::CreateSquareGrid(int numSlices, int numStacks, float scale, Vector2& texScale) {

	MeshData mesh = GeometryGenerator::MakeSquareGrid(numSlices, numStacks, scale, texScale);
	auto obj = make_shared<Model>(m_device, m_context, vector{ mesh });
	obj->m_modelCreationParams = m_modelParams;

	m_basicList.push_back(obj);
	m_savedList.push_back(obj);
}

void KuskApp::CreateCylinder(float bottomRadius, float topRadius, float height, int numSlices) {

	MeshData mesh = GeometryGenerator::MakeCylinder(bottomRadius, topRadius, height, numSlices);
	auto obj = make_shared<Model>(m_device, m_context, vector{ mesh });
	obj->m_modelCreationParams = m_modelParams;

	m_basicList.push_back(obj);
	m_savedList.push_back(obj);
}

void KuskApp::CreateBox(float scale) {

	MeshData mesh = GeometryGenerator::MakeBox(scale);
	auto obj = make_shared<Model>(m_device, m_context, vector{ mesh });
	obj->m_modelCreationParams = m_modelParams;

	m_basicList.push_back(obj);
	m_savedList.push_back(obj);
}

void KuskApp::CreateModelFromFile(const std::string& fullPath) {

	std::filesystem::path filePath(fullPath);
	std::string dirPath = filePath.parent_path( ).string( ) + "\\";
	std::string fileName = filePath.filename( ).string( );
	std::string ext = filePath.extension( ).string( );

	auto meshes = GeometryGenerator::ReadFromFile(dirPath, fileName);
	auto obj = make_shared<Model>(m_device, m_context, meshes);
	obj->m_materialConstsCPU.invertNormalMapY = (ext == ".gltf") ? true : false;
	obj->m_materialConstsCPU.albedoFactor = Vector3(1.0f);
	obj->m_materialConstsCPU.roughnessFactor = 0.3f;
	obj->m_materialConstsCPU.metallicFactor = 0.8f;
	obj->m_materialConstsCPU.emissionFactor = Vector3(0.0f);
	obj->UpdateConstantBuffers(m_device, m_context);
	obj->m_modelCreationParams = m_modelParams;

	m_basicList.push_back(obj);
	m_savedList.push_back(obj);
}

void KuskApp::CreateBillboardPoints( ) {
	vector<Vector4> points;
	Vector4 p = { -1.0f * m_billboardTextureList.size( ), 1.0f, 2.0f, 1.0f };
	for (int i = 0; i < m_billboardTextureList.size( ); i++) {
		points.push_back(p);
		p.x += 1.8f;
	}

	auto billboardPoints = make_shared<BillboardPoints>( );
	const float defaultWidth = 2.4f;
	billboardPoints->Initialize(m_device, m_context, points, defaultWidth, m_billboardTextureList);
	m_billboardPointsList.push_back(billboardPoints);

	m_billboardTextureList.clear( );
}

void KuskApp::LoadSceneDataFromJSON(std::string& filePath) {

	m_selectedModelIndex = -1;
	// 장면 초기화
	m_basicList.clear( );
	m_savedList.clear( );
	for (int i = 0; i < MAX_LIGHTS; i++) {
		m_basicList.push_back(m_lightSphere[ i ]);
	}
	m_basicList.push_back(m_cursorSphere);
	m_billboardPointsList.clear( );

	// JSON 파일의 데이터로 모델 생성 후 리스트 등록
	rapidjson::Document doc;
	doc = JsonManager::LoadJson(filePath);

	if (doc.HasMember("Models")) {
		rapidjson::GenericArray models = doc[ "Models" ].GetArray();

		for (rapidjson::Value& model : models) {
			auto obj = make_shared<Model>(m_device, m_context, model);
			if (false == obj->m_isMirror) {
				m_basicList.push_back(obj);
				m_savedList.push_back(obj);
			}
			else { // 거울
				m_mirror = obj;
				MirrorDataFromJSON(model);
			}
		}
	}

	if (doc.HasMember("Lights")) {
		rapidjson::GenericArray lights = doc[ "Lights" ].GetArray( );

		int i = 0;
		for (rapidjson::Value& lightData : lights) {
			//
			LightFromJSON(lightData, m_globalConstsCPU.lights[ i ]);
			if (m_globalConstsCPU.lights[ i ].type == 0) {
				m_lightSphere[ i ]->m_isVisible = false;
			}
			else {
				m_lightSphere[ i ]->m_isVisible = true;
			}
			i++;

			if (i == MAX_LIGHTS)
				break;
		}
	}

	if (doc.HasMember("Scene")) {
		rapidjson::Value& scene = doc[ "Scene" ];
		if (scene.HasMember("screen")) {
			ScreenDataFromJSON(scene[ "screen" ]);
		}
		if (scene.HasMember("env")) {
			EnvDataFromJSON(scene[ "env" ]);
		}
		if (scene.HasMember("camera")) {
			m_camera.CameraDataFromJSON(scene[ "camera" ]);
		}
		if (scene.HasMember("post_effects")) {
			PostEffectDataFromJSON(scene[ "post_effects" ]);
		}
		if (scene.HasMember("post_processing")) {
			PostProcessingDataFromJSON(scene[ "post_processing" ]);
		}
	}

	if (doc.HasMember("Billboards")) {
		rapidjson::Value& billboards = doc[ "Billboards" ];
		for (rapidjson::Value& bill : billboards.GetArray( )) {
			auto billboard = make_shared<BillboardPoints>( );
			billboard->InitializeFromJSON(m_device, m_context, bill);
			m_billboardPointsList.push_back(billboard);
		}	
	}
}

void KuskApp::SaveSceneDataAsJSON(std::string& filePath) {
	
	rapidjson::Document doc;
	doc.SetObject( );
	
	rapidjson::Document::AllocatorType& allocator = doc.GetAllocator( );

	// 모델 정보 저장
	rapidjson::Value models(rapidjson::kArrayType);
	for (auto& obj : m_savedList) {
		rapidjson::Value model;
		model = obj->ToJson(allocator);
		models.PushBack(model, allocator);
	}

	// 거울 모델 정보 저장
	if (m_mirror) {
		rapidjson::Value model;
		model = m_mirror->ToJson(allocator);
		MirrorDataToJSON(m_mirror, model, allocator);
		models.PushBack(model, allocator);
	}
	
	doc.AddMember("Models", models, allocator);

	// 조명 정보 저장
	rapidjson::Value lights(rapidjson::kArrayType);
	for (int i = 0; i < MAX_LIGHTS; i++) {
		rapidjson::Value light;
		light = LightToJSON(m_globalConstsCPU.lights[i], allocator);
		lights.PushBack(light, allocator);
	}
	doc.AddMember("Lights", lights, allocator);

	// 장면 정보
	rapidjson::Value scene(rapidjson::kObjectType);
	rapidjson::Value tmpObj(rapidjson::kObjectType);

	// 장면 - 환경 정보 저장
	tmpObj = EnvDataToJSON(allocator);
	scene.AddMember("env", tmpObj, allocator);

	// 장면 - 카메라 정보 저장
	tmpObj = m_camera.CameraDataToJSON(allocator);
	scene.AddMember("camera", tmpObj, allocator);

	// 장면 - PostEffect 정보 저장
	tmpObj = PostEffectDataToJSON(allocator);
	scene.AddMember("post_effects", tmpObj, allocator);

	// 장면 - PostProcessing 정보 저장
	tmpObj = PostProcessingDataToJSON(allocator);
	scene.AddMember("post_processing", tmpObj, allocator);

	// 장면 - Screen 정보 저장
	tmpObj = ScreenDataToJSON(allocator);
	scene.AddMember("screen", tmpObj, allocator);

	doc.AddMember("Scene", scene, allocator);

	// 빌보드 정보 저장
	rapidjson::Value billboards(rapidjson::kArrayType);
	for (int i = 0; i < m_billboardPointsList.size( ); i++) {
		rapidjson::Value billboard;
		billboard = m_billboardPointsList[ i ]->BillboardToJSON(allocator);
		billboards.PushBack(billboard, allocator);
	}
	doc.AddMember("Billboards", billboards, allocator);

	JsonManager::SaveJson(filePath, doc);
}

rapidjson::Value KuskApp::LightToJSON(Light& l, rapidjson::Document::AllocatorType& allocator) {
	rapidjson::Value value(rapidjson::kObjectType);

	value.AddMember("radiance", JsonManager::Vector3ToJson(l.radiance, allocator), allocator);
	value.AddMember("direction", JsonManager::Vector3ToJson(l.direction, allocator), allocator);
	value.AddMember("position", JsonManager::Vector3ToJson(l.position, allocator), allocator);
	value.AddMember("fall_off_start", l.fallOffStart, allocator);
	value.AddMember("fall_off_end", l.fallOffEnd, allocator);
	value.AddMember("spot_power", l.spotPower, allocator);
	value.AddMember("type", l.type, allocator);
	value.AddMember("radius", l.radius, allocator);
	value.AddMember("halo_radius", l.haloRadius, allocator);
	value.AddMember("halo_strength", l.haloStrength, allocator);

	return value;
}

void KuskApp::LightFromJSON(rapidjson::Value& lightData, Light& l)
{
	if (lightData.HasMember("radiance")) {
		l.radiance = JsonManager::ParseVector3(lightData[ "radiance" ]);
	}
	if (lightData.HasMember("direction")) {
		l.direction = JsonManager::ParseVector3(lightData[ "direction" ]);
	}
	if (lightData.HasMember("position")) {
		l.position = JsonManager::ParseVector3(lightData[ "position" ]);
	}
	if (lightData.HasMember("fall_off_start")) {
		l.fallOffStart = lightData[ "fall_off_start" ].GetFloat( );
	}
	if (lightData.HasMember("fall_off_end")) {
		l.fallOffEnd = lightData[ "fall_off_end" ].GetFloat( );
	}

	if (lightData.HasMember("spot_power")) {
		l.spotPower = lightData[ "spot_power" ].GetFloat( );
	}
	if (lightData.HasMember("type")) {
		l.type = lightData[ "type" ].GetUint( );
	}
	if (lightData.HasMember("radius")) {
		l.radius = lightData[ "radius" ].GetFloat( );
	}
	if (lightData.HasMember("halo_radius")) {
		l.haloRadius = lightData[ "halo_radius" ].GetFloat( );
	}
	if (lightData.HasMember("halo_strength")) {
		l.haloStrength = lightData[ "halo_strength" ].GetFloat( );
	}
}

rapidjson::Value KuskApp::EnvDataToJSON(rapidjson::Document::AllocatorType& allocator) {
	rapidjson::Value value(rapidjson::kObjectType);

	value.AddMember("strength_ibl", m_globalConstsCPU.strengthIBL, allocator);
	value.AddMember("textrue_to_draw", m_globalConstsCPU.textureToDraw, allocator);
	value.AddMember("env_lod_bias", m_globalConstsCPU.envLodBias, allocator);
	value.AddMember("lod_bias", m_globalConstsCPU.lodBias, allocator);

	rapidjson::Value strValue(rapidjson::kStringType);
	strValue.SetString(m_cubemapTextureEnvFilePath.c_str(), allocator);
	value.AddMember("cubemap_orgn_dds", strValue, allocator);

	strValue.SetString(m_cubemapTextureSpecularFilePath.c_str( ),  allocator);
	value.AddMember("cubemap_spec_dds", strValue, allocator);

	strValue.SetString(m_cubemapTextureIrradianceFilePath.c_str( ), allocator);
	value.AddMember("cubemap_diff_dds", strValue, allocator);

	strValue.SetString(m_cubemapTextureBrdfFilePath.c_str( ), allocator);
	value.AddMember("cubemap_brdf_dds", strValue, allocator);

	return value;
}

void KuskApp::EnvDataFromJSON(rapidjson::Value& value) {
	if (value.HasMember("strength_ibl")) {
		m_globalConstsCPU.strengthIBL = value[ "strength_ibl" ].GetFloat( );
	}
	if (value.HasMember("textrue_to_draw")) {
		m_globalConstsCPU.textureToDraw = value[ "textrue_to_draw" ].GetInt( );
	}
	if (value.HasMember("env_lod_bias")) {
		m_globalConstsCPU.envLodBias = value[ "env_lod_bias" ].GetFloat( );
	}
	if (value.HasMember("lod_bias")) {
		m_globalConstsCPU.lodBias = value[ "lod_bias" ].GetFloat( );
	}

	if (value.HasMember("cubemap_orgn_dds")) {
		m_cubemapTextureEnvFilePath = value[ "cubemap_orgn_dds" ].GetString( );
	}
	if (value.HasMember("cubemap_spec_dds")) {
		m_cubemapTextureSpecularFilePath = value[ "cubemap_spec_dds" ].GetString( );
	}
	if (value.HasMember("cubemap_diff_dds")) {
		m_cubemapTextureIrradianceFilePath = value[ "cubemap_diff_dds" ].GetString( );
	}
	if (value.HasMember("cubemap_brdf_dds")) {
		m_cubemapTextureBrdfFilePath = value[ "cubemap_brdf_dds" ].GetString( );
	}
	AppBase::InitCubemaps(
				JsonManager::UTF8ToWString(m_cubemapTextureEnvFilePath),
				JsonManager::UTF8ToWString(m_cubemapTextureSpecularFilePath),
				JsonManager::UTF8ToWString(m_cubemapTextureIrradianceFilePath),
				JsonManager::UTF8ToWString(m_cubemapTextureBrdfFilePath));
}

rapidjson::Value KuskApp::PostEffectDataToJSON(rapidjson::Document::AllocatorType& allocator) {
	rapidjson::Value value(rapidjson::kObjectType);
	value.AddMember("fog_strength", m_postEffectsConstsCPU.fogStrength, allocator);
	
	return value;
}

void KuskApp::PostEffectDataFromJSON(rapidjson::Value& value) {
	if (value.HasMember("fog_strength")) {
		m_postEffectsConstsCPU.fogStrength = value[ "fog_strength" ].GetFloat( );
	}
	D3D11Utils::UpdateBuffer(m_device, m_context, m_postEffectsConstsCPU, m_postEffectsConstsGPU);
}

rapidjson::Value KuskApp::PostProcessingDataToJSON(rapidjson::Document::AllocatorType& allocator) {
	rapidjson::Value value(rapidjson::kObjectType);
	value.AddMember("bloom_strength", m_postProcess.m_combineFilter.m_constData.strength, allocator);
	value.AddMember("exposure", m_postProcess.m_combineFilter.m_constData.option1, allocator);
	value.AddMember("gamma", m_postProcess.m_combineFilter.m_constData.option2, allocator);

	return value;
}

void KuskApp::PostProcessingDataFromJSON(rapidjson::Value& value) {
	if (value.HasMember("bloom_strength")) {
		m_postProcess.m_combineFilter.m_constData.strength = value[ "bloom_strength" ].GetFloat( );
	}
	if (value.HasMember("exposure")) {
		m_postProcess.m_combineFilter.m_constData.option1 = value[ "exposure" ].GetFloat( );
	}
	if (value.HasMember("gamma")) {
		m_postProcess.m_combineFilter.m_constData.option2 = value[ "gamma" ].GetFloat( );
	}

	m_postProcess.m_combineFilter.UpdateConstantBuffers(m_device, m_context);
}

rapidjson::Value KuskApp::ScreenDataToJSON(rapidjson::Document::AllocatorType& allocator) {
	rapidjson::Value value(rapidjson::kObjectType);
	value.AddMember("screen_width", m_screenWidth, allocator);
	value.AddMember("screen_height", m_screenHeight, allocator);

	return value;
}

void KuskApp::ScreenDataFromJSON(rapidjson::Value& value) {
	if (value.HasMember("screen_width")) {
		m_screenWidth = value[ "screen_width" ].GetInt( );
	}
	if (value.HasMember("screen_height")) {
		m_screenHeight = value[ "screen_height" ].GetInt( );
	}

	SetWindowPos(m_mainWindow, 0, 0, 0, m_screenWidth, m_screenHeight, SWP_NOZORDER | SWP_NOMOVE | SWP_SHOWWINDOW);
}

void KuskApp::MirrorDataToJSON(shared_ptr<Model>& model, rapidjson::Value& value, rapidjson::Document::AllocatorType& allocator) {

	rapidjson::Value mirror(rapidjson::kObjectType);

	mirror.AddMember("alpha", m_mirrorAlpha, allocator);
	mirror.AddMember("rotation", JsonManager::Vector3ToJson(m_mirrorRotation, allocator), allocator);
	mirror.AddMember("scale", JsonManager::Vector3ToJson(m_mirrorScale, allocator), allocator);

	value.AddMember("mirror_data", mirror, allocator);

}

void KuskApp::MirrorDataFromJSON(rapidjson::Value& value) {
	if (value.HasMember("mirror_data")) {
		rapidjson::Value& data = value[ "mirror_data" ];
				
		if (data.HasMember("alpha")) {
			m_mirrorAlpha = data[ "alpha" ].GetFloat( );
		}

		if (data.HasMember("rotation")) {
			m_mirrorRotation = JsonManager::ParseVector3(data[ "rotation" ]);
		}

		if (data.HasMember("scale")) {
			m_mirrorScale = JsonManager::ParseVector3(data[ "scale" ]);
		}

		Vector3 normal = Vector3(0.0f, 0.0f, -1.0f);
		normal = Vector3::TransformNormal(normal, m_mirror->m_worldITRow);
		normal.Normalize( );
		Vector3 worldPosition = Vector3::Transform(Vector3(0.0f, 0.0f, 0.0f), m_mirror->m_worldRow);
		float d = -normal.Dot(worldPosition); // -(Ax+By+Cz) = D
		m_mirrorPlane = DirectX::SimpleMath::Plane(normal.x, normal.y, normal.z, d);
	}
}


} // namespace kusk