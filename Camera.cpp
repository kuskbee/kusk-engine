#include "Camera.h"

#include <iostream>

namespace kusk {

using namespace std;
using namespace DirectX;

Matrix Camera::GetViewRow( ) {
	return Matrix::CreateTranslation(-this->m_position) *
		   Matrix::CreateRotationY(-this->m_yaw) * 
		   Matrix::CreateRotationX(this->m_pitch);
}

Vector3 Camera::GetEyePos( ) { return m_position; }

void Camera::UpdateViewDir( ) {
	// 이동할 때 기준이 되는 정면/오른쪽 방향 계산
	m_viewDir = Vector3::Transform(Vector3(0.0f, 0.0f, 1.0f),
								   Matrix::CreateRotationY(this->m_yaw));
	m_rightDir = m_upDir.Cross(m_viewDir);
}

void Camera::UpdateKeyboard(const float dt, bool const keyPressed[ 256 ]) {
	if (m_useFirstPersonView) {
		if (keyPressed[ 'W' ])
			MoveForward(dt);
		if (keyPressed[ 'S' ])
			MoveForward(-dt);
		if (keyPressed[ 'D' ])
			MoveRight(dt);
		if (keyPressed[ 'A' ])
			MoveRight(-dt);
		if (keyPressed[ 'Q' ])
			MoveUp(dt);
		if (keyPressed[ 'E' ])
			MoveUp(-dt);
	}
}

void Camera::UpdateMouse(float mouseNdcX, float mouseNdcY) {
	if (m_useFirstPersonView) {
		// 얼마나 회전할지 계산
		m_yaw = mouseNdcX * DirectX::XM_2PI; // 좌우 360도
		m_pitch = mouseNdcY * DirectX::XM_PIDIV2; // 위 아래 90도

		UpdateViewDir( );
	}
}

void Camera::MoveForward(float dt) {
	// 이동 후의 위치 = 현재 위치 + 이동방향 * 속도 * 시간 차이
	m_position += m_viewDir * m_speed * dt;
}

void Camera::MoveUp(float dt) { m_position += m_upDir * m_speed * dt; }

void Camera::MoveRight(float dt) { m_position += m_rightDir * m_speed * dt; }

void Camera::SetAspectRatio(float aspect) { m_aspect = aspect; }

Matrix Camera::GetProjRow( ) {
	return m_usePerspectiveProjection 
		? XMMatrixPerspectiveFovLH(XMConvertToRadians(m_projFovAngleY), m_aspect, m_nearZ, m_farZ)
		: XMMatrixOrthographicOffCenterLH(-m_aspect, m_aspect, -1.0f, 1.0f, m_nearZ, m_farZ);
}

} // namespace kusk