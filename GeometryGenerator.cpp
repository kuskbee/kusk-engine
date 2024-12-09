#include "GeometryGenerator.h"

#include "ModelLoader.h"

namespace kusk {

using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;

MeshData GeometryGenerator::MakeSquare(const float scale, const Vector2 texScale) {
	vector<Vector3> positions;
	vector<Vector3> colors;
	vector<Vector3> normals;
	vector<Vector2> texcoords; // 텍스쳐 좌표

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
		v.normalModel = normals[ i ];
		v.texcoord = texcoords[ i ] * texScale;
		v.tangentModel = Vector3(1.0f, 0.0f, 0.0f);
		meshData.vertices.push_back(v);
	}

	meshData.indices = {
		0, 1, 2, 0, 2, 3,
	};

	return meshData;
}

MeshData GeometryGenerator::MakeBox(const float scale) {
	vector<Vector3> positions;
	vector<Vector3> colors;
	vector<Vector3> normals;
	vector<Vector2> texcoords; // 텍스쳐 좌표

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
		v.normalModel = normals[ i ];
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

MeshData GeometryGenerator::MakeSquareGrid(const int numSlices, const int numStacks,
										   const float scale, const Vector2 texScale) {
	// x-y 평면 (z = 0) 위에 격자 구조로 평면 만들기

	//const float dx = width / numSlices;
	//const float dy = height / numStacks;
	const float dx = 2.0f / numSlices;
	const float dy = 2.0f / numStacks;


	MeshData meshData;

	vector<Vertex>& vertices = meshData.vertices;
	vector<uint32_t>& indices = meshData.indices;

	//Vector3 leftbottom = Vector3(-0.5f * width, -0.5f * height, 0.0f);
	Vector3 leftbottom = Vector3(-1.0f, -1.0f, 0.0f);

	for (int j = 0; j <= numStacks; j++) {
		Vector3 stackStartPoint = Vector3::Transform(
			leftbottom, Matrix::CreateTranslation(Vector3(0.0f, j * dy, 0.0f)));

		for (int i = 0; i <= numSlices; i++) {
			Vertex v;

			// x-y 평면에서 시작점을 x 방향으로 이동
			v.position = Vector3::Transform(stackStartPoint, 
				Matrix::CreateTranslation(Vector3(dx * float(i), 0.0f, 0.0f)));
			v.position *= scale;

			// 시점을 향하는 방향
			v.normalModel = Vector3(0.0f, 0.0f, -1.0f);

			v.texcoord =
				Vector2(float(i) / numSlices, 1.0f - float(j) / numStacks) * texScale;
			v.tangentModel = Vector3(1.0f, 0.0f, 0.0f);

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
	Vector3 startPoint = Vector3(-bottomRadius, -0.5f * height, 0.0f);
	// 바닥 Vertex들 (index 가 0 이상 sliceCount 미만)
	for (int i = 0; i <= numSlices; i++) {
		Vertex v;

		v.position = Vector3::Transform(startPoint, Matrix::CreateRotationY(dTheta * float(i)));
		v.normalModel = v.position - center;
		v.normalModel.Normalize( );
		v.texcoord = Vector2(float(i) / numSlices, 1.0f);

		vertices.push_back(v);
	}

	// 천장 Vertex들 (index가 sliceCount 이상 sliceCount * 2 미만)
	startPoint = Vector3(-topRadius, 0.5f * height, 0.0f);
	center = Vector3(0.0f, 0.5f * height, 0.0f);
	for (int i = 0; i <= numSlices; i++) {
		Vertex v;

		v.position = Vector3::Transform(startPoint, Matrix::CreateRotationY(dTheta * float(i)));
		v.normalModel = v.position - center;
		v.normalModel.Normalize( );
		v.texcoord = Vector2(float(i) / numSlices, 0.0f);
		vertices.push_back(v);
	}

	vector<uint32_t>& indices = meshData.indices;

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

MeshData GeometryGenerator::MakeSphere(const float radius, 
									   const int numSlices, const int numStacks,
									   const Vector2 texScale) {

	// Texture 좌표계때문에 (numSlices + 1) 개의 vertex 사용 (마지막에 닫아주는 vertex가 중복)
	// Stack은 y축 양의 방향으로 쌓아가는 방식

	const float dTheta = -XM_2PI / float(numSlices);
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
				stackStartPoint, Matrix::CreateRotationY(dTheta * float(i)));
			v.normalModel = v.position; // 원점이 구의 중심이므로
			v.normalModel.Normalize( );
			v.texcoord = Vector2(float(i) / numSlices, 1.0f - float(j) / numStacks) * texScale;

			Vector3 biTangent = Vector3(0.0f, 1.0f, 0.0f);

			Vector3 normalOrth = v.normalModel - biTangent.Dot(v.normalModel) * v.normalModel;
			normalOrth.Normalize( );

			v.tangentModel = biTangent.Cross(normalOrth);
			v.tangentModel.Normalize( );

			vertices.push_back(v);
		}
	}

	//cout << vertices.size( ) << endl;

	vector<uint32_t>& indices = meshData.indices;

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

	const float a = 1.0f;
	const float x = sqrt(3.0f) / 3.0f * a;
	const float d = sqrt(3.0f) / 6.0f * a; // = x / 2
	const float h = sqrt(6.0f) / 3.0f * a;

	vector<Vector3> points = { {0.0f, x, 0.0f},
							  {-0.5f * a, -d, 0.0f},
							  {+0.5f * a, -d, 0.0f},
							  {0.0f, 0.0f, h} };

	Vector3 center = Vector3(0.0f);

	for (int i = 0; i < 4; i++) {
		center += points[ i ];
	}
	center /= 4.0f;

	for (int i = 0; i < 4; i++) {
		points[ i ] -= center;
	}

	MeshData meshData;

	for (int i = 0; i < points.size( ); i++) {

		Vertex v;
		v.position = points[ i ];
		v.normalModel = v.position; // 중심이 원점
		v.normalModel.Normalize( );

		meshData.vertices.push_back(v);
	}

	meshData.indices = { 0, 1, 2, 3, 2, 1, 0, 3, 1, 0, 2, 3 };

	return meshData;
}

MeshData GeometryGenerator::MakeIcosahedron( ) {

	const float X = 0.525731f;
	const float Z = 0.850651f;

	MeshData newMesh;

	vector<Vector3> pos = {
		Vector3(-X, 0.0f, Z), Vector3(X, 0.0f, Z),   Vector3(-X, 0.0f, -Z),
		Vector3(X, 0.0f, -Z), Vector3(0.0f, Z, X),   Vector3(0.0f, Z, -X),
		Vector3(0.0f, -Z, X), Vector3(0.0f, -Z, -X), Vector3(Z, X, 0.0f),
		Vector3(-Z, X, 0.0f), Vector3(Z, -X, 0.0f),  Vector3(-Z, -X, 0.0f) };

	for (size_t i = 0; i < pos.size( ); i++) {
		Vertex v;
		v.position = pos[ i ];
		v.normalModel = v.position;
		v.normalModel.Normalize( );

		newMesh.vertices.push_back(v);
	}

	newMesh.indices = { 1,  4,  0, 4,  9, 0, 4, 5,  9, 8, 5, 4,  1,  8, 4,
					   1,  10, 8, 10, 3, 8, 8, 3,  5, 3, 2, 5,  3,  7, 2,
					   3,  10, 7, 10, 6, 7, 6, 11, 7, 6, 0, 11, 6,  1, 0,
					   10, 1,  6, 11, 0, 9, 2, 11, 9, 5, 2, 9,  11, 2, 7 };

	return newMesh;

}

MeshData GeometryGenerator::SubdivideToSphere(const float radius, MeshData meshData, bool faceNormal) {

	// 원점을 중심이라고 가정
	
	// 구의 표면으로 옮기고 노멀과 texture 좌표 계산
	auto ProjectVertex = [&](Vertex& v) {
		v.normalModel = v.position;
		v.normalModel.Normalize( );
		v.position = v.normalModel * radius;

		// 텍스쳐 좌표를 역산정하는 로직
		/*const float theta = atan2f(v.position.z, v.position.x);
		const float phi = acosf(v.position.y / radius);
		v.texcoord.x = theta / XM_2PI;
		v.texcoord.y = phi / XM_PI;*/
	};

	auto UpdateFaceNormal = [](Vertex& v0, Vertex& v1, Vertex& v2) {
		// v0, v1, v2로 이루어진 삼각형의 faceNormal 계산
		auto faceNormal =
			(v1.position - v0.position).Cross(v2.position - v0.position);
		faceNormal.Normalize( );
		v0.normalModel = faceNormal;
		v1.normalModel = faceNormal;
		v2.normalModel = faceNormal;
	};

	// Vertex가 중복되는 구조로 구현
	MeshData newMesh;
	uint16_t count = 0;
	for (uint32_t i = 0; i < meshData.indices.size( ); i += 3) {
		size_t i0 = meshData.indices[ i ];
		size_t i1 = meshData.indices[ i + 1 ];
		size_t i2 = meshData.indices[ i + 2 ];

		Vertex v0 = meshData.vertices[ i0 ];
		Vertex v1 = meshData.vertices[ i1 ];
		Vertex v2 = meshData.vertices[ i2 ];

		ProjectVertex(v0);
		ProjectVertex(v1);
		ProjectVertex(v2);

		Vertex v3;
		// 위치와 텍스쳐 좌표 결정
		v3.position = (v0.position + v1.position) * 0.5f;
		v3.texcoord = (v0.texcoord + v1.texcoord) * 0.5f;
		ProjectVertex(v3);

		Vertex v4;
		v4.position = (v1.position + v2.position) * 0.5f;
		v4.texcoord = (v1.texcoord + v2.texcoord) * 0.5f;
		ProjectVertex(v4);

		Vertex v5;
		v5.position = (v0.position + v2.position) * 0.5f;
		v5.texcoord = (v0.texcoord + v2.texcoord) * 0.5f;
		ProjectVertex(v5);
		
		// 모든 vertex 새로 추가

		if(faceNormal) UpdateFaceNormal(v3, v1, v4);
		newMesh.vertices.push_back(v3);
		newMesh.vertices.push_back(v1);
		newMesh.vertices.push_back(v4);

		if (faceNormal) UpdateFaceNormal(v0, v3, v5);
		newMesh.vertices.push_back(v0);
		newMesh.vertices.push_back(v3);
		newMesh.vertices.push_back(v5);

		if (faceNormal) UpdateFaceNormal(v3, v4, v5);
		newMesh.vertices.push_back(v3);
		newMesh.vertices.push_back(v4);
		newMesh.vertices.push_back(v5);

		if (faceNormal) UpdateFaceNormal(v5, v4, v2);
		newMesh.vertices.push_back(v5);
		newMesh.vertices.push_back(v4);
		newMesh.vertices.push_back(v2);
		
		// 인덱스 업데이트
		for (uint32_t j = 0; j < 12; j++) {
			newMesh.indices.push_back(j + count);
		}
		count += 12;
	}

	return newMesh;
}

vector<MeshData> GeometryGenerator::ReadFromFile(std::string basePath,
												 std::string filename,
												 bool revertNormals) {
	using namespace DirectX;

	ModelLoader modelLoader;
	modelLoader.Load(basePath, filename, revertNormals);
	vector<MeshData>& meshes = modelLoader.meshes;

	// Normalize vertices
	Vector3 vmin(1000, 1000, 1000);
	Vector3 vmax(-1000, -1000, -1000);
	for (auto& mesh : meshes) {
		for (auto& v : mesh.vertices) {
			vmin.x = XMMin(vmin.x, v.position.x);
			vmin.y = XMMin(vmin.y, v.position.y);
			vmin.z = XMMin(vmin.z, v.position.z);

			vmax.x = XMMax(vmax.x, v.position.x);
			vmax.y = XMMax(vmax.y, v.position.y);
			vmax.z = XMMax(vmax.z, v.position.z);
		}
	}

	float dx = vmax.x - vmin.x, dy = vmax.y - vmin.y, dz = vmax.z - vmin.z;
	float dl = XMMax(XMMax(dx, dy), dz);
	float cx = (vmax.x + vmin.x) * 0.5f, cy = (vmax.y + vmin.y) * 0.5f, cz = (vmax.z + vmin.z) * 0.5f;

	for (auto& mesh : meshes) {
		for (auto& v : mesh.vertices) {
			v.position.x = (v.position.x - cx) / dl;
			v.position.y = (v.position.y - cy) / dl;
			v.position.z = (v.position.z - cz) / dl;
		}
	}

	return meshes;
}

} // namespace kusk