#pragma once

#include <directxtk/SimpleMath.h>

#include "Light.h"
#include "Material.h"

namespace kusk {

using DirectX::SimpleMath::Matrix;
using DirectX::SimpleMath::Vector2;
using DirectX::SimpleMath::Vector3;

struct BasicVertexConstData {
	Matrix modelWorld;
	Matrix invTranspose;
	Matrix view;
	Matrix proj;
	int useHeightMap = 0;
	float heightScale = 0.0f;
	Vector2 dummy;
};

struct BasicPixelConstData {
	Vector3 eyeWorld;					// 12
	float mipmapLevel = 0.0f;			//4
	Material material;					// 48
	Light lights[ MAX_LIGHTS ];			// 48 * MAX_LIGHTS
	Vector3 rimColor = Vector3(1.0f);	// 12
	float rimPower;						// 4
	float rimStrength = 0.0f;
	bool useSmoothstep = false;
	int useAlbedoMap = 0;
	int useNormalMap = 0;				// 16
	int useAOMap = 0;
	int invertNormalMapY = 0;
	int useMetallicMap = 0;
	int useRoughnessMap = 0;			// 16
	int useEmissiveMap = 0;
	float expose = 1.0f;
	float gamma = 1.0f;
	float dummy1;						// 16
};

struct NormalVertexConstantData {
	float scale = 0.1f;
	float dummy[ 3 ];
};

} // namespace kusk