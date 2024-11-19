#pragma once

#include <algorithm>
#include <directxtk/SimpleMath.h>
#include <iostream>
#include <memory>

#include "AppBase.h"
#include "GeometryGenerator.h"
#include "ImageFilter.h"
#include "Model.h"

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
	shared_ptr<Model> m_ground;
	shared_ptr<Model> m_mainObj;
	shared_ptr<Model> m_lightSphere[MAX_LIGHTS];
	shared_ptr<Model> m_box;
	shared_ptr<Model> m_skybox;
	shared_ptr<Model> m_cursorSphere;
	shared_ptr<Model> m_screenSquare;

	BoundingSphere m_mainBoundingSphere;

	//BasicMeshGroup m_meshGroupCharacter;

	// 거울
	shared_ptr<Model> m_mirror;
	DirectX::SimpleMath::Plane m_mirrorPlane;
	float m_mirrorAlpha = 1.0f; // opacity

	// 거울이 아닌 물체들의 리스트 (for문으로 그리기 위함)
	vector<shared_ptr<Model>> m_basicList;
};
} // namespace kusk
