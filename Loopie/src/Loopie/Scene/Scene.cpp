#include "Scene.h"
#include "Loopie/Files/Json.h"
#include "Loopie/Core/Log.h"
#include "Loopie/Components/Transform.h"
#include "Loopie/Components/Camera.h"
#include "Loopie/Components/MeshRenderer.h"
#include "Loopie/Helpers/LoopieHelpers.h"

#include <unordered_set>


namespace Loopie {
	Scene::Scene(const std::string& filePath)
	{
		m_filePath = filePath;

		m_rootEntity = std::make_shared<Entity>("scene");
		m_rootEntity->AddComponent<Transform>();

		//ReadAndLoadSceneFile(std::string("TESTSavedScene.json"));
		//ReadAndLoadSceneFile(filePath);
	}

	Scene::~Scene()
	{
		m_entities.clear();
	}

	void Scene::SaveScene(const std::string* filePath)
	{
		JsonData saveData;
		saveData.CreateArrayField("entities");

		// Pol comment to understand json::objects and json::arrays
		// Array field = values
		// Object field = kvm
		
		for (const auto& [id, entity] : GetAllEntities())
		{
			json entityObj = json::object();

			entityObj["uuid"] = id.Get();
			entityObj["name"] = entity->GetName();
			entityObj["active"] = entity->GetIsActive();

			if (std::shared_ptr<Entity> parentEntity = entity->GetParent().lock())
				entityObj["parent_uuid"] = parentEntity->GetUUID().Get();
			else
				entityObj["parent_uuid"] = nullptr;

			// Creates an array of components
			entityObj["components"] = json::array();

			for (auto const& component : entity->GetComponents())
			{
				entityObj["components"].push_back(component->Serialize());
			}

			saveData.AddArrayElement("entities", entityObj);
		}

		saveData.ToFile("TESTSavedScene.json");
		Log::Info("Scene saved.");
	}

	std::shared_ptr<Entity> Scene::CreateEntity(const std::string& name,
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

	std::shared_ptr<Entity> Scene::CreateEntity(const UUID& uuid, const std::string& name,
												 std::shared_ptr<Entity> parentEntity)
	{
		std::string uniqueName = GetUniqueName(parentEntity, name);
		std::shared_ptr<Entity> entity = std::make_shared<Entity>(uniqueName);
		entity->SetUUID(uuid);

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
		if (it == m_entities.end())
			return;

		RemoveEntityRecursive(it->second);
	}

	void Scene::RemoveEntity(std::shared_ptr<Entity> entity)
	{
		if (!entity) 
			return;

		RemoveEntityRecursive(entity);
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

	void Scene::ReadAndLoadSceneFile(std::string filePath)
	{
		// TODO: This should read the serialized file and load the entities and their uuid
		// We can use the hierarchy for this

		m_entities.clear();

		// TEMP
		m_rootEntity = std::make_shared<Entity>("scene");
		m_rootEntity->AddComponent<Transform>();
		// END_TEMP
		
		//JsonData saveData = Json::ReadFromFile(*filePath);
		JsonData saveData = Json::ReadFromFile("TESTSavedScene.json");

		if (saveData.IsEmpty())
		{
			Log::Error("Failed to load scene file");
			return;
		}

		json& rootNode = saveData.GetRoot();

		if (!rootNode.contains("entities") || !rootNode["entities"].is_array())
		{
			Log::Error("No entities array in scene file.");
			return;
		}

		json& entitiesArray = rootNode["entities"];

		// Map used to store created entities by UUID for later linking

		// First iteration: Create all entities
		for (size_t i = 0; i < entitiesArray.size(); ++i)
		{
			json& entityJson = entitiesArray[i];

			if (!entityJson.contains("uuid") || !entityJson.contains("name"))
				continue;

			UUID uuid = UUID(entityJson["uuid"].get<std::string>());
			std::string name = entityJson["name"].get<std::string>();
			bool active = entityJson["active"].get<bool>();

			std::shared_ptr<Entity> entity = CreateEntity(uuid, name);
			entity->SetIsActive(active);		}

		// Second iteration: Link relationships and components
		for (size_t i = 0; i < entitiesArray.size(); ++i)
		{
			json& entityJson = entitiesArray[i];

			if (!entityJson.contains("uuid"))
				continue;

			UUID uuid = UUID(entityJson["uuid"].get<std::string>());
			std::shared_ptr<Entity> entity = m_entities[uuid];

			// Set parent if it exists
			if (entityJson.contains("parent_uuid") && !entityJson["parent_uuid"].is_null())
			{
				UUID parentUUID = UUID(entityJson["parent_uuid"].get<std::string>());
				if (m_entities.find(parentUUID) != m_entities.end())
				{
					entity->SetParent(m_entities[parentUUID]);
				}
			}

			if (entityJson.contains("components") && entityJson["components"].is_array())
			{
				json& componentsArray = entityJson["components"];

				for (size_t j = 0; j < componentsArray.size(); ++j)
				{
					json& componentJson = componentsArray[j];

					// Check which component type it is - I don't know if we can do this more clean?
					if (componentJson.contains("transform"))
					{
						entity->GetTransform()->Deserialize(componentJson["transform"]);
					}
					else if (componentJson.contains("camera"))
					{
						auto camera = entity->AddComponent<Camera>();
						if (camera)
						{
							camera->Deserialize(componentJson["camera"]);
						}
					}
					else if (componentJson.contains("meshrenderer"))
					{
						auto meshRenderer = entity->AddComponent<MeshRenderer>();
						if (meshRenderer)
						{
							meshRenderer->Deserialize(componentJson["meshrenderer"]);
						}
					}
				}
			}
		}
		Log::Info("Scene loaded successfully");
	}

	std::string Scene::GetUniqueName(std::shared_ptr<Entity> parentEntity, const std::string& desiredName)
	{
		if (!parentEntity)
			return desiredName;

		std::vector<std::string> existingNames;
		for (const auto& sibling : GetAllSiblings(parentEntity))
		{
			existingNames.emplace_back(sibling->GetName());
		}

		return Helper::MakeUniqueName(desiredName, existingNames);
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

	void Scene::RemoveEntityRecursive(std::shared_ptr<Entity> entity)
	{
		if (!entity) 
			return;

		auto children = entity->GetChildren(); // Making a copy instead of referencing just in case

		for (const auto& child : children)
		{
			RemoveEntityRecursive(child);
		}

		if (std::shared_ptr<Entity> parent = entity->GetParent().lock())
		{
			parent->RemoveChild(entity->GetUUID());
		}

		m_entities.erase(entity->GetUUID());
	}
}
