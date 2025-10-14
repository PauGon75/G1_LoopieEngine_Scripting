#pragma once

#include "Loopie/Core/UUID.h"
#include "Loopie/Components/Component.h"

#include <string>
#include <vector>
#include <memory>

namespace Loopie
{
	class Entity : public std::enable_shared_from_this<Entity>
	{
	public:
		Entity(const std::string& name);
		~Entity();
		
		const std::shared_ptr<Component> AddComponent(const std::shared_ptr<Component> component);

		template<typename T, typename = std::enable_if_t<std::is_base_of_v<Component, T>>>
		std::shared_ptr<T> AddComponent()
		{
			std::shared_ptr<T> component = std::make_shared<T>();
			m_components.push_back(component);
			return component;
		}

		// From Claude.ai:
		// "Without RTTI/dynamic_cast, static_pointer_cast will ALWAYS succeed, 
		// even if the component is the wrong type! This will cause crashes."
		// 
		// From ChatGPT:
		// "It will always succeed, even if the cast is invalid, 
		// because it’s compile-time only — not runtime-checked. 
		// That means if the wrong component type is stored, 
		// you’ll get undefined behavior or crashes later."
		// 
		// TODO: Verify if it works, and if it doesn't, 
		// consider using typing with strings on components.
		template<typename T, typename = std::enable_if_t<std::is_base_of_v<Component, T>>>
		std::shared_ptr<T> GetComponent() const
		{
			for (size_t i = 0; i < m_components.size(); i++)
			{
				std::shared_ptr<T> component = std::static_pointer_cast<T>(m_components[i]);
				if (component)
				{
					return component;
				}
			}
			return nullptr;
		}

		template<typename T, typename = std::enable_if_t<std::is_base_of_v<Component, T>>>
		bool HasComponent() const
		{
			return GetComponent<T>() != nullptr;
		}

		// Removes first component of that specific type
		template<typename T, typename = std::enable_if_t<std::is_base_of_v<Component, T>>>
		void RemoveComponent()
		{
			for (size_t i = 0; i < m_components.size(); i++)
			{
				std::shared_ptr<T> comp = std::static_pointer_cast<T>(m_components[i]);
				if (comp)
				{
					m_components.erase(m_components.begin() + i);
					return;
				}
			}
		}

		void RemoveComponent(std::shared_ptr<Component> component)
		{
			auto it = std::find(m_components.begin(), m_components.end(), component);
			if (it != m_components.end())
			{
				m_components.erase(it);
			}
		}

		// If a child is set up, then it means this is its parent and will update it accordingly
		void AddChild(std::shared_ptr<Entity> child);
		void RemoveChild(std::shared_ptr<Entity> child);
		void RemoveChild(UUID childUuid);

		UUID GetUuid() const;
		const std::string& GetName() const;
		bool GetIsActive() const;
		std::shared_ptr<Entity> GetChild(UUID uuid) const;
		const std::vector<std::shared_ptr<Entity>>& GetChildren() const;
		std::weak_ptr<Entity> GetParent() const;
		std::vector<std::shared_ptr<Component>> GetComponents() const;

		void SetName(const std::string& name);
		void SetIsActive(bool active);
		// If a parent is set up, then it means this is its child and will update it accordingly
		void SetParent(std::shared_ptr<Entity> parent);

	private:
		std::weak_ptr<Entity> m_parentEntity;
		std::vector<std::shared_ptr<Entity>> m_childrenEntities;
		std::vector<std::shared_ptr<Component>> m_components; // Might want to re-do this to a map for optimization

		UUID m_uuid;
		std::string m_name;
		bool m_isActive = true;
	};
}