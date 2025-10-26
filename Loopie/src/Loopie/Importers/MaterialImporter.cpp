#include "MaterialImporter.h"

#include "Loopie/Core/Log.h"
#include "Loopie/Core/Application.h"
#include "Loopie/Files/Json.h"

#include <fstream>
#include <iostream>
#include <filesystem> // Used for checking the extension

namespace Loopie {
	std::string MaterialImporter::ImportMaterial(const std::string& filepath) {
		std::string outputPath;

		JsonData data = Json::ReadFromFile(filepath);
		if (data.IsEmpty())
			return "";

		JsonResult<std::string> shaderNode = data.GetValue<std::string>("shader");
		std::string shaderUUID = shaderNode.Result;

		JsonNode propertiesNode = data.Child("properties");
		std::vector<std::string> propKeys = propertiesNode.GetObjectKeys();

		Project project = Application::GetInstance().m_activeProject;
		std::filesystem::path pathToWrite = project.GetChachePath();
		UUID id;
		pathToWrite /= "Materials";
		pathToWrite /= id.Get() + ".material";

		std::ofstream fs(pathToWrite, std::ios::out | std::ios::binary | std::ios::app);
		

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
			if (type == "Int") {

			}
			else if (type == "Float") {

			}
			else if (type == "Bool") {

			}
			else if (type == "Color") {

			}
			else if (type == "Texture") {

			}
		}

		fs.close();
		return outputPath;

	}

	void MaterialImporter::LoadMaterial(const std::string& path, Material& material) {
		std::filesystem::path filepath = path;
		if (!std::filesystem::exists(path))
			return;

		/// READ
		std::ifstream file(path, std::ios::binary);
		if (!file) {
			Log::Warn("Error opening .material file -> {0}", path.c_str());
			return;
		}

		file.seekg(0, std::ios::end);
		std::streamsize size = file.tellg();
		file.seekg(0, std::ios::beg);

		if (size <= 0) {
			Log::Warn("Error reading .material file -> {0}", path.c_str());
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
			switch (0 /*EnumType*/)
			{
				////TypeReadMode
			default:
				break;
			}
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