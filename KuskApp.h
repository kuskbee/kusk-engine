#pragma once

#include <algorithm>
#include <directxtk/SimpleMath.h>
#include <iostream>
#include <memory>

#include "AppBase.h"
#include "BasicMeshGroup.h"
#include "BillboardPoints.h"
#include "CubeMapping.h"
#include "GeometryGenerator.h"
#include "ImageFilter.h"
#include "Light.h"
#include "TessellatedQuad.h"

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

protected: 
	//TessellatedQuad m_tessellatedQuad;
	//BillboardPoints m_billboardPoints;
	shared_ptr<BasicMeshGroup> m_ground;
	shared_ptr<BasicMeshGroup> m_mainObj;
	shared_ptr<BasicMeshGroup> m_lightSphere;
	BasicMeshGroup m_box;
	BoundingSphere m_mainBoundingSphere;	//
	BasicMeshGroup m_cursorSphere;			//

	//BasicMeshGroup m_meshGroupCharacter;

	CubeMapping m_cubeMapping;

	/*Vector3 m_modelTranslation = Vector3(0.0f, 0.2f, 0.6f);
	Vector3 m_modelRotation = Vector3(0.0f, 0.0f, 0.0f);
	Vector3 m_modelScaling = Vector3(1.8f);*/

	int m_lightType = 0;
	Light m_light;

	// 거울
	shared_ptr<BasicMeshGroup> m_mirror;
	DirectX::SimpleMath::Plane m_mirrorPlane;
	float m_mirrorAlpha = 0.5f; // opacity

	// 거울이 아닌 물체들의 리스트 (for문으로 그리기 위함)
	vector<shared_ptr<BasicMeshGroup>> m_basicList;
	
	//int m_visibleMeshIndex = 0; // Sphere, Character

};
} // namespace kusk
