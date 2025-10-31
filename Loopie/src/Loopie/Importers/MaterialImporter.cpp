#include "MaterialImporter.h"

#include "Loopie/Core/Log.h"
#include "Loopie/Core/Application.h"
#include "Loopie/Files/Json.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <filesystem> // Used for checking the extension

namespace Loopie {

	static std::vector<float> ParseNumberList(const std::string& s) {
		std::vector<float> out;
		std::stringstream ss(s);
		std::string item;
		while (std::getline(ss, item, ',')) {
			out.push_back(std::stof(item));
		}
		return out;
	}

	void MaterialImporter::ImportMaterial(const std::string& filepath, Metadata& metadata) {
		if (metadata.HasCache)
			return;

		JsonData data = Json::ReadFromFile(filepath);
		if (data.IsEmpty())
			return;

		JsonResult<std::string> shaderNode = data.GetValue<std::string>("shader");
		std::string shaderUUID = shaderNode.Result;

		JsonNode propertiesNode = data.Child("properties");
		std::vector<std::string> propKeys = propertiesNode.GetObjectKeys();

		Project project = Application::GetInstance().m_activeProject;
		UUID id;
		std::filesystem::path locationPath = "Materials";
		locationPath /= id.Get() + ".material";

		std::filesystem::path pathToWrite = project.GetChachePath() / locationPath;

		std::ofstream fs(pathToWrite, std::ios::binary | std::ios::trunc);
		

		fs.write(shaderUUID.c_str(), UUID::UUID_SIZE);

		unsigned int propertyCount = propKeys.size();
		fs.write(reinterpret_cast<const char*>(&propertyCount), sizeof(propertyCount));

		for (const auto& key : propKeys)
		{
			JsonNode propNode = propertiesNode.Child(key);
			JsonResult<std::string> typeResult = propNode.GetValue<std::string>("type");
			JsonResult<std::string> valueResult = propNode.GetValue<std::string>("value");

			if (!typeResult.Found || !valueResult.Found)
				continue;
			const std::string& type = typeResult.Result;
			const std::string& value = valueResult.Result;

			unsigned int nameLength = key.size();
			fs.write(reinterpret_cast<const char*>(&nameLength), sizeof(nameLength));
			fs.write(key.c_str(), nameLength);

			///Write EnumType
			///Write Value

			unsigned int enumLength = key.size();
			fs.write(reinterpret_cast<const char*>(&enumLength), sizeof(enumLength));
			if (type == "Int") {
				int data = std::stoi(value);
				fs.write(reinterpret_cast<const char*>(&data), sizeof(data));
			}
			else if (type == "Float") {
				int data = std::stof(value);
				fs.write(reinterpret_cast<const char*>(&data), sizeof(data));
			}
			else if (type == "UInt") {
				unsigned int data = std::stoul(value);
				fs.write(reinterpret_cast<const char*>(&data), sizeof(data));
			}
			else if (type == "Bool") {
				int data = key == "True" ? 1 : 0;
				fs.write(reinterpret_cast<const char*>(&data), sizeof(data));
			}
			else if (type == "Vec2" || type == "Vec3" || type == "Vec4" || type == "Mat2" || type == "Mat3" || type == "Mat4") {		
				auto nums = ParseNumberList(value);
				for(const auto& data : nums)
					fs.write(reinterpret_cast<const char*>(&data), sizeof(data));
			}
			else if (type == "Sampler2D") {
				//fs.write(shaderUUID.c_str(), UUID::UUID_SIZE);
			}
			else if (type == "Sampler3D") {
				//fs.write(shaderUUID.c_str(), UUID::UUID_SIZE);
			}
			else if (type == "SamplerCube") {
				//fs.write(shaderUUID.c_str(), UUID::UUID_SIZE);
			}
		}

		fs.close();

		metadata.HasCache = true;
		metadata.CachesPath.clear();
		metadata.CachesPath.push_back(locationPath.string());
		MetadataRegistry::SaveMetadata(filepath, metadata);

	}

	void MaterialImporter::LoadMaterial(const std::string& path, Material& material) {
		Project project = Application::GetInstance().m_activeProject;
		std::filesystem::path filepath = project.GetChachePath() / path;
		if (!std::filesystem::exists(filepath))
			return;

		/// READ
		std::ifstream file(filepath, std::ios::binary);
		if (!file) {
			Log::Warn("Error opening .material file -> {0}", filepath.string());
			return;
		}

		file.seekg(0, std::ios::end);
		std::streamsize size = file.tellg();
		file.seekg(0, std::ios::beg);

		if (size <= 0) {
			Log::Warn("Error reading .material file -> {0}", filepath.string());
			return;
		}

		///Load

		std::string id;
		file.read(id.data(), UUID::UUID_SIZE);
		UUID shaderUUID = UUID(id);

		unsigned int propertiesCount = 0;
		file.read(reinterpret_cast<char*>(&propertiesCount), sizeof propertiesCount);

		for (size_t i = 0; i < propertiesCount; i++)
		{
			unsigned int propertyNameLength = 0;
			file.read(reinterpret_cast<char*>(&propertyNameLength), sizeof propertyNameLength);

			std::string propertyName(propertyNameLength, '\0');
			file.read(propertyName.data(), sizeof propertyNameLength);

			//// ReadEnume
			unsigned int typeLength = 0;
			file.read(reinterpret_cast<char*>(&typeLength), sizeof(typeLength));
			std::string type(typeLength, '\0');
			file.read(&type[0], typeLength);


			UniformValue uv{};

			if (type == "Int") {
				int32_t v; file.read(reinterpret_cast<char*>(&v), sizeof(v));
				uv.type = UniformType_int; uv.value = v;
			}
			else if (type == "Float") {
				float v; file.read(reinterpret_cast<char*>(&v), sizeof(v));
				uv.type = UniformType_float; uv.value = v;
			}
			else if (type == "UInt") {
				uint32_t v; file.read(reinterpret_cast<char*>(&v), sizeof(v));
				uv.type = UniformType_uint; uv.value = v;
			}
			else if (type == "Bool") {
				uint8_t b; file.read(reinterpret_cast<char*>(&b), sizeof(b));
				uv.type = UniformType_bool; uv.value = (b != 0);
			}
			else if (type == "Vec2") {
				float v[2]; file.read(reinterpret_cast<char*>(v), sizeof(v));
				uv.type = UniformType_vec2; uv.value = vec2(v[0], v[1]);
			}
			else if (type == "Vec3") {
				float v[3]; file.read(reinterpret_cast<char*>(v), sizeof(v));
				uv.type = UniformType_vec3; uv.value = vec3(v[0], v[1], v[2]);
			}
			else if (type == "Vec4") {
				float v[4]; file.read(reinterpret_cast<char*>(v), sizeof(v));
				uv.type = UniformType_vec4; uv.value = vec4(v[0], v[1], v[2], v[3]);
			}
			else if (type == "Mat2") {
				float v[4]; file.read(reinterpret_cast<char*>(v), sizeof(v));
				uv.type = UniformType_mat2; uv.value = matrix2(v[0], v[1], v[2], v[3]);
			}
			else if (type == "Mat3") {
				float v[9]; file.read(reinterpret_cast<char*>(v), sizeof(v));
				uv.type = UniformType_mat3; uv.value = matrix3(v[0], v[1], v[2], v[3], v[4],v[5], v[6], v[7], v[8]);
			}
			else if (type == "Mat4") {
				float v[16]; file.read(reinterpret_cast<char*>(v), sizeof(v));
				uv.type = UniformType_mat4; uv.value = matrix4(v[0], v[1], v[2], v[3], v[4], v[5], v[6], v[7], v[8], v[9], v[10], v[11], v[12], v[13], v[14], v[15]);
			}
			else if (type == "Sampler2D" ) {

			}
			else if (type == "Sampler3D") {

			}
			else if (type == "SamplerCube") {

			}

			material.SetShaderVariable(propertyName, uv);
		}

		file.close();
	}

	bool MaterialImporter::CheckIfIsMaterial(const char* path) {
		std::string extension = std::filesystem::path(path).extension().string();
		for (char& c : extension)
		{
			c = std::tolower(static_cast<unsigned char>(c));
		}

		if (!extension.empty() && extension[0] == '.')
			extension = extension.substr(1);

		return extension == "mat";
	}
}