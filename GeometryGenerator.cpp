#include "GeometryGenerator.h"

#include <iostream>

namespace kusk {

using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;

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

MeshData GeometryGenerator::MakeGrid(const float width, const float height,
									 const int numSlices, const int numStacks) {
	// x-y 평면 (z = 0) 위에 격자 구조로 평면 만들기

	const float dx = width / numSlices;
	const float dy = height / numStacks;

	MeshData meshData;

	vector<Vertex>& vertices = meshData.vertices;
	vector<uint16_t>& indices = meshData.indices;

	Vector3 leftbottom = Vector3(-0.5f * width, -0.5 * height, 0.0f);

	for (int j = 0; j <= numStacks; j++) {
		Vector3 stackStartPoint = Vector3::Transform(
			leftbottom, Matrix::CreateTranslation(Vector3(0.0f, j * dy, 0.0f)));

		for (int i = 0; i <= numSlices; i++) {
			Vertex v;

			// x-y 평면에서 시작점을 x 방향으로 이동
			v.position = Vector3::Transform(
				stackStartPoint,
				Matrix::CreateTranslation(Vector3(dx * float(i), 0.0f, 0.0f)));

			// 시점을 향하는 방향
			v.normal = Vector3(0.0f, 0.0f, -1.0f);

			v.texcoord =
				Vector2(float(i) / numSlices, 1.0f - float(j) / numStacks);

			vertices.push_back(v);

		}

		if (j > 0) {
			for (int i = 0; i < numSlices; i++) {
				int idx = (j - 1) * (numSlices + 1) + i;
				// 첫 번째 삼각형
				indices.push_back(idx);
				indices.push_back(idx + numSlices + 1);
				indices.push_back(idx + numSlices + 2);
				// 두 번째 삼각형
				indices.push_back(idx);
				indices.push_back(idx + numSlices + 2);
				indices.push_back(idx + 1);
			}
		}
	}

	return meshData;
}

MeshData GeometryGenerator::MakeCylinder(const float bottomRadius,
										 const float topRadius, float height,
										 int numSlices) {
	
	MeshData meshData;

	// Texture 좌표계 때문에 (sliceCount + 1) * 2 개의 vertex 사용

	const float dTheta = -XM_2PI / float(numSlices);

	vector<Vertex>& vertices = meshData.vertices;
	Vector3 center = Vector3(0.0f, -0.5f * height, 0.0f);
	Vector3 startPoint = Vector3(-bottomRadius, -0.5 * height, 0.0f);
	// 바닥 Vertex들 (index 가 0 이상 sliceCount 미만)
	for (int i = 0; i <= numSlices; i++) {
		Vertex v;

		v.position = Vector3::Transform(startPoint, Matrix::CreateRotationY(dTheta * float(i)));
		v.normal = v.position - center;
		v.normal.Normalize( );
		v.texcoord = Vector2(float(i) / numSlices, 1.0f);

		vertices.push_back(v);
	}

	// 천장 Vertex들 (index가 sliceCount 이상 sliceCount * 2 미만)
	startPoint = Vector3(-topRadius, 0.5 * height, 0.0f);
	center = Vector3(0.0f, 0.5f * height, 0.0f);
	for (int i = 0; i <= numSlices; i++) {
		Vertex v;

		v.position = Vector3::Transform(startPoint, Matrix::CreateRotationY(dTheta * float(i)));
		v.normal = v.position - center;
		v.normal.Normalize( );
		v.texcoord = Vector2(float(i) / numSlices, 0.0f);
		vertices.push_back(v);
	}

	vector<uint16_t>& indices = meshData.indices;

	for (int i = 0; i < numSlices; i++) {
		indices.push_back(i);
		indices.push_back(i + numSlices + 1);
		indices.push_back(i + numSlices + 2);
		
		indices.push_back(i + numSlices + 2);
		indices.push_back(i + 1);
		indices.push_back(i);
	}

	return meshData;
}

MeshData GeometryGenerator::MakeSphere(const float radius, const int numStacks, const int numSlices) {

	// Texture 좌표계때문에 (numSlices + 1) 개의 vertex 사용 (마지막에 닫아주는 vertex가 중복)
	// Stack은 y축 양의 방향으로 쌓아가는 방식

	const float dDelta = -XM_2PI / float(numSlices);
	const float dPhi = -XM_PI / float(numStacks);

	MeshData meshData;

	vector<Vertex>& vertices = meshData.vertices;

	for (int j = 0; j <= numStacks; j++) {
		// 스택에 쌓일 수록 시작점을 x-y평면에서 회전시켜서 올리는 구조
		Vector3 stackStartPoint = Vector3::Transform(
			Vector3(0.0f, -radius, 0.0f), Matrix::CreateRotationZ(dPhi * float(j)));

		for (int i = 0; i <= numSlices; i++) {
			Vertex v;

			// 시작점을 x-z평면에서 회전시키면서 원을 만드는 구조
			v.position = Vector3::Transform(
				stackStartPoint, Matrix::CreateRotationY(dDelta * float(i)));
			v.normal = v.position; // 원점이 구의 중심이므로
			v.normal.Normalize( );
			v.texcoord = Vector2(float(i) / numSlices, 1.0f - float(j) / numStacks);
			vertices.push_back(v);
		}
	}

	cout << vertices.size( ) << endl;

	vector<uint16_t>& indices = meshData.indices;

	for (int j = 0; j < numStacks; j++) {
		for (int i = 0; i < numSlices; i++) {
			int idx = j * (numSlices + 1) + i;
			indices.push_back(idx);
			indices.push_back(idx + numSlices + 1);
			indices.push_back(idx + numSlices + 2);

			indices.push_back(idx);
			indices.push_back(idx + numSlices + 2);
			indices.push_back(idx + 1);
		}
	}
	
	return meshData;
}

MeshData GeometryGenerator::MakeTetrahedron() {

	MeshData meshData;

	return meshData;
}

MeshData GeometryGenerator::MakeIcosahedron( ) {

	MeshData meshData;

	return meshData;
}

MeshData GeometryGenerator::SubdivideToSphere(const float radius, MeshData meshData) {

	return MeshData( );
}

} // namespace kusk