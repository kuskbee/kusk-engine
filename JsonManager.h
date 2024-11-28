#pragma once

//vcpkg install rapidjson
#include <rapidjson/document.h>
#include <directxtk/SimpleMath.h>
#include <string>

namespace kusk {

using DirectX::SimpleMath::Vector3;
using DirectX::SimpleMath::Vector2;
using DirectX::SimpleMath::Matrix;

class JsonManager {
public:
	static rapidjson::Document LoadJson(const std::string filePath);
	static void SaveJson(const std::string filePath, 
						 const rapidjson::Document& jsonData);
	
	static Vector3 ParseVector3(const rapidjson::Value& value);
	static Vector2 ParseVector2(const rapidjson::Value& value);
	static Matrix ParseMatrix(const rapidjson::Value& matrixArray);
	static rapidjson::Value Vector3ToJson(const Vector3& vec, 
										  rapidjson::Document::AllocatorType& allocator);
	static rapidjson::Value Vector2ToJson(const Vector2& vec,
										  rapidjson::Document::AllocatorType& allocator);
	static rapidjson::Value MatrixToJson(const Matrix& matrix, 
											   rapidjson::Document::AllocatorType& allocator);
};

} // namespace kusk