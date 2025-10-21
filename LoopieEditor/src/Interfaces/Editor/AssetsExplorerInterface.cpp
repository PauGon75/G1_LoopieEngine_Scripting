#include "AssetsExplorerInterface.h"

#include "Loopie/Core/Application.h"
#include "Loopie/Resources/ResourceDatabase.h"
#include "Loopie/Importers/TextureImporter.h"
#include "Loopie/Resources/Types/Texture.h"

#include <imgui.h>
#include <imgui_internal.h>

namespace Loopie {
	AssetsExplorerInterface::AssetsExplorerInterface() {

		
	}

	void AssetsExplorerInterface::Init()
	{
		std::string iconPath = "assets/icons/icon_file.png";
		if (!AssetRegistry::AssetExists(iconPath)) {
			std::string cacheFile = TextureImporter::LoadImage(iconPath);
			AssetMetadata metadata = AssetRegistry::CreateAssetMetadata(iconPath, cacheFile);
			AssetRegistry::RegisterAsset(metadata);
		}
		m_fileIcon = ResourceDatabase::LoadResource<Texture>(AssetRegistry::GetUUIDFromSourcePath(iconPath)[0]);

		iconPath = "assets/icons/icon_folder.png";
		if (!AssetRegistry::AssetExists(iconPath)) {
			std::string cacheFile = TextureImporter::LoadImage(iconPath);
			AssetMetadata metadata = AssetRegistry::CreateAssetMetadata(iconPath, cacheFile);
			AssetRegistry::RegisterAsset(metadata);
		}
		m_emptyFolderIcon = ResourceDatabase::LoadResource<Texture>(AssetRegistry::GetUUIDFromSourcePath(iconPath)[0]);

		iconPath = "assets/icons/icon_folderFill.png";
		if (!AssetRegistry::AssetExists(iconPath)) {
			std::string cacheFile = TextureImporter::LoadImage(iconPath);
			AssetMetadata metadata = AssetRegistry::CreateAssetMetadata(iconPath, cacheFile);
			AssetRegistry::RegisterAsset(metadata);
		}
		m_folderIcon = ResourceDatabase::LoadResource<Texture>(AssetRegistry::GetUUIDFromSourcePath(iconPath)[0]);

		const Project& project = Application::GetInstance().m_activeProject;
		GoToDirectory(project.GetAssetsPath());
	}

	void AssetsExplorerInterface::Render() {
		if (ImGui::Begin("Assets Explorer", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse)){
			const Project& project = Application::GetInstance().m_activeProject;
			if (project.IsEmpty()) {
				ImGui::End();
				return;
			}

			ImVec2 avail = ImGui::GetContentRegionAvail();
			float sizeLeft;
			float sizeRight;
			m_Splitter.GetSizes(avail, sizeLeft, sizeRight);

			DrawSearchBar();
			ImGui::Separator();

			ImGui::BeginChild("AssetsTree", ImVec2(sizeLeft, avail.y), 0, ImGuiWindowFlags_HorizontalScrollbar);
			DrawDirectoryTree(project.GetAssetsPath());
			ImGui::EndChild();

			m_Splitter.Place(avail);

			if (ImGui::BeginChild("Assets", ImVec2(sizeRight, avail.y), 0, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse)) {
				ImGui::Separator();
				DrawPathBar();
				ImGui::Separator();

				if (ImGui::BeginChild("FilesScrollView", ImVec2(0, 0), 0, 0)) {
					DrawFolderContent();
				}
				ImGui::EndChild();
			}	
			ImGui::EndChild();

		}
		ImGui::End();
	}

	void AssetsExplorerInterface::GoToDirectory(const std::filesystem::path& directory)
	{
		m_currentDirectory = directory;

		const Project& project = Application::GetInstance().m_activeProject;
		m_relativePath = std::filesystem::relative(directory, project.GetAssetsPath());

		m_relativePathSteps.clear();
		m_relativePathSteps.push_back(project.GetAssetsPath().filename().string());

		if (m_relativePath != ".") {
			for (auto& p : m_relativePath) {
				m_relativePathSteps.push_back(p.string());
			}
		}
	}

	void AssetsExplorerInterface::GoToFile(const std::filesystem::path& filePath)
	{
		m_selectedFile = filePath;
		GoToDirectory(filePath.parent_path());
	}

	void AssetsExplorerInterface::DrawSearchBar()
	{
		float fullWidth = ImGui::GetContentRegionAvail().x;
		float searchBarWidthPercentage = 30.0f;
		float searchBarTotalWidth = fullWidth * (searchBarWidthPercentage / 100.0f);

		float buttonWidth = ImGui::GetFontSize() * 1.5f;
		float spacing = ImGui::GetStyle().ItemSpacing.x;

		float inputWidth = searchBarTotalWidth - (buttonWidth + spacing);

		float cursorX = ImGui::GetCursorPosX();
		ImGui::SetCursorPosX(cursorX + fullWidth - searchBarTotalWidth);

		if (ImGui::Button("X", ImVec2(buttonWidth, 0))) {
			ClearSearch();
		}

		ImGui::SameLine(0, spacing);

		ImGui::SetNextItemWidth(inputWidth);
		if (ImGui::InputTextWithHint("##search", "Search files...", m_searchBuffer, sizeof(m_searchBuffer))) {
			m_isSearching = strlen(m_searchBuffer) > 0;
		}
	}


	void AssetsExplorerInterface::ClearSearch()
	{
		m_isSearching = false;
		m_searchBuffer[0] = '\0';
	}

	void AssetsExplorerInterface::DrawDirectoryTree(const std::filesystem::path& directory, int indent)
	{
		if (!std::filesystem::is_directory(directory))
			return;
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_OpenOnArrow;

		bool hasChildren = !std::filesystem::is_empty(directory);
		if (!hasChildren)
			flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
		if (directory == m_currentDirectory)
			flags |= ImGuiTreeNodeFlags_Selected;

		std::string folderName = directory.filename().string();
		if (folderName.empty())
			folderName = directory.root_name().string();

		bool opened = ImGui::TreeNodeEx(folderName.c_str(), flags);

		if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
			GoToDirectory(directory);

		if (opened && hasChildren)
		{
			for (auto& entry : std::filesystem::directory_iterator(
				directory, std::filesystem::directory_options::skip_permission_denied))
			{
				const auto& path = entry.path();
				std::string name = path.filename().string();

				if (entry.is_directory())
				{
					DrawDirectoryTree(path, indent + 1);
				}
				else
				{
					ImGuiTreeNodeFlags leafFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanAvailWidth;

					if (path == m_selectedFile)
						leafFlags |= ImGuiTreeNodeFlags_Selected;

					ImGui::TreeNodeEx(name.c_str(), leafFlags);

					if (ImGui::IsItemClicked()) {
						GoToFile(path);
					}
				}
			}
			ImGui::TreePop();
		}
	}

	void AssetsExplorerInterface::DrawPathBar()
	{
		const Project& project = Application::GetInstance().m_activeProject;

		for (size_t i = 0; i < m_relativePathSteps.size(); ++i)
		{
			if (i != 0) {
				ImGui::SameLine();
				ImGui::Text("/");
				ImGui::SameLine();
			}

			ImVec2 textSize = ImGui::CalcTextSize(m_relativePathSteps[i].c_str());
			if (ImGui::Selectable(m_relativePathSteps[i].c_str(), false, 0, textSize))
			{
				std::filesystem::path newPath = project.GetAssetsPath();
				for (size_t j = 1; j <= i; ++j)
					newPath /= m_relativePathSteps[j];
				GoToDirectory(newPath);
			}
		}
	}

	void AssetsExplorerInterface::DrawFolderContent()
	{

		int cellSize = thumbnailSize + padding;
		float availX = ImGui::GetContentRegionAvail().x;
		int columnCount = (int)(availX / cellSize);
		if (columnCount < 1) columnCount = 1;

		ImGui::Columns(columnCount, 0, false);

		std::vector<std::filesystem::path> filesToShow;

		if (m_isSearching) {
			filesToShow = GetFilteredFiles();

			if (filesToShow.empty()) {
				ImGui::Columns(1);
				ImGui::TextDisabled("No results found");
				return;
			}
		}
		else {
			for (auto& entry : std::filesystem::directory_iterator(m_currentDirectory, std::filesystem::directory_options::skip_permission_denied))
			{
				filesToShow.push_back(entry.path());
			}
		}

		for (auto& filePath : filesToShow) {
			std::string fileName = filePath.filename().string();

			ImGui::PushID(fileName.c_str());
			bool isDir = std::filesystem::is_directory(filePath);

			if (isDir) {
				bool isEmpty = std::filesystem::is_empty(filePath);
				if(isEmpty)
					ImGui::Image((ImTextureID)m_emptyFolderIcon->GetRenderId(), ImVec2((float)thumbnailSize, (float)thumbnailSize));
				else
					ImGui::Image((ImTextureID)m_folderIcon->GetRenderId(), ImVec2((float)thumbnailSize, (float)thumbnailSize));
			}
			else {
				ImGui::Image((ImTextureID)m_fileIcon->GetRenderId(), ImVec2((float)thumbnailSize, (float)thumbnailSize));
			}

			if (ImGui::IsItemHovered())
			{
				if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
					if (isDir) {
						m_isSearching = false;
						GoToDirectory(filePath);
					}
					else {
						/// Open
					}
				}
				else if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
					GoToFile(filePath);
				}
			}

			ImGui::Text(ImGuiHelpers::TruncateText(fileName, (float)thumbnailSize).c_str());

			ImGui::NextColumn();
			ImGui::PopID();
		}
		ImGui::Columns(1);
	}

	std::vector<std::filesystem::path> AssetsExplorerInterface::GetFilteredFiles()
	{
		std::vector<std::filesystem::path> results;
		const Project& project = Application::GetInstance().m_activeProject;

		for (auto& entry : std::filesystem::recursive_directory_iterator(project.GetAssetsPath(), std::filesystem::directory_options::skip_permission_denied))
		{
			const auto& filePath = entry.path();
			std::string fileName = filePath.filename().string();

			std::string lowerFile = fileName;
			std::string lowerQuery = m_searchBuffer;
			std::transform(lowerFile.begin(), lowerFile.end(), lowerFile.begin(), ::tolower);
			std::transform(lowerQuery.begin(), lowerQuery.end(), lowerQuery.begin(), ::tolower);

			if (lowerFile.find(lowerQuery) != std::string::npos) {
				results.push_back(filePath);
			}
		}
		return results;
	}
}