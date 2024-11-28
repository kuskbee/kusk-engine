#pragma once

#include <algorithm>
#include <directxtk/SimpleMath.h>
#include <iostream>
#include <memory>

#include "AppBase.h"
#include "GeometryGenerator.h"
#include "ImageFilter.h"
#include "Model.h"
#include "JsonManager.h"

namespace kusk {

using DirectX::SimpleMath::Vector3;

class KuskApp : public AppBase
{
public:
	KuskApp();

	virtual bool Initialize() override;
	virtual void UpdateGUI() override;
	virtual void Update(float dt) override;
	virtual void Render() override;

	void UpdateLights(float dt);
	void UpdateMousePicking( );

	// ImGui Popup 관련
	virtual void UpdateObjectCreationFrameGUI() override;

	// 물체 생성 관련
	void CreateSphere(float radius, int numSlices, int numStacks, Vector2& texScale);
	void CreateSquare(float scale, Vector2& texScale);
	void CreateSquareGrid(int numSlices, int numStacks, float scale, Vector2& texScale);
	void CreateCylinder(float bottomRadius, float topRadius, float height, int numSlices);
	void CreateBox(float scale);
	void CreateModelFromFile(const std::string& fullPath);

	// Json 관련
	void LoadSceneDataAsJSON(std::string& filePath);
	void SaveSceneDataAsJSON(std::string& filePath);

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

	// 거울
	shared_ptr<Model> m_mirror;
	DirectX::SimpleMath::Plane m_mirrorPlane;
	float m_mirrorAlpha = 1.0f; // opacity

	// 거울이 아닌 물체들의 리스트 (for문으로 그리기 위함)
	vector<shared_ptr<Model>> m_basicList;

	// 장면으로 저장되는 물체 리스트
	vector<shared_ptr<Model>> m_savedList;
};
} // namespace kusk
 