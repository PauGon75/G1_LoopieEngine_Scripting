#pragma once 

#include "Editor/Interfaces/Interface.h"
#include "Loopie/Events/IObserver.h"
#include "Editor/Events/EditorEventTypes.h"

namespace Loopie {
	class Transform;
	class Camera;
	class MeshRenderer;


	class InspectorInterface : public Interface , public IObserver<OnEntityOrFileNotification>{
	public:
		InspectorInterface();
		~InspectorInterface();
		void Init() override;
		void Render() override;

	private:

		enum class InspectorMode {
			EntityMode,
			ImportMode,
			None
		};

		void DrawEntityInspector(const std::shared_ptr<Entity>& entity);
		void DrawFileImportSettings(const std::filesystem::path& path);

		///EntityRelated
		void DrawEntityConfig(const std::shared_ptr<Entity>& entity);
		void DrawTransform(Transform* transform);
		void DrawCamera(Camera* camera);
		void DrawMeshRenderer(MeshRenderer* meshRenderer);
		void AddComponent(const std::shared_ptr<Entity>& entity);
		bool RemoveComponent(Component* component);

		///FilesRelated
		void DrawMaterialImportSettings(const std::filesystem::path& path);

		// Inherited via IObserver
		void OnNotify(const OnEntityOrFileNotification& id) override;

	private:

		InspectorMode m_mode = InspectorMode::None;
	};
}