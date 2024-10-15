#pragma once

#include <directxtk/SimpleMath.h>
#include <vector>
#include <string>

#include "Vertex.h"
#include "MeshData.h"

namespace kusk {

class GeometryGenerator
{
public:
	static vector<MeshData> ReadFromFile(std::string basePath,
										 std::string filename);
	static MeshData MakeSquare( );
	static MeshData MakeBox(const float scale = 1.0f);
	static MeshData MakeGrid(const float width, const float height,
							const int numSlices, const int numStacks);
	static MeshData MakeCylinder(const float bottomRadius,
								 const float topRadius, float height,
								 int numSlices);
	static MeshData MakeSphere(const float radius, const int numSlices,
							   const int numStacks);
	static MeshData MakeTetrahedron( ); // 사면체
	static MeshData MakeIcosahedron( ); // 이십면체
	static MeshData SubdivideToSphere(const float radius, MeshData meshData, bool faceNormal=false);
};

}


