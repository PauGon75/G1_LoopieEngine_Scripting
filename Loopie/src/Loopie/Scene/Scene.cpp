#include "Scene.h"
#include "Loopie/Files/Json.h"
#include "Loopie/Core/Application.h"
#include "Loopie/Core/Log.h"
#include "Loopie/Components/Transform.h"
#include "Loopie/Components/Camera.h"
#include "Loopie/Components/MeshRenderer.h"
#include "Loopie/Helpers/LoopieHelpers.h"
#include "Loopie/Resources/AssetRegistry.h"

#include <unordered_set>


namespace Loopie {
	Scene::Scene(const std::string& filePath)
	{
		m_filePath = filePath;

		m_rootEntity = std::make_shared<Entity>("scene");
		m_rootEntity->AddComponent<Transform>();

		AABB worldBounds(vec3(-500, -500, -500), vec3(500, 500, 500));
		m_octree = std::make_unique<Octree>(worldBounds);

	}

	Scene::~Scene()
	{
		m_entities.clear();
	}

	void Scene::SaveScene(const std::string filePath)
	{
		JsonData saveData;
		JsonNode entitiesObj = saveData.CreateArrayField("entities");
		
		for (const auto& [id, entity] : GetAllEntities())
		{
			JsonData entityObj = JsonData();
			
			entityObj.CreateField<std::string>("uuid", id.Get());
			entityObj.CreateField<std::string>("name", entity->GetName());
			entityObj.CreateField<bool>("active", entity->GetIsActive());


			if (std::shared_ptr<Entity> parentEntity = entity->GetParent().lock())
				entityObj.CreateField<std::string>("parent_uuid", parentEntity->GetUUID().Get());

			// Creates an array of components
			JsonNode componentsObj = entityObj.CreateArrayField("components");
			
			for (auto const& component : entity->GetComponents())
			{
				JsonData componentObj = JsonData();
				component->Serialize(componentObj.Node());
				componentsObj.AddArrayElement(componentObj.GetRoot());
			}

			saveData.AddArrayElement("entities", entityObj.GetRoot());
		}

		saveData.ToFile(filePath);
		Log::Info("Scene saved.");
	}

	std::shared_ptr<Entity> Scene::CreateEntity(const std::string& name,
												std::shared_ptr<Entity> parentEntity)
	{
		std::shared_ptr<Entity> realParent = parentEntity ? parentEntity : m_rootEntity;
		std::string uniqueName = GetUniqueName(realParent, name);
		std::shared_ptr<Entity> entity = std::make_shared<Entity>(uniqueName);

		realParent->AddChild(entity);

		entity->AddComponent<Transform>();

		m_entities[entity->GetUUID()] = entity;
		m_octree->Insert(entity);

		return entity;
	}

	std::shared_ptr<Entity> Scene::CreateEntity(const UUID& uuid, const std::string& name,
												 std::shared_ptr<Entity> parentEntity)
	{

		std::shared_ptr<Entity> realParent = parentEntity ? parentEntity : m_rootEntity;

		std::string uniqueName = GetUniqueName(realParent, name);
		std::shared_ptr<Entity> entity = std::make_shared<Entity>(uniqueName);
		entity->SetUUID(uuid);

		realParent->AddChild(entity);

		entity->AddComponent<Transform>();

		m_entities[entity->GetUUID()] = entity;
		m_octree->Insert(entity);

		return entity;
	}

	std::shared_ptr<Entity> Scene::CreateEntity(const vec3& position, const quaternion& rotation, const vec3& scale,
												std::shared_ptr<Entity> parentEntity, const std::string& name)
	{
		std::shared_ptr<Entity> realParent = parentEntity ? parentEntity : m_rootEntity;
		std::string uniqueName = GetUniqueName(realParent, name);
		std::shared_ptr<Entity> entity = std::make_shared<Entity>(uniqueName);

		realParent->AddChild(entity);

		entity->AddComponent<Transform>(position, rotation, scale);
		m_entities[entity->GetUUID()] = entity;
		m_octree->Insert(entity);

		return entity;
	}

	std::shared_ptr<Entity> Scene::CreateEntity(Transform* transform,
										std::shared_ptr<Entity> parentEntity,
										const std::string& name)
	{
		std::shared_ptr<Entity> realParent = parentEntity ? parentEntity : m_rootEntity;
		std::string uniqueName = GetUniqueName(realParent, name);
		std::shared_ptr<Entity> entity = std::make_shared<Entity>(uniqueName);
		realParent->AddChild(entity);

		if (!transform)
		{
			entity->AddComponent<Transform>();
		}
		else
		{
			entity->AddComponent<Transform>(*transform);
		}
		m_entities[entity->GetUUID()] = entity;
		m_octree->Insert(entity);

		return entity;
	}

	void Scene::RemoveEntity(UUID uuid)
	{
		auto it = m_entities.find(uuid);
		if (it == m_entities.end())
			return;

		m_octree->Remove(it->second);
		RemoveEntityRecursive(it->second);
	}

	void Scene::RemoveEntity(std::shared_ptr<Entity> entity)
	{
		if (!entity) 
			return;

		m_octree->Remove(entity);
		RemoveEntityRecursive(entity);
	}

	void Scene::SetFilePath(std::string filePath)
	{
		m_filePath = filePath;
	}

	std::string Scene::GetFilePath() const
	{
		return m_filePath;
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

	Octree& Scene::GetOctree() const
	{
		return *m_octree;
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

	void Scene::ReadAndLoadSceneFile(std::string filePath, bool safeSceneAsLastLoaded)
	{
		m_entities.clear();
		m_octree->Clear();

		AABB worldBounds(vec3(-500, -500, -500), vec3(500, 500, 500));
		m_octree = std::make_unique<Octree>(worldBounds);

		m_rootEntity = std::make_shared<Entity>("scene");
		m_rootEntity->AddComponent<Transform>();
		
		JsonData saveData = Json::ReadFromFile(filePath);

		if (saveData.IsEmpty())
		{
			Log::Error("Failed to load scene file");
			return;
		}

		JsonNode rootNode = saveData.Child("entities");

		if (!rootNode.IsValid() || !rootNode.IsArray())
		{
			Log::Error("No entities array in scene file.");
			return;
		}

		// First iteration: Create all entities
		for (unsigned int i = 0; i < rootNode.Size(); ++i)
		{
			JsonResult<json> entityJson = rootNode.GetArrayElement<json>(i);
			JsonNode entityNode = JsonNode(&entityJson.Result);

			if (!entityNode.IsValid() || !entityNode.Contains("uuid") || !entityNode.Contains("name"))
				continue;

			UUID uuid = UUID(entityNode.GetValue<std::string>("uuid").Result);
			std::string name = entityNode.GetValue<std::string>("name").Result;
			bool active = entityNode.GetValue<bool>("active", false).Result;

			std::shared_ptr<Entity> entity = CreateEntity(uuid, name);
			entity->SetName(name);
			entity->SetIsActive(active);		
		}

		// Second iteration: Link relationships and components
		for (size_t i = 0; i < rootNode.Size(); ++i)
		{
			JsonResult<json> entityJson = rootNode.GetArrayElement<json>(uint32_t(i));
			JsonNode entityNode = JsonNode(&entityJson.Result);

			if (!entityNode.IsValid() || !entityNode.Contains("uuid"))
				continue;

			UUID uuid = UUID(entityNode.GetValue<std::string>("uuid").Result);
			std::shared_ptr<Entity> entity = m_entities[uuid];

			// Set parent if it exists
			if (entityNode.Contains("parent_uuid"))
			{
				UUID parentUUID = UUID(entityNode.GetValue<std::string>("parent_uuid").Result);
				if (m_entities.find(parentUUID) != m_entities.end())
				{
					entity->SetParent(m_entities[parentUUID]);
				}
			}

			JsonNode componentsObj = entityNode.Child("components");
			if (componentsObj.IsValid() && componentsObj.IsArray())
			{

				for (size_t j = 0; j < componentsObj.Size(); ++j)
				{
					JsonResult<json> componentJson = componentsObj.GetArrayElement<json>(uint32_t(j));
					JsonNode componentNode = JsonNode(&componentJson.Result);

					// *** Component Checking *** - PSS 08/12/25
					// This checks manually which component type it is.
					// This lacks scalability. 
					// Might be worth looking into if components expand too much.
					if (componentNode.Contains("transform"))
					{
						JsonNode node = componentNode.Child("transform");
						entity->GetTransform()->Deserialize(node);
					}
					else if (componentNode.Contains("camera"))
					{
						JsonNode node = componentNode.Child("camera");
						auto camera = entity->AddComponent<Camera>();
						if (camera)
						{
							camera->Deserialize(node);
						}
					}
					else if (componentNode.Contains("meshrenderer"))
					{
						JsonNode node = componentNode.Child("meshrenderer");
						auto meshRenderer = entity->AddComponent<MeshRenderer>();
						if (meshRenderer)
						{
							meshRenderer->Deserialize(node);
						}
					}
				}
			}
		}
		Log::Info("Scene loaded successfully");

		if (safeSceneAsLastLoaded) {
			std::filesystem::path config = Application::GetInstance().m_activeProject.GetConfigPath();
			if (!config.empty())
			{
				JsonData configData = Json::ReadFromFile(config.string());
				JsonResult<std::string> result = configData.Child("last_scene").GetValue<std::string>();
				if (!result.Found) {
					configData.CreateField<std::string>("last_scene", "");
				}
				configData.SetValue<std::string>("last_scene", filePath);
				configData.ToFile(config.string());

				/*Metadata* metadata = AssetRegistry::GetMetadata(filePath); /// Swap to UUID
				if (metadata)
					configData.SetValue<std::string>("last_scene", metadata->UUID.Get());*/
			}
		}

		
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
