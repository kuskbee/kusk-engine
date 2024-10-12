#pragma once

#include <directxtk/SimpleMath.h>
#include <vector>

namespace kusk {

using DirectX::SimpleMath::Vector2;
using DirectX::SimpleMath::Vector3;

struct Vertex {
	Vector3 position;
	Vector3 normal;
	Vector2 texcoord;
};

struct MeshData {
	std::vector<Vertex> vertices;
	std::vector<uint16_t> indices;
};

class GeometryGenerator
{
public:
	static MeshData MakeSquare( );
	static MeshData MakeBox( );
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


