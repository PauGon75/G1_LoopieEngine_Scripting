#include "ProjectSetupModule.h"


#include "Loopie/Core/Application.h"

namespace Loopie {
	class EditorApplication : public Application {
	public:
		EditorApplication() : Application() {
			//// Open first the module -> Project CreationSetUp
			//// Once the Porject is selected, remove the module and add the EditorModule

			AddModule(new ProjectSetupModule());
		}
	};
}

int main() {
	Loopie::Application* app = new Loopie::EditorApplication();
	app->Run();
	delete app;
}