#pragma once

#include "Loopie/Core/UUID.h"
#include "Loopie/Components/Component.h"

#include <string>
#include <vector>
#include <memory>

namespace Loopie
{
	class Entity : std::enable_shared_from_this<Entity>
	{
	public:
		Entity(const std::string& name);
		~Entity();
		

		void AddComponent(std::shared_ptr<Component> component);
		template<typename T>
		std::shared_ptr<T> GetComponent() const
		{
			for (Component* comp : m_components)
			{
				// Try to cast to the requested type
				T* casted = dynamic_cast<T*>(comp);
				if (casted)
				{
					// Wrap in shared_ptr without taking ownership
					return std::shared_ptr<T>(std::shared_ptr<Component>(), casted);
				}
			}
			return nullptr;
		}
		template<typename T>
		bool HasComponent() const
		{
			return GetComponent<T>() != nullptr;
		}
		template<typename T>
		void RemoveComponent()
		{
			for (auto it = m_components.begin(); it != m_components.end(); ++it)
			{
				T* casted = dynamic_cast<T*>(*it);
				if (casted)
				{
					delete* it;
					m_components.erase(it);
					return;  // Remove first match only
				}
			}
		}

		// If a child is set up, then it means this is its parent and will update it accordingly
		void AddChild(std::shared_ptr<Entity> child);
		void RemoveChild(std::shared_ptr<Entity> child);
		void RemoveChild(UUID childUuid);

		UUID GetUuid() const;
		std::string GetName() const;
		bool GetIsActive() const;
		std::shared_ptr<Entity> GetChild(UUID uuid) const;
		std::vector<std::shared_ptr<Entity>> GetChildren() const;
		std::weak_ptr<Entity> GetParent() const;

		void SetName(const std::string& name);
		void SetIsActive(bool active);
		// If a parent is set up, then it means this is its child and will update it accordingly
		void SetParent(std::shared_ptr<Entity> parent);

	private:
		std::weak_ptr<Entity> m_parentEntity;
		std::vector<std::shared_ptr<Entity>> m_childrenEntities;
		std::vector<std::shared_ptr<Component>> m_components;

		UUID m_uuid;
		std::string m_name;
		bool m_isActive = true;
	};
}