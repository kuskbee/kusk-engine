#pragma once

//vcpkg install rapidjson
#include <rapidjson/document.h>
#include <directxtk/SimpleMath.h>
#include <string>

namespace kusk {

using DirectX::SimpleMath::Vector3;
using DirectX::SimpleMath::Vector2;

class JsonManager {
	static rapidjson::Document LoadJson(const std::string filePath);
	static void SaveJson(const std::string filePath, 
						 const rapidjson::Document& jsonData);
	
	static Vector3 parseVector3(const rapidjson::Value& value);
	static Vector2 parseVector2(const rapidjson::Value& value);
	static rapidjson::Value toJsonVector3(const Vector3& vec, 
										  rapidjson::Document::AllocatorType& allocator);
	static rapidjson::Value toJsonVector2(const Vector2& vec,
										  rapidjson::Document::AllocatorType& allocator);
};

} // namespace kusk