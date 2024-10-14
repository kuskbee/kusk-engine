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
	std::string textureFilename;
};

} // namespace kusk