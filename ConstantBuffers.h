#pragma once

#include <directxtk/SimpleMath.h>

// "Common.hlsli"와 동일해야 함
#define MAX_LIGHTS 3
#define LIGHT_OFF 0x00
#define LIGHT_DIRECTIONAL 0x01
#define LIGHT_POINT 0x02
#define LIGHT_SPOT 0x04
#define LIGHT_SHADOW 0x10

namespace kusk {

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

	
	// Rim 관련 데이터
	float rimPower = 0.01f;
	Vector3 rimColor = Vector3(1.0f);
	float rimStrength = 0.0f;
	bool useSmoothstep = false;
	Vector3 dummy = Vector3(0.0f);

};

// 조명
struct Light {
	Vector3 radiance = Vector3(5.0f); // strength
	float fallOffStart = 0.0f;
	Vector3 direction = Vector3(0.0f, 0.0f, 1.0f);
	float fallOffEnd = 20.0f;
	Vector3 position = Vector3(0.0f, 0.0f, -2.0f);
	float spotPower = 6.0f;

	// Light type bitmasking
	// ex) LIGHT_SPOT | LIGHT_SHADOW
	uint32_t type = LIGHT_OFF;
	float radius = 0.0f; // 반지름
	
	float haloRadius = 2.0f;
	float haloStrength = 1.0f;

	Matrix viewProj;	// 그림자 렌더링에 필요
	Matrix invProj;		// 그림자 렌더링 디버깅용
};

// register(b1) 사용
__declspec(align(256)) struct GlobalConstants {
	Matrix view;
	Matrix proj;
	Matrix invProj;
	Matrix viewProj;
	Matrix invViewProj;	// Proj -> World
	Vector3 eyeWorld;
	float strengthIBL = 0.0f;
	int textureToDraw = 0;	// 0 : Env, 1 : Specular, 2 : Irradiance, 그외 : 검은색
	float envLodBias = 0.0f;	// 환경맵 LodBias
	float lodBias = 2.0f;		// 다른 물체들 LoadBias
	// 거울 관련 데이터
	bool isMirror = false;
	Vector4 mirrorPlane;

	Light lights[ MAX_LIGHTS ];
};

// register(b3) 사용, PostEffectsPS.hlsl
__declspec(align(256)) struct PostEffectsConstants {
	int mode = 1; // 1: Rendered image, 2: DepthOnly
	float depthScale = 1.0f;
	float fogStrength = 0.0f;
};

} // namespace kusk