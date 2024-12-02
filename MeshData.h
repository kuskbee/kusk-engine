#pragma once

#include <directxtk/SimpleMath.h>
#include <string>
#include <vector>

#include "Vertex.h"

namespace kusk {

using std::vector;

struct MeshData {
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	std::string albedoTextureFilename;
	std::string emissiveTextureFilename;
	std::string normalTextureFilename;
	std::string heightTextureFilename;
	std::string aoTextureFilename; // Ambient Occlusion
	std::string metallicTextureFilename;
	std::string roughnessTextureFilename;
};


// Model 생성에 필요한 파라미터
enum MESH_TYPE {
	MESH_TYPE_NONE = 0,
	MESH_TYPE_SPHERE,
	MESH_TYPE_SQUARE,
	MESH_TYPE_SQUARE_GRID,
	MESH_TYPE_CYLINDER,
	MESH_TYPE_BOX,
};

struct ModelCreationParams
{
	float scale = 1.0f;
	Vector2 tex_scale = Vector2(1.0f);
	int numSlices = 20;
	int numStacks = 20;
	float bottomRadius = 1.0f;
	float topRadius = 1.0f;
	float height = 1.0f;
	float radius = 1.0f;
	bool isMirror = false;
	MESH_TYPE type = MESH_TYPE_NONE;
	std::string selectedFilePath;
};

} // namespace kusk