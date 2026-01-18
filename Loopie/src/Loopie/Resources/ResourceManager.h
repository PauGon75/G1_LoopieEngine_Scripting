#pragma once
#include "Loopie/Resources/Resource.h"
#include "Loopie/Resources/Types/Texture.h"
#include "Loopie/Resources/Types/Mesh.h"
#include "Loopie/Resources/Types/Material.h"
#include "Loopie/Resources/Resource.h"
#include "Loopie/Resources/AssetRegistry.h"

#include <unordered_map>
#include <memory>

namespace Loopie {

    struct ResourceKey {
        Metadata metadata;
        int index = 0;

        bool operator==(const ResourceKey& other) const noexcept {
            return metadata.UUID == other.metadata.UUID && index == other.index;
        }
    };

    struct ResourceKeyHash {
        std::size_t operator()(const ResourceKey& key) const noexcept {
            return std::hash<UUID>()(key.metadata.UUID) ^ (std::hash<int>()(key.index) << 1);
        }
    };

    class ResourceManager {    
    public:
        static std::shared_ptr<Texture> GetTexture(const Metadata& metadata);
        static std::shared_ptr<Mesh> GetMesh(const Metadata& metadata, int index);
        static std::shared_ptr<Material> GetMaterial(const Metadata& metadata);
        static void RemoveResource(Resource& resource);
        template<typename T>
        static std::shared_ptr<T> GetResourceByPath(const std::string& path) {
            // Obtenemos la metadata del registro de assets
            auto metadata = AssetRegistry::GetMetadata(path);

            // IMPORTANTE: Si GetMetadata devuelve un puntero, usa metadata->UUID
            // Si devuelve un objeto, asegúrate de tener el include de Metadata.h
            for (auto const& [key, resource] : m_Resources) {
                if (key.metadata.UUID == metadata.UUID) {
                    return std::static_pointer_cast<T>(resource);
                }
            }
            return nullptr;
        }
        static std::shared_ptr<Resource> GetResource(const ResourceKey& key);

    private:
		static std::unordered_map<ResourceKey, std::shared_ptr<Resource>, ResourceKeyHash> m_Resources;
    };
}