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
		if (keyPressed[ 'E' ])
			MoveUp(dt);
		if (keyPressed[ 'Q' ])
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

rapidjson::Value Camera::CameraDataToJSON(rapidjson::Document::AllocatorType& allocator) {
	rapidjson::Value value(rapidjson::kObjectType);

	value.AddMember("position", JsonManager::Vector3ToJson(m_position, allocator), allocator);
	value.AddMember("pitch", m_pitch, allocator);
	value.AddMember("yaw", m_yaw, allocator);
	value.AddMember("speed", m_speed, allocator);
	value.AddMember("proj_fov_angle_y", m_projFovAngleY, allocator);
	value.AddMember("near_z", m_nearZ, allocator);
	value.AddMember("far_z", m_farZ, allocator);

	return value;
}

void Camera::CameraDataFromJSON(rapidjson::Value& value) {
	if (value.HasMember("position")) {
		m_position = JsonManager::ParseVector3(value[ "position" ]);
	}
	if (value.HasMember("pitch")) {
		m_pitch = value[ "pitch" ].GetFloat( );
	}
	if (value.HasMember("yaw")) {
		m_yaw = value[ "yaw" ].GetFloat( );
	}
	if (value.HasMember("speed")) {
		m_speed = value[ "speed" ].GetFloat( );
	}
	if (value.HasMember("proj_fov_angle_y")) {
		m_projFovAngleY = value[ "proj_fov_angle_y" ].GetFloat( );
	}
	if (value.HasMember("near_z")) {
		m_nearZ = value[ "near_z" ].GetFloat( );
	}
	if (value.HasMember("far_z")) {
		m_farZ = value[ "far_z" ].GetFloat( );
	}

	UpdateViewDir( );
}

} // namespace kusk