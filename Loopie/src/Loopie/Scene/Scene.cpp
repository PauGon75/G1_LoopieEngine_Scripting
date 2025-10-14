#include "Scene.h"

namespace Loopie {
	Scene::Scene(const std::string& filePath)
	{
		m_filePath = filePath;
		ReadAndLoadSceneFile();
	}

	Scene::~Scene()
	{
		m_entities.clear();
	}

	void Scene::SaveScene()
	{
		// TODO: This should export the scene and its entities 
		// into a serialized file 
	}

	std::shared_ptr<Entity> Scene::CreateEntity(const std::string& name)
	{
		std::shared_ptr<Entity> entity = std::make_shared<Entity>(name);
		m_entities[entity->GetUuid()] = entity;
		return entity;
	}

	void Scene::RemoveEntity(UUID uuid)
	{
		m_entities.erase(uuid);
	}

	std::shared_ptr<Entity> Scene::GetEntity(UUID uuid) const
	{
		auto it = m_entities.find(uuid);
		return (it != m_entities.end()) ? it->second : nullptr;
	}

	std::shared_ptr<Entity> Scene::GetEntity(const std::string& name) const
	{
		for (const auto& [uuid, entity] : m_entities)
		{
			if (entity->GetName() == name)
			{
				return entity;
			}
		}
		return nullptr;
	}

	const std::unordered_map<UUID, std::shared_ptr<Entity>>& Scene::GetAllEntities() const
	{
		return m_entities;
	}

	void Scene::ReadAndLoadSceneFile()
	{
		// TODO: This should read the serialized file and load the entities and their uuid
	}
}
