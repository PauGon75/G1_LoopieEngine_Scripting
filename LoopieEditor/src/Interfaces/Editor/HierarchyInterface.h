#pragma once 

#include "src/Interfaces/Interface.h"
#include "Loopie/Scene/Scene.h"

namespace Loopie {
	class HierarchyInterface : public Interface {
	public:
		HierarchyInterface();
		~HierarchyInterface() = default;
		void Init() override {}
		void Render() override;

		void SetScene(Scene* scene);

	private:
		void DrawEntitySlot(const std::shared_ptr<Entity>& entity);

	public:
		static std::shared_ptr<Entity> s_SelectedEntity;
	private:
		Scene* m_scene = nullptr;
	};
}