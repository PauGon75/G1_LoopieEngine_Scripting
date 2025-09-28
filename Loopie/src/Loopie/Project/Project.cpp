#include "Project.h"

#include "Loopie/Files/DirectoryManager.h"

namespace Loopie {
	bool Project::Create(const std::filesystem::path& projectPath, const std::string& name) {

		if (DirectoryManager::Contains(projectPath / name)) {
			return false;
		}

		m_projectPath = DirectoryManager::CreateFolder(projectPath, name);
		m_assetsPath = DirectoryManager::CreateFolder(m_projectPath, "Assets");
		m_congifPath = DirectoryManager::CreateFile(m_projectPath, "project", ".config");

		/// Maybe some config Files???? Once Scene Exists a default One
		return true;
	}

	bool Project::Open(const std::filesystem::path& projectPath) {
		if (!DirectoryManager::Contains(projectPath) || !DirectoryManager::Contains(projectPath / "project.config")) {
			return false;
		}

		m_projectPath = projectPath;
		m_assetsPath = DirectoryManager::CreateFolder(m_projectPath, "Assets");
		m_congifPath = projectPath / "project.config";

		/// Maybe read/save config Files????
		return true;
	}
}