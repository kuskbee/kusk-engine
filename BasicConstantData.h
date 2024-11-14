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
	float rimStrength = 0.0f;			// 4
	bool useSmoothstep = false;			// 4
	int useAlbedoMap = 0;				// 4
	int useNormalMap = 0;				// 4
	int useAOMap = 0;					// 4
	int invertNormalMapY = 0;			// 4
	int useMetallicMap = 0;				// 4
	int useRoughnessMap = 0;			// 4
	float expose = 1.0f;				// 4
	float gamma = 1.0f;					// 4
	float dummy1;
	float dummy2;
};

struct NormalVertexConstantData {
	float scale = 0.1f;
	float dummy[ 3 ];
};

} // namespace kusk