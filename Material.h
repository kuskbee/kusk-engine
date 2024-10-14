#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxtk/SimpleMath.h>
#include <memory>

#include "Mesh.h"

namespace kusk {

using DirectX::SimpleMath::Matrix;
using DirectX::SimpleMath::Vector3;

struct Material {
	Vector3 ambient = Vector3(0.0f);	// 12
	float shininess = 256.0f;			// 4
	Vector3 diffuse = Vector3(0.0f);	// 12
	float dummy1;						// 4
	Vector3 specular = Vector3(1.0f);	// 12
	float dummy2;						// 4
};

} // namespace kusk