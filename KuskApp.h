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
#include "BillboardPoints.h"

namespace kusk {

using DirectX::SimpleMath::Vector3;

class KuskApp : public AppBase
{
public:
	KuskApp();

	virtual bool Initialize() override;
	virtual void UpdateGUI() override;
	virtual void UpdatePopupGUI() override;
	virtual void Update(float dt) override;
	virtual void Render() override;

	void UpdateLights(float dt);
	void UpdateMousePicking( );

	// ImGui Popup 관련
	virtual void UpdateObjectCreationFrameGUI() override;

	// 물체 생성 관련
	void CreateSphere(float radius, int numSlices, int numStacks, Vector2& texScale);
	void CreateSquare(float scale, Vector2& texScale, bool isMirror);
	void CreateSquareGrid(int numSlices, int numStacks, float scale, Vector2& texScale);
	void CreateCylinder(float bottomRadius, float topRadius, float height, int numSlices);
	void CreateBox(float scale);
	void CreateModelFromFile(const std::string& fullPath);
	void CreateBillboardPoints();

	// Json 관련
	void LoadSceneDataFromJSON(std::string& filePath);
	void SaveSceneDataAsJSON(std::string& filePath);

	rapidjson::Value LightToJSON(Light& l, rapidjson::Document::AllocatorType& allocator);
	rapidjson::Value EnvDataToJSON(rapidjson::Document::AllocatorType& allocator);
	rapidjson::Value ScreenDataToJSON(rapidjson::Document::AllocatorType& allocator);
	rapidjson::Value PostEffectDataToJSON(rapidjson::Document::AllocatorType& allocator);
	rapidjson::Value PostProcessingDataToJSON(rapidjson::Document::AllocatorType& allocator);
	void MirrorDataToJSON(shared_ptr<Model>& model, rapidjson::Value& value, rapidjson::Document::AllocatorType& allocator);

	void LightFromJSON(rapidjson::Value& lightData, Light& l);
	void EnvDataFromJSON(rapidjson::Value& value);
	void PostEffectDataFromJSON(rapidjson::Value& value);
	void PostProcessingDataFromJSON(rapidjson::Value& value);
	void ScreenDataFromJSON(rapidjson::Value& value);
	void MirrorDataFromJSON(rapidjson::Value& value);
	
protected: 
	//TessellatedQuad m_tessellatedQuad;
	shared_ptr<Model> m_ground;
	shared_ptr<Model> m_lightSphere[MAX_LIGHTS];
	shared_ptr<Model> m_skybox;
	shared_ptr<Model> m_cursorSphere;
	shared_ptr<Model> m_screenSquare;
	vector<shared_ptr<BillboardPoints>> m_billboardPointsList;

	// 거울
	shared_ptr<Model> m_mirror;
	DirectX::SimpleMath::Plane m_mirrorPlane;
	float m_mirrorAlpha = 1.0f; // opacity
	Vector3 m_mirrorRotation = Vector3(0.0f, 0.0f, 0.0f);
	Vector3 m_mirrorScale = Vector3(1.0f);

	// 거울이 아닌 물체들의 리스트 (for문으로 그리기 위함)
	vector<shared_ptr<Model>> m_basicList;

	// 장면으로 저장되는 물체 리스트
	vector<shared_ptr<Model>> m_savedList;

	// Billboard 생성용 파라미터
	bool m_showBillboardEditPopup = false;
	vector<std::string> m_billboardTextureList;

};
} // namespace kusk
 