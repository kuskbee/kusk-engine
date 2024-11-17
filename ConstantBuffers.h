#pragma once

#include <directxtk/SimpleMath.h>

#define MAX_LIGHTS 3

namespace kusk
{

using DirectX::SimpleMath::Matrix;
using DirectX::SimpleMath::Vector2;
using DirectX::SimpleMath::Vector3;
using DirectX::SimpleMath::Vector4;

// DirectX-Graphics-Samples/MiniEngine을 따라서 파일이름 변경
// __declspec(align(256)) : DX12에서는 256 align

// 주로 Vertex/Geometry 쉐이더에서 사용
__declspec(align(256)) struct MeshConstants {
	Matrix world;
	Matrix worldIT;
	int useHeightMap = 0;
	float heightScale = 0.0f;
	Vector2 dummy;
};

// 주로 Pixel 쉐이더에서 사용
__declspec(align(256)) struct MaterialConstants {
	Vector3 albedoFactor = Vector3(1.0f);
	float roughnessFactor = 1.0f;
	float metallicFactor = 1.0f;
	Vector3 emissionFactor = Vector3(0.0f);

	int useAlbedoMap = 0;
	int useNormalMap = 0;
	int useAOMap = 0;
	int invertNormalMapY = 0;
	int useMetallicMap = 0;
	int useRoughnessMap = 0;
	int useEmissiveMap = 0;
	float expose = 1.0f;
	float gamma = 1.0f;
	
	// Rim 관련 데이터
	Vector3 rimColor = Vector3(1.0f);
	float rimPower = 0.01f;
	float rimStrength = 0.0f;
	bool useSmoothstep = false;
	float dummy;

};

// 조명
struct Light {
	Vector3 radiance = Vector3(0.0f); // strength
	float fallOffStart = 0.0f;
	Vector3 direction = Vector3(0.0f, 0.0f, 1.0f);
	float fallOffEnd = 10.0f;
	Vector3 position = Vector3(0.0f, 0.0f, -2.0f);
	float spotPower = 100.0f;
};

// register(b1) 사용
__declspec(align(256)) struct GlobalConstants {
	Matrix view;
	Matrix proj;
	Matrix viewProj;
	Vector3 eyeWorld;
	float strengthIBL = 1.0f;
	int textureToDraw = 0;	// 0 : Env, 1 : Specular, 2 : Irradiance, 그외 : 검은색
	float envLodBias = 0.0f;	// 환경맵 LodBias
	float lodBias = 2.0f;		// 다른 물체들 LoadBias
	// 거울 관련 데이터
	bool isMirror = false;
	Vector4 mirrorPlane;

	Light lights[ MAX_LIGHTS ];
};

} // namespace kusk