#include "GeometryGenerator.h"

namespace kusk {

using namespace std;

MeshData GeometryGenerator::MakeSquare( ) {
	vector<Vector3> positions;
	vector<Vector3> colors;
	vector<Vector3> normals;
	vector<Vector2> texcoords; // 텍스쳐 좌표

	const float scale = 1.0f;

	positions.push_back(Vector3(-1.0f, 1.0f, 0.0f) * scale);
	positions.push_back(Vector3(1.0f, 1.0f, 0.0f) * scale);
	positions.push_back(Vector3(1.0f, -1.0f, 0.0f) * scale);
	positions.push_back(Vector3(-1.0f, -1.0f, 0.0f) * scale);
	colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
	colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
	colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
	colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
	normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
	normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
	normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
	normals.push_back(Vector3(0.0f, 0.0f, -1.0f));

	texcoords.push_back(Vector2(0.0f, 0.0f));
	texcoords.push_back(Vector2(1.0f, 0.0f));
	texcoords.push_back(Vector2(1.0f, 1.0f));
	texcoords.push_back(Vector2(0.0f, 1.0f));

	MeshData meshData;

	for (size_t i = 0; i < positions.size( ); i++) {
		Vertex v;
		v.position = positions[ i ];
		v.normal = normals[ i ];
		v.texcoord = texcoords[ i ];
		meshData.vertices.push_back(v);
	}

	meshData.indices = {
		0, 1, 2, 0, 2, 3,
	};

	return meshData;
}

MeshData GeometryGenerator::MakeBox( ) {
	vector<Vector3> positions;
	vector<Vector3> colors;
	vector<Vector3> normals;
	vector<Vector2> texcoords; // 텍스쳐 좌표

	const float scale = 1.0f;

	const Vector3 v0 = Vector3(-1.0f, 1.0f, -1.0f);
	const Vector3 v1 = Vector3(-1.0f, 1.0f, 1.0f);
	const Vector3 v2 = Vector3(1.0f, 1.0f, 1.0f);
	const Vector3 v3 = Vector3(1.0f, 1.0f, -1.0f);
	const Vector3 v4 = Vector3(-1.0f, -1.0f, -1.0f);
	const Vector3 v5 = Vector3(-1.0f, -1.0f, 1.0f);
	const Vector3 v6 = Vector3(1.0f, -1.0f, 1.0f);
	const Vector3 v7 = Vector3(1.0f, -1.0f, -1.0f);

	// 윗면
	positions.push_back(v0 * scale);
	positions.push_back(v1 * scale);
	positions.push_back(v2 * scale);
	positions.push_back(v3 * scale);
	colors.push_back(Vector3(1.0f, 0.0f, 0.0f));
	colors.push_back(Vector3(1.0f, 0.0f, 0.0f));
	colors.push_back(Vector3(1.0f, 0.0f, 0.0f));
	colors.push_back(Vector3(1.0f, 0.0f, 0.0f));
	normals.push_back(Vector3(0.0f, 1.0f, 0.0f));
	normals.push_back(Vector3(0.0f, 1.0f, 0.0f));
	normals.push_back(Vector3(0.0f, 1.0f, 0.0f));
	normals.push_back(Vector3(0.0f, 1.0f, 0.0f));
	texcoords.push_back({ 0.0f, 1.0f });
	texcoords.push_back({ 0.0f, 0.0f });
	texcoords.push_back({ 1.0f, 0.0f });
	texcoords.push_back({ 1.0f, 1.0f });

	// 아랫면
	positions.push_back(v4 * scale);
	positions.push_back(v7 * scale);
	positions.push_back(v6 * scale);
	positions.push_back(v5 * scale);
	colors.push_back(Vector3(0.0f, 1.0f, 0.0f));
	colors.push_back(Vector3(0.0f, 1.0f, 0.0f));
	colors.push_back(Vector3(0.0f, 1.0f, 0.0f));
	colors.push_back(Vector3(0.0f, 1.0f, 0.0f));
	normals.push_back(Vector3(0.0f, -1.0f, 0.0f));
	normals.push_back(Vector3(0.0f, -1.0f, 0.0f));
	normals.push_back(Vector3(0.0f, -1.0f, 0.0f));
	normals.push_back(Vector3(0.0f, -1.0f, 0.0f));
	texcoords.push_back({ 0.0f, 1.0f });
	texcoords.push_back({ 0.0f, 0.0f });
	texcoords.push_back({ 1.0f, 0.0f });
	texcoords.push_back({ 1.0f, 1.0f });

	// 앞면
	positions.push_back(v4 * scale);
	positions.push_back(v0 * scale);
	positions.push_back(v3 * scale);
	positions.push_back(v7 * scale);
	colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
	colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
	colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
	colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
	normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
	normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
	normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
	normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
	texcoords.push_back({ 0.0f, 1.0f });
	texcoords.push_back({ 0.0f, 0.0f });
	texcoords.push_back({ 1.0f, 0.0f });
	texcoords.push_back({ 1.0f, 1.0f });

	// 뒷면
	positions.push_back(v5 * scale);
	positions.push_back(v6 * scale);
	positions.push_back(v2 * scale);
	positions.push_back(v1 * scale);
	colors.push_back(Vector3(0.0f, 1.0f, 1.0f));
	colors.push_back(Vector3(0.0f, 1.0f, 1.0f));
	colors.push_back(Vector3(0.0f, 1.0f, 1.0f));
	colors.push_back(Vector3(0.0f, 1.0f, 1.0f));
	normals.push_back(Vector3(0.0f, 0.0f, 1.0f));
	normals.push_back(Vector3(0.0f, 0.0f, 1.0f));
	normals.push_back(Vector3(0.0f, 0.0f, 1.0f));
	normals.push_back(Vector3(0.0f, 0.0f, 1.0f));
	texcoords.push_back({ 0.0f, 1.0f });
	texcoords.push_back({ 0.0f, 0.0f });
	texcoords.push_back({ 1.0f, 0.0f });
	texcoords.push_back({ 1.0f, 1.0f });

	// 왼쪽
	positions.push_back(v5 * scale);
	positions.push_back(v1 * scale);
	positions.push_back(v0 * scale);
	positions.push_back(v4 * scale);
	colors.push_back(Vector3(1.0f, 1.0f, 0.0f));
	colors.push_back(Vector3(1.0f, 1.0f, 0.0f));
	colors.push_back(Vector3(1.0f, 1.0f, 0.0f));
	colors.push_back(Vector3(1.0f, 1.0f, 0.0f));
	normals.push_back(Vector3(-1.0f, 0.0f, 0.0f));
	normals.push_back(Vector3(-1.0f, 0.0f, 0.0f));
	normals.push_back(Vector3(-1.0f, 0.0f, 0.0f));
	normals.push_back(Vector3(-1.0f, 0.0f, 0.0f));
	texcoords.push_back({ 0.0f, 1.0f });
	texcoords.push_back({ 0.0f, 0.0f });
	texcoords.push_back({ 1.0f, 0.0f });
	texcoords.push_back({ 1.0f, 1.0f });

	// 오른쪽
	positions.push_back(v6 * scale);
	positions.push_back(v7 * scale);
	positions.push_back(v3 * scale);
	positions.push_back(v2 * scale);
	colors.push_back(Vector3(1.0f, 0.0f, 1.0f));
	colors.push_back(Vector3(1.0f, 0.0f, 1.0f));
	colors.push_back(Vector3(1.0f, 0.0f, 1.0f));
	colors.push_back(Vector3(1.0f, 0.0f, 1.0f));
	normals.push_back(Vector3(1.0f, 0.0f, 0.0f));
	normals.push_back(Vector3(1.0f, 0.0f, 0.0f));
	normals.push_back(Vector3(1.0f, 0.0f, 0.0f));
	normals.push_back(Vector3(1.0f, 0.0f, 0.0f));
	texcoords.push_back({ 0.0f, 1.0f });
	texcoords.push_back({ 0.0f, 0.0f });
	texcoords.push_back({ 1.0f, 0.0f });
	texcoords.push_back({ 1.0f, 1.0f });

	MeshData meshData;
	for (size_t i = 0; i < positions.size( ); i++) {
		Vertex v;
		v.position = positions[ i ];
		v.normal = normals[ i ];
		v.texcoord = texcoords[ i ];
		meshData.vertices.push_back(v);
	}

	meshData.indices = {
		0, 1, 2, 0, 2, 3, // 윗면
		4, 5, 6, 4, 6, 7, // 아랫면
		8, 9, 10, 8, 10, 11, // 앞면
		12, 13, 14, 12, 14, 15, // 뒷면
		16, 17, 18, 16, 18, 19, // 왼쪽
		20, 21, 22, 20, 22, 23 // 오른쪽
	};

	return meshData;
}

MeshData GeometryGenerator::MakeCylinder(const float bottomRadius,
										 const float topRadius, float height,
										 int sliceCount) {

	MeshData meshData;

	return meshData;
}

}