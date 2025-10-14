#pragma once

#include "Loopie/Core/UUID.h"

#include <unordered_map>
#include <vector>
#include <string>

namespace Loopie {

    struct AssetMetadata {
        UUID uuid;
        std::string sourcePath;
        std::string cachePath;
        bool isValid = true;
    };

    class AssetRegistry {
    public:
        static void Initialize();
        static void Shutdown();

        static void RegisterAsset(const AssetMetadata& metadata);
        static bool RemoveAsset(const UUID& uuid);
        static bool UpdateAsset(const UUID& uuid, const AssetMetadata& newMetadata);

        static AssetMetadata* GetMetadata(const UUID& uuid);
        static const std::vector<UUID>& GetUUIDFromSourcePath(const std::string& sourcePath);

        static bool AssetExists(const UUID& uuid);
        static bool AssetExists(const std::string& sourcePath);
        static bool ValidateAsset(const UUID& uuid);
        static bool ValidateAllAsset();

        static void RemoveInvalidAssets();

        static void Clear() { s_Assets.clear(); s_SourcePathToUUID.clear(); }

    private:
        static bool SaveRegistry();
        static bool LoadRegistry();

    private:
        static std::unordered_map<UUID, AssetMetadata> s_Assets;
        static std::unordered_map<std::string, std::vector<UUID>> s_SourcePathToUUID;
        static std::vector<UUID> s_EmptyUUIDVector;
    };
}