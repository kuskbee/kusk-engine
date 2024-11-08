#pragma once

#include <algorithm>
#include <iostream>
#include <memory>

#include "AppBase.h"
#include "BasicMeshGroup.h"
#include "CubeMapping.h"
#include "GeometryGenerator.h"
#include "ImageFilter.h"
#include "Light.h"

namespace kusk {

using DirectX::BoundingSphere;
using DirectX::SimpleMath::Vector3;

class KuskApp : public AppBase
{
public:
	KuskApp();

	virtual bool Initialize() override;
	virtual void UpdateGUI() override;
	virtual void Update(float dt) override;
	virtual void Render() override;

	void BuildFilters( );

protected:
	BasicMeshGroup m_mainSphere;
	BoundingSphere m_mainBoundingSphere;	//
	BasicMeshGroup m_cursorSphere;			//

	BasicMeshGroup m_meshGroupCharacter;
	BasicMeshGroup m_meshGroupGround;
	CubeMapping m_cubeMapping;

	/*Vector3 m_modelTranslation = Vector3(0.0f, 0.2f, 0.6f);
	Vector3 m_modelRotation = Vector3(0.0f, 0.0f, 0.0f);
	Vector3 m_modelScaling = Vector3(1.8f);*/

	int m_lightType = 0;
	Light m_lightFromGUI;
	float m_materialDiffuse = 1.0f;
	float m_materialSpecular = 1.0f;
	
	int m_visibleMeshIndex = 0; // Sphere, Character

	std::vector<shared_ptr<ImageFilter>> m_filters;

	// Bloom 효과 옵션
	int m_dirtyFlag = 1; // 처음 한 번만 실행
	int m_down = 16;
	int m_repeat = 5;
	float m_threshold = 0.0f;
	float m_strength = 0.8f;

};
} // namespace kusk
