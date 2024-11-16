#pragma once

#include <directxtk/SimpleMath.h>

namespace kusk {
using DirectX::SimpleMath::Matrix;
using DirectX::SimpleMath::Vector3;

class Camera
{
public:
	Camera( ) { UpdateViewDir( ); }
	Matrix GetViewRow( );
	Matrix GetProjRow( );
	Vector3 GetEyePos( );

	void UpdateViewDir( ) {
		// 이동할 때 기준이 되는 정면/오른쪽 방향 계산
		m_viewDir = Vector3::Transform(Vector3(0.0f, 0.0f, 1.0f),
									   Matrix::CreateRotationY(this->m_yaw));
		m_rightDir = m_upDir.Cross(m_viewDir);
	}

	void UpdateMouse(float mouseNdcX, float mouseNdcY);
	void MoveForward(float dt);
	void MoveRight(float dt);
	void MoveUp(float dt);
	void SetAspectRatio(float aspect);

private:
	Vector3 m_position = Vector3(-1.28343f, 0.501636f, 1.64959f);
	Vector3 m_viewDir = Vector3(0.0f, 0.0f, 1.0f);
	Vector3 m_upDir = Vector3(0.0f, 1.0f, 0.0f);
	Vector3 m_rightDir = Vector3(1.0f, 0.0f, 0.0f);

	// roll, pitch, yaw
	float m_pitch = -0.135263f;
	float m_yaw = 1.37445f;

	float m_speed = 1.0f;

	// 프로젝션 옵션 : 카메라 클래스로 이동
	float m_projFovAngleY = 90.0f;
	float m_nearZ = 0.01f;
	float m_farZ = 100.0f;
	float m_aspect = 16.0f / 9.0f;
	bool m_usePerspectiveProjection = true;
};
} // namespace kusk


