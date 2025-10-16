#pragma once
#include <memory>

#include "Loopie/Core/Identificable.h"

namespace Loopie {
	class Entity;
	class Transform;

	class Component : public Identificable
	{
		friend class Entity;
	public:
		Component() = default;
		virtual ~Component();

		Transform* GetTransform() const;
		std::shared_ptr<Entity> GetOwner() const { return m_owner.lock(); }
		virtual void Init() = 0;
	private:
		std::weak_ptr<Entity> m_owner;
	};
}