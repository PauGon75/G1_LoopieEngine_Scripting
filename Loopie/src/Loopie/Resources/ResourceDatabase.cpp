#include "ResourceDatabase.h"
#include "Loopie/Resources/Types/Mesh.h"

#include <filesystem>

namespace Loopie {

	std::unordered_map<UUID, std::shared_ptr<Resource>> ResourceDatabase::s_Resources;

	void ResourceDatabase::AddResource(std::shared_ptr<Resource> resource) {
		s_Resources[resource->GetUUID()] = resource;
	}

	std::shared_ptr<Resource> ResourceDatabase::GetResource(const UUID& uuid) {
		if (ExistsResource(uuid))
			return s_Resources[uuid];
		return nullptr;
	}

	bool ResourceDatabase::ExistsResource(const UUID& uuid) {
		return s_Resources.find(uuid) != s_Resources.end();
	}
}