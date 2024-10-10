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
		v.color = colors[i];
		v.uv = texcoords[i];
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

	vector<Vertex> vertices;
	for (size_t i = 0; i < positions.size(); i++) {
		Vertex v;
		v.position = positions[i];
		v.color = colors[i];
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

KuskApp::KuskApp() : AppBase(), m_indexCount(0) {}

bool KuskApp::Initialize() {

	if (!AppBase::Initialize())
		return false;

	// Geometry 정의
	auto [vertices, indices] = MakeSquare();

	// Vertex Buffer
	AppBase::CreateVertexBuffer(vertices, m_vertexBuffer);

	// Index Buffer
	m_indexCount = UINT(indices.size());
	AppBase::CreateIndexBuffer(indices, m_indexBuffer);

	// Vertex Constant Buffer
	m_vsConstantBufferData.model = Matrix();
	m_vsConstantBufferData.view = Matrix();
	m_vsConstantBufferData.proj = Matrix();
	AppBase::CreateConstantBuffer(m_vsConstantBufferData, m_vsConstantBuffer);

	// Pixel Constant Buffer
	m_psConstantBufferData.xSplit = 0.0f;
	AppBase::CreateConstantBuffer(m_psConstantBufferData, m_psConstantBuffer);

	// Make Shaders
	vector<D3D11_INPUT_ELEMENT_DESC> inputElements = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 4 * 3, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 4 * 3 * 2, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	AppBase::CreateVertexShaderAndInputLayout(L"ColorVertexShader.hlsl", inputElements,
												m_colorVertexShader, m_colorInputLayout);

	AppBase::CreatePixelShader(L"ColorPixelShader.hlsl", m_colorPixelShader);

	return true;
}

void KuskApp::Update(float dt) {
	
	using namespace DirectX;
	
	// 모델의 변환
	m_vsConstantBufferData.model = Matrix::CreateScale(m_modelScaling) * 
								 Matrix::CreateRotationY(m_modelRotation.y) *
								 Matrix::CreateRotationX(m_modelRotation.x) *
								 Matrix::CreateRotationZ(m_modelRotation.z) *
								 Matrix::CreateTranslation(m_modelTranslation);
	m_vsConstantBufferData.model = m_vsConstantBufferData.model.Transpose();

	// 시점 변환
	m_vsConstantBufferData.view =
		XMMatrixLookToLH(m_viewEyePos, m_viewEyeDir, m_viewUp);
	m_vsConstantBufferData.view = m_vsConstantBufferData.view.Transpose();

	// 프로젝션
	if (m_usePerspectiveProjection) {
		m_vsConstantBufferData.proj =
			XMMatrixPerspectiveFovLH(XMConvertToRadians(m_projFovAngleY), m_aspect, m_nearZ, m_farZ);
	}
	else {
		m_vsConstantBufferData.proj =
			XMMatrixOrthographicOffCenterLH(-m_aspect, m_aspect, -1.0f, 1.0f, m_nearZ, m_farZ);
	}
	m_vsConstantBufferData.proj = m_vsConstantBufferData.proj.Transpose();

	// Constant를 CPU에서 GPU로복사
	AppBase::UpdateBuffer(m_vsConstantBufferData, m_vsConstantBuffer);
	AppBase::UpdateBuffer(m_psConstantBufferData, m_psConstantBuffer);
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
	m_context->VSSetConstantBuffers(0, 1, m_vsConstantBuffer.GetAddressOf());
	m_context->PSSetShader(m_colorPixelShader.Get(), 0, 0);
	m_context->PSSetConstantBuffers(0, 1, m_psConstantBuffer.GetAddressOf());
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
	ImGui::SliderFloat("xSplit", &m_psConstantBufferData.xSplit, 0.0f, 1.0f);
	/*ImGui::Checkbox("usePerspectiveProjection", &m_usePerspectiveProjection);
	ImGui::SliderFloat3("m_modelTranslation", &m_modelTranslation.x, -2.0f, 2.0f);
	ImGui::SliderFloat3("m_modelRotation(Radian)", &m_modelRotation.x, -3.14f, 3.14f);
	ImGui::SliderFloat3("m_modelScaling", &m_modelScaling.x, 0.1f, 2.0f);

	ImGui::SliderFloat3("m_viewEyePos", &m_viewEyePos.x, -4.0f, 4.0f);
	ImGui::SliderFloat3("m_viewEyeDir", &m_viewEyeDir.x, -4.0f, 4.0f);
	ImGui::SliderFloat3("m_viewUp", &m_viewUp.x, -2.0f, 2.0f);

	ImGui::SliderFloat("m_projFovAngleY(Degree)", &m_projFovAngleY, 10.0f, 180.0f);
	ImGui::SliderFloat("m_nearZ", &m_nearZ, 0.01f, 10.0f);
	ImGui::SliderFloat("m_farZ", &m_farZ, 0.01f, 10.0f);
	ImGui::SliderFloat("m_aspect", &m_aspect, 1.0f, 3.0f);*/
}

} // namespace kusk