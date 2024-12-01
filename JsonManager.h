#pragma once

//vcpkg install rapidjson
#include <rapidjson/document.h>
#include <directxtk/SimpleMath.h>
#include <string>
#include <locale>


namespace kusk {

using DirectX::SimpleMath::Vector4;
using DirectX::SimpleMath::Vector3;
using DirectX::SimpleMath::Vector2;
using DirectX::SimpleMath::Matrix;

class JsonManager {
public:
	static rapidjson::Document LoadJson(const std::string filePath);
	static void SaveJson(const std::string filePath, 
						 const rapidjson::Document& jsonData);
	
	static Vector4 ParseVector4(const rapidjson::Value& value);
	static Vector3 ParseVector3(const rapidjson::Value& value);
	static Vector2 ParseVector2(const rapidjson::Value& value);
	static Matrix ParseMatrix(const rapidjson::Value& matrixArray);
	static rapidjson::Value Vector4ToJson(const Vector4& vec,
										  rapidjson::Document::AllocatorType& allocator);
	static rapidjson::Value Vector3ToJson(const Vector3& vec, 
										  rapidjson::Document::AllocatorType& allocator);
	static rapidjson::Value Vector2ToJson(const Vector2& vec,
										  rapidjson::Document::AllocatorType& allocator);
	static rapidjson::Value MatrixToJson(const Matrix& matrix, 
											   rapidjson::Document::AllocatorType& allocator);

	static std::string WStringToUTF8(const std::wstring& wstr) {
		std::string utf8Str;
		std::mbstate_t state = std::mbstate_t( );
		const wchar_t* src = wstr.data( );
		size_t len = 1 + std::wcsrtombs(nullptr, &src, 0, &state);

		if (len > 0) {
			std::vector<char> buffer(len);
			std::wcsrtombs(buffer.data( ), &src, buffer.size( ), &state);
			utf8Str.assign(buffer.data( ));
		}
		return utf8Str;
	}

	static std::wstring UTF8ToWString(const std::string& str) {
		std::wstring wstr;
		std::mbstate_t state = std::mbstate_t( );
		const char* src = str.data( );
		size_t len = 1 + std::mbsrtowcs(nullptr, &src, 0, &state);

		if (len > 0) {
			std::vector<wchar_t> buffer(len);
			std::mbsrtowcs(buffer.data( ), &src, buffer.size( ), &state);
			wstr.assign(buffer.data( ));
		}
		return wstr;
	}
};

} // namespace kusk