#include "AssetsExplorerInterface.h"

#include "Loopie/Core/Application.h"
#include "Loopie/Files/DirectoryManager.h"
#include "Loopie/Resources/AssetRegistry.h"
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
			std::string cacheFile = TextureImporter::ImportImage(iconPath);
			AssetMetadata metadata = AssetRegistry::CreateAssetMetadata(iconPath, cacheFile);
			AssetRegistry::RegisterAsset(metadata);
		}
		m_fileIcon = ResourceDatabase::LoadResource<Texture>(AssetRegistry::GetUUIDFromSourcePath(iconPath)[0]);

		iconPath = "assets/icons/icon_folder.png";
		if (!AssetRegistry::AssetExists(iconPath)) {
			std::string cacheFile = TextureImporter::ImportImage(iconPath);
			AssetMetadata metadata = AssetRegistry::CreateAssetMetadata(iconPath, cacheFile);
			AssetRegistry::RegisterAsset(metadata);
		}
		m_emptyFolderIcon = ResourceDatabase::LoadResource<Texture>(AssetRegistry::GetUUIDFromSourcePath(iconPath)[0]);

		iconPath = "assets/icons/icon_folderFill.png";
		if (!AssetRegistry::AssetExists(iconPath)) {
			std::string cacheFile = TextureImporter::ImportImage(iconPath);
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

			DrawSearchBar();
			ImGui::Separator();

			ImVec2 avail = ImGui::GetContentRegionAvail();
			float sizeLeft;
			float sizeRight;
			m_Splitter.GetSizes(avail, sizeLeft, sizeRight);

			if (ImGui::BeginChild("AssetsTree", ImVec2(sizeLeft, avail.y), 0, ImGuiWindowFlags_HorizontalScrollbar)) {
				ImGui::Separator();
				DrawDirectoryTree(project.GetAssetsPath());
			}
			ImGui::EndChild();

			m_Splitter.Place(avail);

			if (ImGui::BeginChild("Assets", ImVec2(sizeRight, avail.y), 0, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse)) {
				ImGui::Separator();
				DrawPathBar();
				ImGui::Separator();

				float footerHeight = ImGui::GetFrameHeightWithSpacing();
				if (ImGui::BeginChild("FilesScrollView", ImVec2(0, -footerHeight), 0, 0)) {
					if(ImGui::IsWindowHovered(ImGuiHoveredFlags_None))
						GetExternalFile();
					DrawFolderContent();
				}
				ImGui::EndChild();
				ImGui::Separator();
				DrawFooter();
			}	
			ImGui::EndChild();

		}
		ImGui::End();


	}

	void AssetsExplorerInterface::GetExternalFile()
	{
		Application& app = Application::GetInstance();
		InputEventManager& inputEvent = app.GetInputEvent();
		if (!inputEvent.HasFileBeenDropped())
			return;

		std::filesystem::path assetsPath = app.m_activeProject.GetAssetsPath();
		std::filesystem::path currentPath = m_currentDirectory;
		std::filesystem::path droppedPath = inputEvent.GetDroppedFile(0);

		currentPath = std::filesystem::absolute(currentPath);
		droppedPath = std::filesystem::absolute(droppedPath);

		std::filesystem::path targetPath = currentPath / droppedPath.filename();

		bool isInAssetsFolder = droppedPath.string().rfind(assetsPath.string(), 0) == 0;

		if (isInAssetsFolder) {
			if (droppedPath == currentPath) {
				const Project& project = Application::GetInstance().m_activeProject;
				GoToDirectory(assetsPath);
			}
			DirectoryManager::Move(droppedPath, targetPath);
		}
		else {
			DirectoryManager::Copy(droppedPath, targetPath);
		}

		//AssetRegistry::Reload();
	}

	void AssetsExplorerInterface::GoToDirectory(const std::filesystem::path& directory, bool removeSearch)
	{
		if (std::filesystem::is_directory(directory)) {
			m_currentDirectory = directory;
		}else
			m_currentDirectory = directory.parent_path();

		const Project& project = Application::GetInstance().m_activeProject;
		m_relativePath = std::filesystem::relative(m_currentDirectory, project.GetAssetsPath());

		if (removeSearch) {
			m_selectedFile.clear();
			ClearSearch();
		}

		m_relativePathSteps.clear();
		m_relativePathSteps.push_back(project.GetAssetsPath().filename().string());

		if (m_relativePath != ".") {
			for (auto& p : m_relativePath) {
				m_relativePathSteps.push_back(p.string());
			}
		}
	}

	void AssetsExplorerInterface::SelectFile(const std::filesystem::path& filePath)
	{
		m_selectedFile = filePath;
		if (m_isSearching) {
			GoToDirectory(m_selectedFile, false);
		}
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
		if (!exists(directory) || !is_directory(directory))
			return;

		bool isEmpty = true;
		for (const auto& entry : std::filesystem::directory_iterator(directory)) {
			if (entry.is_directory())
				isEmpty =  false;
		}

		bool isRoot = indent == 0;
		std::string name = directory.filename().string();

		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth;
		if (isEmpty)
			flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
		else
			flags |= ImGuiTreeNodeFlags_OpenOnArrow;

		if (directory == m_currentDirectory)
			flags |= ImGuiTreeNodeFlags_Selected;

		if (name.empty())
			name = directory.root_name().string();

		bool opened = ImGui::TreeNodeEx(name.c_str(), flags);

		if(!isRoot)
			DragFile(directory.string());
		DropFile(directory.string());

		if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
		{
			GoToDirectory(directory);
		}
		
		if (opened && !isEmpty)
		{
			for (auto& entry : std::filesystem::directory_iterator(directory, std::filesystem::directory_options::skip_permission_denied))
			{
				DrawDirectoryTree(entry.path(), indent+1);
			}
			ImGui::TreePop();
		}
	}

	void AssetsExplorerInterface::DrawPathBar()
	{
		const Project& project = Application::GetInstance().m_activeProject;

		if (m_isSearching) {
			ImGui::Text("Searching Files...");
			return;
		}

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
		const Project& project = Application::GetInstance().m_activeProject;

		if (ImGui::IsWindowHovered() && (ImGui::IsMouseClicked(ImGuiMouseButton_Left) || ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))) {
			SelectFile("");
		}

		if (!DirectoryManager::Contains(m_currentDirectory))
			GoToDirectory(project.GetAssetsPath());

		int cellSize = thumbnailSize + padding;
		float availX = ImGui::GetContentRegionAvail().x;
		int columnCount = (int)(availX / cellSize);
		if (columnCount < 1) 
			columnCount = 1;

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
				filesToShow.push_back(entry.path());
		}

		for (auto& directory : filesToShow) {
			ImGui::PushID(directory.string().c_str());
			bool isDir = std::filesystem::is_directory(directory);

			std::shared_ptr<Texture> icon = nullptr;
			if (isDir) {
				bool isEmpty = std::filesystem::is_empty(directory);
				icon = isEmpty ? m_emptyFolderIcon : m_folderIcon;
			}
			else
				icon = m_fileIcon;

			ImVec2 cursor = ImGui::GetCursorScreenPos();
			ImGui::Image((ImTextureID)icon->GetRenderId(), ImVec2((float)thumbnailSize, (float)thumbnailSize));

			if (ImGui::IsItemHovered()) {
				if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
					if (isDir) {
						GoToDirectory(directory);
						SelectFile(directory);
					}
					else
					{
						///Open
					}
				}
				else if (ImGui::IsItemClicked()) {
					SelectFile(directory);
				}
			}

			DragFile(directory.string());
			if (isDir)
				DropFile(directory.string());

			ImGui::SetCursorScreenPos(cursor);
			ImGuiSelectableFlags flags = ImGuiSelectableFlags_Disabled;
			if (m_selectedFile == directory)
				flags |= ImGuiSelectableFlags_Highlight;
			if (ImGui::Selectable("##file", false, flags, ImVec2((float)thumbnailSize, (float)thumbnailSize))) { }

			ImGuiHelpers::TextCentered(ImGuiHelpers::TruncateText(directory.stem().string(), (float)thumbnailSize).c_str(), 0.5f);

			ImGui::NextColumn();
			ImGui::PopID();
		}
		ImGui::Columns(1);
	}

	void AssetsExplorerInterface::DrawFooter()
	{
		if (!m_selectedFile.empty()) {
			const Project& project = Application::GetInstance().m_activeProject;
			std::filesystem::path path = project.GetAssetsPath();
			path = path.stem() / std::filesystem::relative(m_selectedFile, path);
			ImGui::TextDisabled(path.string().c_str());
		}
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
	void AssetsExplorerInterface::DragFile(std::string from)
	{
		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceNoDisableHover | ImGuiDragDropFlags_SourceAllowNullID))
		{
			std::string filePathStr = from;
			ImGui::SetDragDropPayload("ASSET_EXPLORER_FILE", filePathStr.c_str(), filePathStr.size() + 1);
			ImGui::EndDragDropSource();
		}
	}
	void AssetsExplorerInterface::DropFile(std::string to)
	{
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_EXPLORER_FILE"))
			{
				const char* from = (const char*)payload->Data;
				std::filesystem::path fromPath = from;
				std::filesystem::path newPath = to;
				newPath /= fromPath.filename().string();
				if (fromPath == m_currentDirectory) {
					const Project& project = Application::GetInstance().m_activeProject;
					GoToDirectory(newPath);
				}

				DirectoryManager::Move(fromPath, newPath);

				//AssetRegistry::Reload();
			}
			ImGui::EndDragDropTarget();
		}
	}
}