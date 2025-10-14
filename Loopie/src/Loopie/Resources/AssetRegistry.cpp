#include "AssetRegistry.h"

#include "Loopie/Core/Log.h"
#include "Loopie/Core/Application.h"
#include "Loopie/Files/Json.h"
#include "Loopie/Files/DirectoryManager.h"

#include <filesystem>

namespace Loopie {

	std::unordered_map<UUID, AssetMetadata> AssetRegistry::s_Assets;
	std::unordered_map<std::string, std::vector<UUID>> AssetRegistry::s_SourcePathToUUID;
	std::vector<UUID> AssetRegistry::s_EmptyUUIDVector;

	void AssetRegistry::Initialize() {
		if (LoadRegistry()) {
			Log::Info("AssetRegistry initialized with {0} assets", s_Assets.size());
			ValidateAllAsset();
		}
		else {
			Log::Critical("AssetRegistry initialization failed");
		}
	}

	void AssetRegistry::Shutdown() {
		if (SaveRegistry()) {
			Log::Info("AssetRegistry shutdown, saved {0} assets", s_Assets.size());
		}
	}

	void AssetRegistry::RegisterAsset(const AssetMetadata& metadata) {
		RemoveAsset(metadata.uuid);
		s_Assets[metadata.uuid] = metadata;
		s_SourcePathToUUID[metadata.sourcePath].push_back(metadata.uuid);
	}

	bool AssetRegistry::RemoveAsset(const UUID& uuid) {
		auto it = s_Assets.find(uuid);
		if (it == s_Assets.end()) {
			return false;
		}
		const AssetMetadata& metadata = it->second;
		s_SourcePathToUUID.erase(metadata.sourcePath);
		s_Assets.erase(it);
		return true;
	}

	bool AssetRegistry::UpdateAsset(const UUID& uuid, const AssetMetadata& newMetadata) {
		if (!AssetExists(uuid)) {
			return false;
		}
		RemoveAsset(uuid);
		RegisterAsset(newMetadata);
		return true;
	}

	AssetMetadata* AssetRegistry::GetMetadata(const UUID& uuid) {
		auto it = s_Assets.find(uuid);
		if (it == s_Assets.end())
			return nullptr;
		return &it->second;
	}

	const std::vector<UUID>& AssetRegistry::GetUUIDFromSourcePath(const std::string& sourcePath)
	{
		auto it = s_SourcePathToUUID.find(sourcePath);
		if (it == s_SourcePathToUUID.end())
			return s_EmptyUUIDVector;
		return it->second;
	}

	bool AssetRegistry::AssetExists(const UUID& uuid)
	{
		auto it = s_Assets.find(uuid);
		if (it != s_Assets.end()) {
			if (it->second.isValid)
				return true;
			RemoveAsset(uuid);
			return false;
		}
		return false;
	}

	bool AssetRegistry::AssetExists(const std::string& sourcePath)
	{
		const std::vector<UUID> uuids = GetUUIDFromSourcePath(sourcePath);

		for (size_t i = 0; i < uuids.size(); i++)
		{
			if (AssetExists(uuids[i])) {
				return true;
			}
		}
		return false;
	}

	bool AssetRegistry::ValidateAsset(const UUID& uuid)
	{
		auto metadata = GetMetadata(uuid);
		if (!metadata) return false;

		bool sourceExists = std::filesystem::exists(metadata->sourcePath);
		bool cacheExists = std::filesystem::exists(metadata->cachePath);

		metadata->isValid = sourceExists && cacheExists;

		return metadata->isValid;
	}

	bool AssetRegistry::ValidateAllAsset()
	{
		bool correct = true;
		for (auto& assetData : s_Assets) {
			if (!ValidateAsset(assetData.first))
				correct = false;
		}
		return correct;
	}

	void AssetRegistry::RemoveInvalidAssets()
	{
		std::vector<UUID> invalidAssets;

		for (const auto& assetData : s_Assets) {
			if (!assetData.second.isValid) {
				invalidAssets.push_back(assetData.first);
				std::filesystem::remove(assetData.second.cachePath);
			}
		}

		for (const auto& uuid : invalidAssets) {
			RemoveAsset(uuid);
		}
	}

	bool AssetRegistry::SaveRegistry()
	{
		const Project& project = Application::GetInstance().m_activeProject;
		if (project.IsEmpty()) {
			return false;
		}

		ValidateAllAsset();
		RemoveInvalidAssets();

		std::filesystem::path savePath = project.GetProjectPath() / "assetsCache.json";
		if (!DirectoryManager::Contains(savePath))
			Json::WriteToFileFromString(savePath, "{}");

		JsonData data = Json::ReadFromFile(savePath);
		data.Root().RemoveSelf("");

		data.CreateField("AssetCount",s_Assets.size());
		JsonNode assetsObject = data.CreateObjectField("Assets");

		for (const auto& assetData : s_Assets)
		{
			if (!assetData.second.isValid)
				continue;
			JsonNode asset = assetsObject.CreateObjectField(assetData.second.uuid.Get());
			asset.CreateField("Id", assetData.second.uuid.Get());
			asset.CreateField("SourcePath", assetData.second.sourcePath);
			asset.CreateField("CachePath", assetData.second.cachePath);
			
		}

		data.ToFile(savePath);

		return true;
	}

	bool AssetRegistry::LoadRegistry()
	{
		const Project& project = Application::GetInstance().m_activeProject;
		if (project.IsEmpty()) {
			return false;
		}

		Clear();

		std::filesystem::path savePath = project.GetProjectPath() / "assetsCache.json";
		if (!DirectoryManager::Contains(savePath))
			Json::WriteToFileFromString(savePath, "{}");

		JsonData data = Json::ReadFromFile(savePath);
		JsonNode assetsObject = data.Child("Assets");
		if (assetsObject.IsValid() && assetsObject.Size() > 0) {
			std::vector<std::string> assetsData = assetsObject.GetObjectKeys();
			for (size_t i = 0; i < assetsData.size(); i++)
			{
				JsonNode asset = assetsObject.Child(assetsData[i]);
				AssetMetadata metadata;
				metadata.uuid = UUID(asset.GetValue<std::string>("Id").Result);
				metadata.sourcePath = asset.GetValue<std::string>("SourcePath").Result;
				metadata.cachePath = asset.GetValue<std::string>("CachePath").Result;

				s_Assets[metadata.uuid] = metadata;
				s_SourcePathToUUID[metadata.sourcePath].push_back(metadata.uuid);
			}
		}

		ValidateAllAsset();
		RemoveInvalidAssets();

		return true;
	}
}