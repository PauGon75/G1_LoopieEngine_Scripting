#pragma once 

#include "Loopie/Resources/Types/Texture.h"
#include "src/Interfaces/Interface.h"
#include "src/ImGuiHelpers/ImGuiHelpers.h"

#include <filesystem>
#include <vector>
#include <memory>
#include <string>

namespace Loopie {
	class AssetsExplorerInterface : public Interface {
	public:
		AssetsExplorerInterface();
		~AssetsExplorerInterface() = default;

		void Init() override;
		void Render() override;

	private:
		void GetExternalFile();
		void GoToDirectory(const std::filesystem::path& directory, bool removeSearch = true);
		void SelectFile(const std::filesystem::path& filePath);
		void DrawSearchBar();
		void ClearSearch();
		void DrawDirectoryTree(const std::filesystem::path& directory, int indent = 0);
		void DrawPathBar();
		void DrawFolderContent();
		void DrawFooter();
		std::vector<std::filesystem::path> GetFilteredFiles();

		void DragFile(std::string from);
		void DropFile(std::string to);

	private:
		std::filesystem::path m_currentDirectory;
		std::filesystem::path m_selectedFile;

		std::filesystem::path m_relativePath;
		std::vector<std::string> m_relativePathSteps;

		char m_searchBuffer[256] = "";
		bool m_isSearching = false;

		int thumbnailSize = 64;
		int padding = 16;

		Splitter m_Splitter{ SplitterMode::VERTICAL, 0.15f, 5.0f, 3.0f, ImGuiCol_ButtonActive };

		std::shared_ptr<Texture> m_fileIcon;
		std::shared_ptr<Texture> m_emptyFolderIcon;
		std::shared_ptr<Texture> m_folderIcon;
	};
}