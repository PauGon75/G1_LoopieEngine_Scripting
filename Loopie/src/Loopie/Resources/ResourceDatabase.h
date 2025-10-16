#pragma once

#include "Loopie/Core/UUID.h"
#include "Loopie/Resources/Resource.h"

#include <unordered_map>
#include <memory>
#include <type_traits>


namespace Loopie {
	class ResourceDatabase {

	public:
		static void AddResource(std::shared_ptr<Resource> resource);

		template<typename T, typename = std::enable_if_t<std::is_base_of_v<Resource, T>>>
		static std::shared_ptr<T> LoadResource(const UUID& uuid) {
			if (ExistsResource(uuid)) {
				return GetResourceAs<T>(uuid);
			}

			auto metadata = AssetRegistry::GetMetadata(uuid);
			if (!metadata || !metadata->isValid) {
				return nullptr;
			}

			std::string extension = std::filesystem::path(metadata->cachePath).extension().string();
			std::shared_ptr<T> resource;

			resource = std::make_shared<T>(uuid);

			if (resource) {
				resource->LoadFromFile(metadata->cachePath);
				AddResource(resource);
			}

			return resource;
		}

		static std::shared_ptr<Resource> GetResource(const UUID& uuid);

		template<typename T, typename = std::enable_if_t<std::is_base_of_v<Resource, T>>>
		static std::shared_ptr<T> GetResourceAs(const UUID& uuid) {
			auto resource = GetResource(uuid);
			if (resource) {
				if (resource->GetTypeID() == T::GetTypeIDStatic())
					return std::static_pointer_cast<T>(resource);
				else
					return nullptr;

			}
			return nullptr;
		}

		static bool ExistsResource(const UUID& uuid);
	private:
		static std::unordered_map<UUID, std::shared_ptr<Resource>> s_Resources;
	};
}