#include "KuskApp.h"

#include <tuple>
#include <vector>

namespace kusk {
using namespace std;

auto MakeSquare() {
	vector<Vector3> positions;
	vector<Vector3> colors;
	vector<Vector3> normals;
	vector<Vector2> texcoords; // 텍스쳐 좌표

	const float scale = 1.0f;

	positions.push_back(Vector3(-1.0f, 1.0f, 0.0f) * scale);
	positions.push_back(Vector3(1.0f, 1.0f, 0.0f) * scale);
	positions.push_back(Vector3(1.0f, -1.0f, 0.0f) * scale);
	positions.push_back(Vector3(-1.0f, -1.0f, 0.0f) * scale);
	colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
	colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
	colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
	colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
	normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
	normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
	normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
	normals.push_back(Vector3(0.0f, 0.0f, -1.0f));

	texcoords.push_back(Vector2(0.0f, 0.0f));
	texcoords.push_back(Vector2(1.0f, 0.0f));
	texcoords.push_back(Vector2(1.0f, 1.0f));
	texcoords.push_back(Vector2(0.0f, 1.0f));

	vector<Vertex> vertices;
	for (size_t i = 0; i < positions.size(); i++) {
		Vertex v;
		v.position = positions[i];
		v.normal = normals[i];
		v.texcoord = texcoords[i];
		vertices.push_back(v);
	}

	vector<uint16_t> indices = {
		0, 1, 2, 0, 2, 3,
	};

	return tuple{ vertices, indices };
}

auto MakeBox() {
	vector<Vector3> positions;
	vector<Vector3> colors;
	vector<Vector3> normals;
	vector<Vector2> texcoords; // 텍스쳐 좌표

	const float scale = 1.0f;

	const Vector3 v0 = Vector3(-1.0f, 1.0f, -1.0f);
	const Vector3 v1 = Vector3(-1.0f, 1.0f, 1.0f);
	const Vector3 v2 = Vector3(1.0f, 1.0f, 1.0f);
	const Vector3 v3 = Vector3(1.0f, 1.0f, -1.0f);
	const Vector3 v4 = Vector3(-1.0f, -1.0f, -1.0f);
	const Vector3 v5 = Vector3(-1.0f, -1.0f, 1.0f);
	const Vector3 v6 = Vector3(1.0f, -1.0f, 1.0f);
	const Vector3 v7 = Vector3(1.0f, -1.0f, -1.0f);

	// 윗면
	positions.push_back(v0 * scale);
	positions.push_back(v1 * scale);
	positions.push_back(v2 * scale);
	positions.push_back(v3 * scale);
	colors.push_back(Vector3(1.0f, 0.0f, 0.0f));
	colors.push_back(Vector3(1.0f, 0.0f, 0.0f));
	colors.push_back(Vector3(1.0f, 0.0f, 0.0f));
	colors.push_back(Vector3(1.0f, 0.0f, 0.0f));
	normals.push_back(Vector3(0.0f, 1.0f, 0.0f));
	normals.push_back(Vector3(0.0f, 1.0f, 0.0f));
	normals.push_back(Vector3(0.0f, 1.0f, 0.0f));
	normals.push_back(Vector3(0.0f, 1.0f, 0.0f));
	texcoords.push_back({ 0.0f, 1.0f });
	texcoords.push_back({ 0.0f, 0.0f });
	texcoords.push_back({ 1.0f, 0.0f });
	texcoords.push_back({ 1.0f, 1.0f });

	// 아랫면
	positions.push_back(v4 * scale);
	positions.push_back(v7 * scale);
	positions.push_back(v6 * scale);
	positions.push_back(v5 * scale);
	colors.push_back(Vector3(0.0f, 1.0f, 0.0f));
	colors.push_back(Vector3(0.0f, 1.0f, 0.0f));
	colors.push_back(Vector3(0.0f, 1.0f, 0.0f));
	colors.push_back(Vector3(0.0f, 1.0f, 0.0f));
	normals.push_back(Vector3(0.0f, -1.0f, 0.0f));
	normals.push_back(Vector3(0.0f, -1.0f, 0.0f));
	normals.push_back(Vector3(0.0f, -1.0f, 0.0f));
	normals.push_back(Vector3(0.0f, -1.0f, 0.0f));
	texcoords.push_back({ 0.0f, 1.0f });
	texcoords.push_back({ 0.0f, 0.0f });
	texcoords.push_back({ 1.0f, 0.0f });
	texcoords.push_back({ 1.0f, 1.0f });

	// 앞면
	positions.push_back(v4 * scale);
	positions.push_back(v0 * scale);
	positions.push_back(v3 * scale);
	positions.push_back(v7 * scale);
	colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
	colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
	colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
	colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
	normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
	normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
	normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
	normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
	texcoords.push_back({ 0.0f, 1.0f });
	texcoords.push_back({ 0.0f, 0.0f });
	texcoords.push_back({ 1.0f, 0.0f });
	texcoords.push_back({ 1.0f, 1.0f });

	// 뒷면
	positions.push_back(v5 * scale);
	positions.push_back(v6 * scale);
	positions.push_back(v2 * scale);
	positions.push_back(v1 * scale);
	colors.push_back(Vector3(0.0f, 1.0f, 1.0f));
	colors.push_back(Vector3(0.0f, 1.0f, 1.0f));
	colors.push_back(Vector3(0.0f, 1.0f, 1.0f));
	colors.push_back(Vector3(0.0f, 1.0f, 1.0f));
	normals.push_back(Vector3(0.0f, 0.0f, 1.0f));
	normals.push_back(Vector3(0.0f, 0.0f, 1.0f));
	normals.push_back(Vector3(0.0f, 0.0f, 1.0f));
	normals.push_back(Vector3(0.0f, 0.0f, 1.0f));
	texcoords.push_back({ 0.0f, 1.0f });
	texcoords.push_back({ 0.0f, 0.0f });
	texcoords.push_back({ 1.0f, 0.0f });
	texcoords.push_back({ 1.0f, 1.0f });

	// 왼쪽
	positions.push_back(v5 * scale);
	positions.push_back(v1 * scale);
	positions.push_back(v0 * scale);
	positions.push_back(v4 * scale);
	colors.push_back(Vector3(1.0f, 1.0f, 0.0f));
	colors.push_back(Vector3(1.0f, 1.0f, 0.0f));
	colors.push_back(Vector3(1.0f, 1.0f, 0.0f));
	colors.push_back(Vector3(1.0f, 1.0f, 0.0f));
	normals.push_back(Vector3(-1.0f, 0.0f, 0.0f));
	normals.push_back(Vector3(-1.0f, 0.0f, 0.0f));
	normals.push_back(Vector3(-1.0f, 0.0f, 0.0f));
	normals.push_back(Vector3(-1.0f, 0.0f, 0.0f));
	texcoords.push_back({ 0.0f, 1.0f });
	texcoords.push_back({ 0.0f, 0.0f });
	texcoords.push_back({ 1.0f, 0.0f });
	texcoords.push_back({ 1.0f, 1.0f });

	// 오른쪽
	positions.push_back(v6 * scale);
	positions.push_back(v7 * scale);
	positions.push_back(v3 * scale);
	positions.push_back(v2 * scale);
	colors.push_back(Vector3(1.0f, 0.0f, 1.0f));
	colors.push_back(Vector3(1.0f, 0.0f, 1.0f));
	colors.push_back(Vector3(1.0f, 0.0f, 1.0f));
	colors.push_back(Vector3(1.0f, 0.0f, 1.0f));
	normals.push_back(Vector3(1.0f, 0.0f, 0.0f));
	normals.push_back(Vector3(1.0f, 0.0f, 0.0f));
	normals.push_back(Vector3(1.0f, 0.0f, 0.0f));
	normals.push_back(Vector3(1.0f, 0.0f, 0.0f));
	texcoords.push_back({ 0.0f, 1.0f });
	texcoords.push_back({ 0.0f, 0.0f });
	texcoords.push_back({ 1.0f, 0.0f });
	texcoords.push_back({ 1.0f, 1.0f });

	vector<Vertex> vertices;
	for (size_t i = 0; i < positions.size(); i++) {
		Vertex v;
		v.position = positions[i];
		v.normal = normals[i];
		v.texcoord = texcoords[i];
		vertices.push_back(v);
	}

	vector<uint16_t> indices = {
		0, 1, 2, 0, 2, 3, // 윗면
		4, 5, 6, 4, 6, 7, // 아랫면
		8, 9, 10, 8, 10, 11, // 앞면
		12, 13, 14, 12, 14, 15, // 뒷면
		16, 17, 18, 16, 18, 19, // 왼쪽
		20, 21, 22, 20, 22, 23 // 오른쪽
	};

	return tuple{ vertices, indices };
}

KuskApp::KuskApp() : AppBase(), m_indexCount(0), m_pixelConstantBufferData() {}

bool KuskApp::Initialize() {

	if (!AppBase::Initialize())
		return false;

	// Texture 만들기
	AppBase::CreateTexture("crate2_diffuse.png", m_texture, m_textureResourceView);
	AppBase::CreateTexture("wall.jpg", m_texture2, m_textureResourceView2);

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
	auto [vertices, indices] = MakeBox();

	// Vertex Buffer
	AppBase::CreateVertexBuffer(vertices, m_vertexBuffer);

	// Index Buffer
	m_indexCount = UINT(indices.size());
	AppBase::CreateIndexBuffer(indices, m_indexBuffer);

	// Vertex Constant Buffer
	m_vertexConstantBufferData.model = Matrix();
	m_vertexConstantBufferData.view = Matrix();
	m_vertexConstantBufferData.proj = Matrix();
	AppBase::CreateConstantBuffer(m_vertexConstantBufferData, m_vertexConstantBuffer);

	// Pixel Constant Buffer
	AppBase::CreateConstantBuffer(m_pixelConstantBufferData, m_pixelConstantBuffer);

	// Make Shaders
	vector<D3D11_INPUT_ELEMENT_DESC> inputElements = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 4 * 3, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 4 * 3 + 4 * 3, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	AppBase::CreateVertexShaderAndInputLayout(L"BasicVertexShader.hlsl", inputElements,
												m_colorVertexShader, m_colorInputLayout);

	AppBase::CreatePixelShader(L"BasicPixelShader.hlsl", m_colorPixelShader);

	return true;
}

void KuskApp::Update(float dt) {
	
	using namespace DirectX;
	
	// 모델의 변환
	m_vertexConstantBufferData.model = Matrix::CreateScale(m_modelScaling) * 
								 Matrix::CreateRotationX(m_modelRotation.x) *
								 Matrix::CreateRotationY(m_modelRotation.y) *
								 Matrix::CreateRotationZ(m_modelRotation.z) *
								 Matrix::CreateTranslation(m_modelTranslation);
	m_vertexConstantBufferData.model = m_vertexConstantBufferData.model.Transpose();

	m_vertexConstantBufferData.invTranspose = m_vertexConstantBufferData.model;
	m_vertexConstantBufferData.invTranspose.Translation(Vector3(0.0f));
	m_vertexConstantBufferData.invTranspose.Transpose( ).Invert( );

	// 시점 변환
	//XMMatrixLookToLH(m_viewEyePos, m_viewEyeDir, m_viewUp);
	m_vertexConstantBufferData.view =
		Matrix::CreateRotationY(m_viewRot) *
		Matrix::CreateTranslation(0.0f, 0.0f, 2.0f);

	m_pixelConstantBufferData.eyeWorld = Vector3::Transform(
		Vector3(0.0f), m_vertexConstantBufferData.view.Invert( ));
		
	m_vertexConstantBufferData.view = m_vertexConstantBufferData.view.Transpose();

	// 프로젝션
	m_aspect = AppBase::GetAspectRatio( );
	if (m_usePerspectiveProjection) {
		m_vertexConstantBufferData.proj =
			XMMatrixPerspectiveFovLH(XMConvertToRadians(m_projFovAngleY), m_aspect, m_nearZ, m_farZ);
	}
	else {
		m_vertexConstantBufferData.proj =
			XMMatrixOrthographicOffCenterLH(-m_aspect, m_aspect, -1.0f, 1.0f, m_nearZ, m_farZ);
	}
	m_vertexConstantBufferData.proj = m_vertexConstantBufferData.proj.Transpose();

	// Constant를 CPU에서 GPU로복사
	AppBase::UpdateBuffer(m_vertexConstantBufferData, m_vertexConstantBuffer);

	m_pixelConstantBufferData.material.diffuse = Vector3(m_materialDiffuse);
	m_pixelConstantBufferData.material.specular = Vector3(m_materialSpecular);

	// 여러 개 조명 사용 예시
	for (int i = 0; i < MAX_LIGHTS; i++) {
		// 다른 조명 끄기
		if (i != m_lightType) {
			m_pixelConstantBufferData.light[ i ].strength *= 0.0f;
		}
		else {
			m_pixelConstantBufferData.light[ i ] = m_lightFromGUI;
		}
	}
	AppBase::UpdateBuffer(m_pixelConstantBufferData, m_pixelConstantBuffer);
}

void KuskApp::Render() {
	
	m_context->RSSetViewports(1, &m_screenViewport);

	float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	m_context->ClearRenderTargetView(m_renderTargetView.Get(), clearColor);
	m_context->ClearDepthStencilView(m_depthStencilView.Get(),
									 D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	//
	m_context->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), m_depthStencilView.Get());
	m_context->OMSetDepthStencilState(m_depthStencilState.Get(), 0);

	// Shader setting
	m_context->VSSetShader(m_colorVertexShader.Get(), 0, 0);
	m_context->VSSetConstantBuffers(0, 1, m_vertexConstantBuffer.GetAddressOf());

	ID3D11ShaderResourceView* pixelResources[ 2 ] = {
		m_textureResourceView.Get( ),
		m_textureResourceView2.Get( ),
	};
	m_context->PSSetShaderResources(0, 2, pixelResources);
	m_context->PSSetSamplers(0, 1, m_samplerState.GetAddressOf( ));
	m_context->PSSetConstantBuffers(0, 1, m_pixelConstantBuffer.GetAddressOf( ));
	m_context->PSSetShader(m_colorPixelShader.Get(), 0, 0);
	
	m_context->RSSetState(m_rasterizerState.Get());

	// Vertex/Index Buffer setting
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	m_context->IASetInputLayout(m_colorInputLayout.Get());
	m_context->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);
	m_context->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
	m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_context->DrawIndexed(m_indexCount, 0, 0);
}

void KuskApp::UpdateGUI() {
	ImGui::Checkbox("Use Texture", &m_pixelConstantBufferData.useTexture);

	ImGui::SliderFloat3("m_modelTranslation", &m_modelTranslation.x, -2.0f, 2.0f);
	ImGui::SliderFloat3("m_modelRotation", &m_modelRotation.x, -3.14f, 3.14f);
	ImGui::SliderFloat3("m_modelScaling", &m_modelScaling.x, 0.1f, 2.0f);
	ImGui::SliderFloat("m_viewRot", &m_viewRot, -3.14f, 3.14f);

	ImGui::SliderFloat("Material Shininess",
					   &m_pixelConstantBufferData.material.shininess, 1.0f, 256.0f);

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

	ImGui::SliderFloat("Material Diffuse", &m_materialDiffuse, 0.0f, 1.0f);
	ImGui::SliderFloat("Material Specular", &m_materialSpecular, 0.0f, 1.0f);

	ImGui::SliderFloat3("Light Position", &m_lightFromGUI.position.x, -5.0f, 5.0f);

	ImGui::SliderFloat("Light fallOffStart", &m_lightFromGUI.fallOffStart, 0.0f, 5.0f);

	ImGui::SliderFloat("Light fallOffEnd", &m_lightFromGUI.fallOffEnd, 0.0f, 10.0f);

	ImGui::SliderFloat("Light spotPower", &m_lightFromGUI.spotPower, 1.0f, 512.0f);
}

} // namespace kusk