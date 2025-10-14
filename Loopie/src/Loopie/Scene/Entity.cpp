#include "Entity.h"

namespace Loopie {
	Entity::Entity(const std::string& name) : m_name(name)
	{

	}

	Entity::~Entity()
	{
		m_components.clear();
		m_childrenEntities.clear();
	}

	const std::shared_ptr<Component> Entity::AddComponent(const std::shared_ptr<Component> component)
	{
		if (component)
		{
			m_components.push_back(component);
			return component;
		}
		return nullptr;
	}

	void Entity::AddChild(std::shared_ptr<Entity> child)
	{
		if (child && child.get() != this)
		{
			std::shared_ptr<Entity> childParent = child->m_parentEntity.lock();
			if (childParent)
			{
				childParent->RemoveChild(child);
			}

			m_childrenEntities.push_back(child);
			child->m_parentEntity = weak_from_this();
		}
	}

	void Entity::RemoveChild(std::shared_ptr<Entity> child)
	{
		auto it = std::find(m_childrenEntities.begin(), m_childrenEntities.end(), child);
		if (it != m_childrenEntities.end())
		{
			(*it)->m_parentEntity.reset();
			m_childrenEntities.erase(it);
		}
	}

	void Entity::RemoveChild(UUID childUuid)
	{
		for (auto it = m_childrenEntities.begin(); it != m_childrenEntities.end(); ++it)
		{
			if ((*it)->GetUuid() == childUuid)
			{
				(*it)->m_parentEntity.reset();
				m_childrenEntities.erase(it);
				return;
			}
		}
	}

	UUID Entity::GetUuid() const
	{
		return m_uuid;
	}

	const std::string& Entity::GetName() const
	{
		return m_name;
	}

	bool Entity::GetIsActive() const
	{
		return m_isActive;
	}

	std::shared_ptr<Entity> Entity::GetChild(UUID uuid) const
	{
		for (const auto& child : m_childrenEntities)
		{
			if (child->GetUuid() == uuid)
			{
				return child;
			}
		}
		return nullptr;
	}

	const std::vector<std::shared_ptr<Entity>>& Entity::GetChildren() const
	{
		return m_childrenEntities;
	}

	std::weak_ptr<Entity> Entity::GetParent() const
	{ 
		return m_parentEntity; 
	}

	std::vector<std::shared_ptr<Component>> Entity::GetComponents() const
	{
		return m_components;
	}

	void Entity::SetName(const std::string& name)
	{
		m_name = name;
	}

	void Entity::SetIsActive(bool active)
	{
		m_isActive = active;
	}

	void Entity::SetParent(std::shared_ptr<Entity> parent)
	{
		std::shared_ptr<Entity> parentEntity = m_parentEntity.lock();
		if (parentEntity)
		{
			parentEntity->RemoveChild(shared_from_this());
		}

		if (parent && (parent != shared_from_this()))
		{
			parent->AddChild(shared_from_this());
		}
	}
}