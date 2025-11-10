#include "Scene.h"
#include "Loopie/Components/Transform.h"

#include <unordered_set>

namespace Loopie {
	Scene::Scene(const std::string& filePath)
	{
		m_filePath = filePath;
		ReadAndLoadSceneFile();

		m_rootEntity = std::make_shared<Entity>("scene");
		m_rootEntity->AddComponent<Transform>();
	}

	Scene::~Scene()
	{
		m_entities.clear();
	}

	void Scene::SaveScene()
	{
		// TODO: This should export the scene and its entities 
		// into a serialized file 
		// We can use the hierarchy for this
	}

	std::shared_ptr<Entity>  Scene::CreateEntity(const std::string& name,
												std::shared_ptr<Entity> parentEntity)
	{
		std::string uniqueName = GetUniqueName(parentEntity, name);
		std::shared_ptr<Entity> entity = std::make_shared<Entity>(uniqueName);
		if (!parentEntity)
			m_rootEntity->AddChild(entity);
		else
			parentEntity->AddChild(entity);

		entity->AddComponent<Transform>();

		m_entities[entity->GetUUID()] = entity;
		return entity;
	}

	std::shared_ptr<Entity> Scene::CreateEntity(const vec3& position, const quaternion& rotation, const vec3& scale,
												std::shared_ptr<Entity> parentEntity, const std::string& name)
	{
		std::string uniqueName = GetUniqueName(parentEntity, name);
		std::shared_ptr<Entity> entity = std::make_shared<Entity>(uniqueName);
		if (!parentEntity)
			m_rootEntity->AddChild(entity);
		else
			parentEntity->AddChild(entity);

		entity->AddComponent<Transform>(position, rotation, scale);
		m_entities[entity->GetUUID()] = entity;

		return entity;
	}

	std::shared_ptr<Entity> Scene::CreateEntity(Transform* transform,
										std::shared_ptr<Entity> parentEntity,
										const std::string& name)
	{
		std::string uniqueName = GetUniqueName(parentEntity, name);
		std::shared_ptr<Entity> entity = std::make_shared<Entity>(uniqueName);
		if (!parentEntity)
			m_rootEntity->AddChild(entity);
		else
			parentEntity->AddChild(entity);

		if (!transform)
		{
			entity->AddComponent<Transform>();
		}
		else
		{
			entity->AddComponent<Transform>(*transform);
		}
		m_entities[entity->GetUUID()] = entity;

		return entity;
	}

	void Scene::RemoveEntity(UUID uuid)
	{
		auto it = m_entities.find(uuid);
		if (it == m_entities.end()) return;

		std::shared_ptr<Entity> entity = it->second;

		if (std::shared_ptr<Entity> parent = entity->GetParent().lock())
		{
			parent->RemoveChild(uuid);
		}

		m_entities.erase(uuid);
	}

	std::shared_ptr<Entity> Scene::GetRootEntity() const
	{
		return m_rootEntity;
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

	std::vector<std::shared_ptr<Entity>> Scene::GetAllEntitiesHierarchical(std::shared_ptr<Entity> parentEntity) const
	{
		std::vector<std::shared_ptr<Entity>> entities;

		if (!parentEntity)
		{
			parentEntity = m_rootEntity;
		}

		CollectEntitiesRecursive(parentEntity, entities);
		return entities;
	}

	std::vector<std::shared_ptr<Entity>> Scene::GetAllSiblings(std::shared_ptr<Entity> parentEntity) const
	{
		std::vector<std::shared_ptr<Entity>> siblingEntities;

		if (!parentEntity)
		{
			parentEntity = m_rootEntity;
		}

		for (const auto& child : parentEntity->GetChildren())
		{
			siblingEntities.push_back(child);
		}

		return siblingEntities;
	}

	void Scene::ReadAndLoadSceneFile()
	{
		// TODO: This should read the serialized file and load the entities and their uuid
		// We can use the hierarchy for this
	}

	std::string Scene::GetUniqueName(std::shared_ptr<Entity> parentEntity, const std::string& desiredName)
	{
		std::unordered_set<std::string> existingNames;
		for (const auto& sibling : GetAllSiblings(parentEntity))
		{
			existingNames.insert(sibling->GetName());
		}

		if (existingNames.find(desiredName) == existingNames.end())
			return desiredName;

		int counter = 1;
		std::string uniqueName;

		do
		{
			uniqueName = desiredName + " (" + std::to_string(counter) + ")";
			counter++;
		} while (existingNames.find(uniqueName) != existingNames.end());

		return uniqueName;
	}

	void Scene::CollectEntitiesRecursive(std::shared_ptr<Entity> entity,
		std::vector<std::shared_ptr<Entity>>& outEntities) const
	{
		if (!entity)
			return;

		outEntities.push_back(entity);

		for (const auto& child : entity->GetChildren())
		{
			CollectEntitiesRecursive(child, outEntities);
		}
	}
}
