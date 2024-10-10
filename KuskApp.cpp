#include "KuskApp.h"

#include <tuple>
#include <vector>

namespace kusk {
using namespace std;

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
	colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
	colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
	colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
	colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
	normals.push_back(Vector3(0.0f, -1.0f, 0.0f));
	normals.push_back(Vector3(0.0f, -1.0f, 0.0f));
	normals.push_back(Vector3(0.0f, -1.0f, 0.0f));
	normals.push_back(Vector3(0.0f, -1.0f, 0.0f));

	// 앞면
	positions.push_back(v4 * scale);
	positions.push_back(v0 * scale);
	positions.push_back(v3 * scale);
	positions.push_back(v7 * scale);
	colors.push_back(Vector3(1.0f, 1.0f, 0.0f));
	colors.push_back(Vector3(1.0f, 1.0f, 0.0f));
	colors.push_back(Vector3(1.0f, 1.0f, 0.0f));
	colors.push_back(Vector3(1.0f, 1.0f, 0.0f));
	normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
	normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
	normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
	normals.push_back(Vector3(0.0f, 0.0f, -1.0f));

	// 뒷면
	positions.push_back(v5 * scale);
	positions.push_back(v6 * scale);
	positions.push_back(v2 * scale);
	positions.push_back(v1 * scale);
	colors.push_back(Vector3(1.0f, 0.0f, 1.0f));
	colors.push_back(Vector3(1.0f, 0.0f, 1.0f));
	colors.push_back(Vector3(1.0f, 0.0f, 1.0f));
	colors.push_back(Vector3(1.0f, 0.0f, 1.0f));
	normals.push_back(Vector3(0.0f, 0.0f, 1.0f));
	normals.push_back(Vector3(0.0f, 0.0f, 1.0f));
	normals.push_back(Vector3(0.0f, 0.0f, 1.0f));
	normals.push_back(Vector3(0.0f, 0.0f, 1.0f));

	// 왼쪽
	positions.push_back(v5 * scale);
	positions.push_back(v1 * scale);
	positions.push_back(v0 * scale);
	positions.push_back(v4 * scale);
	colors.push_back(Vector3(0.0f, 1.0f, 1.0f));
	colors.push_back(Vector3(0.0f, 1.0f, 1.0f));
	colors.push_back(Vector3(0.0f, 1.0f, 1.0f));
	colors.push_back(Vector3(0.0f, 1.0f, 1.0f));
	normals.push_back(Vector3(-1.0f, 0.0f, 0.0f));
	normals.push_back(Vector3(-1.0f, 0.0f, 0.0f));
	normals.push_back(Vector3(-1.0f, 0.0f, 0.0f));
	normals.push_back(Vector3(-1.0f, 0.0f, 0.0f));

	// 오른쪽
	positions.push_back(v6 * scale);
	positions.push_back(v7 * scale);
	positions.push_back(v3 * scale);
	positions.push_back(v2 * scale);
	colors.push_back(Vector3(0.0f, 1.0f, 0.0f));
	colors.push_back(Vector3(0.0f, 1.0f, 0.0f));
	colors.push_back(Vector3(0.0f, 1.0f, 0.0f));
	colors.push_back(Vector3(0.0f, 1.0f, 0.0f));
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
	auto [vertices, indices] = MakeBox();

	// Vertex Buffer
	AppBase::CreateVertexBuffer(vertices, m_vertexBuffer);

	// Index Buffer
	m_indexCount = UINT(indices.size());
	AppBase::CreateIndexBuffer(indices, m_indexBuffer);

	// Constant Buffer
	m_constantBufferData.model = Matrix();
	m_constantBufferData.view = Matrix();
	m_constantBufferData.proj = Matrix();
	AppBase::CreateConstantBuffer(m_constantBufferData, m_constantBuffer);

	// Make Shaders
	vector<D3D11_INPUT_ELEMENT_DESC> inputElements = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 4 * 3, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	AppBase::CreateVertexShaderAndInputLayout(L"ColorVertexShader.hlsl", inputElements,
												m_colorVertexShader, m_colorInputLayout);

	AppBase::CreatePixelShader(L"ColorPixelShader.hlsl", m_colorPixelShader);

	return true;
}

void KuskApp::Update(float dt) {
	static float rot = 0.0f;
	rot += dt;

	// 모델의 변환
	m_constantBufferData.model = Matrix::CreateScale(0.5f) * Matrix::CreateRotationY(rot) *
								 Matrix::CreateTranslation(Vector3(0.0f, -0.3f, 1.0f));
	m_constantBufferData.model = m_constantBufferData.model.Transpose();

	using namespace DirectX;

	// 시점 변환
	m_constantBufferData.view =
		XMMatrixLookAtLH({ 0.0f, 0.0f, -1.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f });
	m_constantBufferData.view = m_constantBufferData.view.Transpose();

	// 프로젝션
	const float aspect = AppBase::GetAspectRatio();
	if (m_usePerspectiveProjection) {
		const float fovAngleY = 70.0f * XM_PI / 180.0f;
		m_constantBufferData.proj =
			XMMatrixPerspectiveFovLH(fovAngleY, aspect, 0.01f, 100.0f);
	}
	else {
		m_constantBufferData.proj =
			XMMatrixOrthographicOffCenterLH(-aspect, aspect, -1.0f, 1.0f, 0.1f, 10.0f);
	}
	m_constantBufferData.proj = m_constantBufferData.proj.Transpose();

	// Constant를 CPU에서 GPU로복사
	AppBase::UpdateBuffer(m_constantBufferData, m_constantBuffer);
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
	m_context->VSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf());
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
	ImGui::Checkbox("usePerspectiveProjection", &m_usePerspectiveProjection);
}

} // namespace kusk