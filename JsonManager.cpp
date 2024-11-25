
#include "JsonManager.h"

#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/filereadstream.h>
#include <rapidjson/filewritestream.h>
#include <rapidjson/error/en.h> // for error messages
#include <sstream>
#include <fstream>
#include <filesystem>

#include <vector>
#include <stdexcept>

namespace kusk {

rapidjson::Document JsonManager::LoadJson(const std::string filePath) {

    if (!std::filesystem::exists(filePath)) {
        throw std::runtime_error("File does not exist: " + filePath);
    }

    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filePath);
    }

    std::ostringstream oss;
    oss << file.rdbuf(); // 파일 내용을 스트림으로 읽음
    const std::string& fileContent = oss.str();

    rapidjson::Document jsonData;
    jsonData.Parse(fileContent.c_str( ));

    if (jsonData.HasParseError( )) {
        throw std::runtime_error("JSON parse error: " +
                std::string(rapidjson::GetParseError_En(jsonData.GetParseError( ))) +
                " at offset " + std::to_string(jsonData.GetErrorOffset( )));
    }

    return jsonData;
}

void JsonManager::SaveJson(const std::string filePath, const rapidjson::Document& jsonData) {

    rapidjson::StringBuffer sBuffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(sBuffer);
    jsonData.Accept(writer);

    std::ofstream file(filePath, std::ios::binary);
    if(!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filePath);
    }

    file.write(sBuffer.GetString( ), sBuffer.GetSize( ));
    if (!file.good( )) {
        throw std::runtime_error("Failed to write the complete JSON to file: " + filePath);
    }
}

Vector3 JsonManager::parseVector3(const rapidjson::Value& value) {
    if (!value.IsArray( ) || value.Size( ) != 3) {
        throw std::runtime_error("Invalid Vector3 format: must be an array of size 3.");
    }

    for (auto& v : value.GetArray( )) {
        if (!v.IsNumber( )) {
            throw std::runtime_error("Invalid Vector3 format: all elements must be numbers.");
        }
    }

    return Vector3(
        value[ 0 ].GetFloat( ),    //x
        value[ 1 ].GetFloat( ),    //y
        value[ 2 ].GetFloat( )     //z
    );
}

Vector2 JsonManager::parseVector2(const rapidjson::Value& value) {
    if (!value.IsArray( ) || value.Size( ) != 2) {
        throw std::runtime_error("Invalid Vector2 format: must be an array of size 3.");
    }

    for (auto& v : value.GetArray( )) {
        if (!v.IsNumber( )) {
            throw std::runtime_error("Invalid Vector2 format: all elements must be numbers.");
        }
    }

    return Vector2(
        value[ 0 ].GetFloat( ),    //x
        value[ 1 ].GetFloat( )    //y
    );
}

rapidjson::Value JsonManager::toJsonVector3(const Vector3& vec, rapidjson::Document::AllocatorType& allocator) {
    rapidjson::Value value(rapidjson::kArrayType);

    value.PushBack(vec.x, allocator)
         .PushBack(vec.y, allocator)
         .PushBack(vec.z, allocator);

    return value;
}

rapidjson::Value JsonManager::toJsonVector2(const Vector2& vec, rapidjson::Document::AllocatorType& allocator) {
    rapidjson::Value value(rapidjson::kArrayType);

    value.PushBack(vec.x, allocator)
        .PushBack(vec.y, allocator);

    return value;
}

} // namespace kusk