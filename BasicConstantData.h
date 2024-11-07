#pragma once

#include <directxtk/SimpleMath.h>

#include "Light.h"
#include "Material.h"

namespace kusk {

using DirectX::SimpleMath::Matrix;
using DirectX::SimpleMath::Vector3;

struct BasicVertexConstantData {
	Matrix model;
	Matrix invTranspose;
	Matrix view;
	Matrix proj;
};

static_assert((sizeof(BasicVertexConstantData) % 16) == 0,
	"Constant Buffer size must be 16-byte aligned");

struct BasicPixelConstantData {
	Vector3 eyeWorld;		// 12
	bool useTexture = true;		// bool 1 + 3 padding
	Material material;		// 48
	Light lights[ MAX_LIGHTS ]; // 48 * MAX_LIGHTS
	Vector3 rimColor = Vector3(1.0f);
	float rimPower;
	float rimStrength = 0.0f;
	bool useSmoothstep = false;
	float dummy[ 2 ];
};

static_assert((sizeof(BasicPixelConstantData) % 16) == 0,
	"Constant Buffer size must be 16-byte aligned");

struct NormalVertexConstantData {
	float scale = 0.1f;
	float dummy[ 3 ];
};

} // namespace kusk