#pragma once

#include "Loopie/Resources/Resource.h"
#include "Loopie/Resources/Types/Texture.h"
#include "Loopie/Resources/Types/Mesh.h"
#include "Loopie/Resources/Types/Material.h"
#include "Loopie/Resources/AssetRegistry.h"
#include <unordered_map>
#include <memory>
#include <string>

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
        static std::shared_ptr<Resource> GetResource(const ResourceKey& key);

        template<typename T>
        static std::shared_ptr<T> GetResource(const std::string& path) {
            Metadata* metadata = AssetRegistry::GetMetadata(path);

            ResourceKey key{ *metadata, 0 };
            auto it = m_Resources.find(key);

            if (it != m_Resources.end()) {
                return std::static_pointer_cast<T>(it->second);
            }
            return nullptr;
        }

        template<typename T>
        static void AddResource(const std::string& path, std::shared_ptr<T> resource) {
            Metadata* metadata = AssetRegistry::GetMetadata(path);
            if (metadata) {
                ResourceKey key{ *metadata, 0 };
                m_Resources[key] = resource;
            }
        }
    private:
        static std::unordered_map<ResourceKey, std::shared_ptr<Resource>, ResourceKeyHash> m_Resources;
    };
}