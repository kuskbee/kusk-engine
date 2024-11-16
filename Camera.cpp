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

void Camera::UpdateMouse(float mouseNdcX, float mouseNdcY) {
	// 얼마나 회전할지 계산
	m_yaw = mouseNdcX * DirectX::XM_2PI; // 좌우 360도
	m_pitch = mouseNdcY * DirectX::XM_PIDIV2; // 위 아래 90도

	UpdateViewDir( );
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

}