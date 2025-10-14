#pragma once

#include "Loopie/Core/UUID.h"
#include "Loopie/Scene/Entity.h"

#include <string>
#include <unordered_map>
	

namespace Loopie {
	class Scene
	{
	public:
		Scene(const std::string& filePath);
		~Scene();

		void SaveScene();

		std::shared_ptr<Entity> CreateEntity(const std::string& name = "Entity");
		void RemoveEntity(UUID uuid);

		std::shared_ptr<Entity> GetEntity(UUID uuid) const;
		std::shared_ptr<Entity> GetEntity(const std::string& name) const;
		// Returns the unordered_map of the UUID and Entity ptrs
		// Usage: for (const auto& [uuid, entity] : scene.GetAllEntities()) {entity->Update();}
		const std::unordered_map<UUID, std::shared_ptr<Entity>>& GetAllEntities() const;

	private:
		void ReadAndLoadSceneFile();

	private:
		std::unordered_map<UUID, std::shared_ptr<Entity>> m_entities;
		std::string m_filePath;
	};	
}