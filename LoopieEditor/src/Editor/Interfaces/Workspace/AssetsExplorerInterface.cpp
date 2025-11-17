#include "AssetsExplorerInterface.h"

#include "Loopie/Core/Application.h"
#include "Loopie/Core/Log.h"
#include "Loopie/Files/DirectoryManager.h"
#include "Loopie/Resources/ResourceManager.h"
#include "Loopie/Resources/Types/Texture.h"

#include "Loopie/Importers/TextureImporter.h"
#include "Loopie/Importers/MaterialImporter.h"
#include "Loopie/Importers/MeshImporter.h"

#include <imgui.h>
#include <imgui_internal.h>

namespace Loopie {
	AssetsExplorerInterface::AssetsExplorerInterface() {

		
	}

	void AssetsExplorerInterface::Init()
	{	
		std::vector<std::string> iconsToLoad = {
			"assets/icons/icon_file.png",
			"assets/icons/icon_folder.png",
			"assets/icons/icon_folderFill.png"
		};

		std::vector<Metadata> iconsToLoadMetadatas;
		for (size_t i = 0; i < iconsToLoad.size(); i++)
		{
			Metadata& meta = AssetRegistry::GetOrCreateMetadata(iconsToLoad[i]);
			TextureImporter::ImportImage(iconsToLoad[i], meta);
			iconsToLoadMetadatas.emplace_back(meta);
		}
		
		m_fileIcon = ResourceManager::GetTexture(iconsToLoadMetadatas[0]);
		m_emptyFolderIcon = ResourceManager::GetTexture(iconsToLoadMetadatas[1]);
		m_folderIcon = ResourceManager::GetTexture(iconsToLoadMetadatas[2]);

		const Project& project = Application::GetInstance().m_activeProject;
		GoToDirectory(project.GetAssetsPath());
	}

	void AssetsExplorerInterface::Update(float dt, const InputEventManager& inputEvent) {
		if (m_focused)
			HotKeysControls(inputEvent);

		if (inputEvent.HasEvent(SDL_EVENT_WINDOW_FOCUS_GAINED))
			Refresh();
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
				DrawDirectoryTree();
			}
			ImGui::EndChild();

			m_Splitter.Place(avail);

			if (ImGui::BeginChild("Assets", ImVec2(sizeRight, avail.y), 0, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse)) {
				ImGui::Separator();
				DrawPathBar();
				ImGui::Separator();

				float footerHeight = ImGui::GetFrameHeightWithSpacing();
				if (ImGui::BeginChild("FilesScrollView", ImVec2(0, -footerHeight), 0, 0)) {
					m_focused = ImGui::IsWindowHovered(ImGuiHoveredFlags_None);
					if (m_focused)
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

	void AssetsExplorerInterface::HotKeysControls(const InputEventManager& inputEvent)
	{
		if (m_selectedFile.empty()) {
			return;
		}

		bool actionDone = false;
		if (inputEvent.GetKeyStatus(SDL_SCANCODE_DELETE) == KeyState::DOWN) {
			DirectoryManager::Delete(m_selectedFile);
			actionDone = true;
		}

		if (inputEvent.GetKeyWithModifier(SDL_SCANCODE_C, KeyModifier::CTRL)) {
			/// Copy
			actionDone = true;
		}

		if (inputEvent.GetKeyWithModifier(SDL_SCANCODE_V, KeyModifier::CTRL)) {
			/// Paste
			actionDone = true;
		}

		if (inputEvent.GetKeyWithModifier(SDL_SCANCODE_X, KeyModifier::CTRL)) {
			/// Cut
			actionDone = true;
		}

		if(actionDone)
			Refresh();
	}

	void AssetsExplorerInterface::GetExternalFile()
	{
		if (m_isSearching)
			return;

		Application& app = Application::GetInstance();
		InputEventManager& inputEvent = app.GetInputEvent();
		if (!inputEvent.HasFileBeenDropped())
			return;

		std::filesystem::path assetsPath = app.m_activeProject.GetAssetsPath();
		std::filesystem::path currentPath = m_currentDirectory;

		const std::vector<const char*>& droppedFiles = inputEvent.GetDroppedFiles();
		for (size_t i = 0; i < droppedFiles.size(); i++)
		{
			std::filesystem::path droppedPath = droppedFiles[i];

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
				DirectoryManager::Move(droppedPath += ".meta", targetPath += ".meta");
			}
			else {
				DirectoryManager::Copy(droppedPath, targetPath);
			}
		}


		Refresh(true, true, false);
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

		Refresh(false, true, false);
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
			Refresh(false, false, true);
		}
	}


	void AssetsExplorerInterface::ClearSearch()
	{
		m_isSearching = false;
		m_searchBuffer[0] = '\0';
		Refresh(false, false, true);
	}

	void AssetsExplorerInterface::DrawDirectoryTree()
	{
		if (m_dirtyFoldersTree)
			RebuildTreeFolderCache();

		DrawDirectoryTreeNode(m_cachedTreeFolder, 0);
	}

	void AssetsExplorerInterface::DrawDirectoryTreeNode(const CachedDirectoryNode& node, int indent)
	{
		bool isRoot = (indent == 0);

		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth;

		if (node.isEmpty)
			flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
		else
			flags |= ImGuiTreeNodeFlags_OpenOnArrow;

		if (node.path == m_currentDirectory)
			flags |= ImGuiTreeNodeFlags_Selected;

		std::string name = node.path.filename().string();
		if (name.empty())
			name = node.path.root_name().string();

		bool opened = ImGui::TreeNodeEx(name.c_str(), flags);

		if (!isRoot)
			DragFile(node.path.string());
		DropFile(node.path.string());

		if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			GoToDirectory(node.path);

		if (opened && !node.isEmpty)
		{
			for (const auto& child : node.children)
				DrawDirectoryTreeNode(child, indent + 1);

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
		int columnCount = std::max(1, (int)(availX / cellSize));

		ImVec2 iconSize = ImVec2((float)thumbnailSize, (float)thumbnailSize);

		ImGui::Columns(columnCount, nullptr, false);

		std::vector<std::filesystem::path>* filesToShow;
		
		if (m_isSearching) {
			if (m_dirtyFileSearch)
				RebuildSearchFilesCache();
			filesToShow = &m_cachedSearchFiles;
		}
		else {
			if (m_dirtyFolderFiles)
				RebuildFolderFilesCache();
			filesToShow = &m_cachedFolderFiles;
		}

		for (size_t i = 0; i < filesToShow->size(); ++i) {

			const auto& directory = (*filesToShow)[i];
			const bool isDir = std::filesystem::is_directory(directory);

			ImGui::PushID(i);

			std::shared_ptr<Texture> icon = nullptr;
			if (isDir) {
				bool isEmpty = std::filesystem::is_empty(directory);
				icon = isEmpty ? m_emptyFolderIcon : m_folderIcon;
			}
			else
				icon = m_fileIcon;

			ImVec2 cursor = ImGui::GetCursorScreenPos();
			ImGui::Image((ImTextureID)icon->GetRendererId(), iconSize);

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
			if (MetadataRegistry::IsMetadataFile(entry.path()))
				continue;
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
	void AssetsExplorerInterface::DragFile(const std::string& from)
	{
		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceNoDisableHover | ImGuiDragDropFlags_SourceAllowNullID))
		{
			std::string filePathStr = from;
			ImGui::SetDragDropPayload("ASSET_EXPLORER_FILE", filePathStr.c_str(), filePathStr.size() + 1);
			ImGui::EndDragDropSource();
		}
	}
	void AssetsExplorerInterface::DropFile(const std::string& to)
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
				DirectoryManager::Move(fromPath += ".meta", newPath += ".meta");

				Refresh();
			}
			ImGui::EndDragDropTarget();
		}
	}
	void AssetsExplorerInterface::Refresh(bool folderTree, bool folderFiles, bool searchFiles)
	{
		AssetRegistry::RefreshAssetRegistry();

		if(!m_dirtyFoldersTree)
			m_dirtyFoldersTree = folderTree;
		if(!m_dirtyFolderFiles)
			m_dirtyFolderFiles = folderFiles;
		if(!m_dirtyFileSearch)
			m_dirtyFileSearch = searchFiles;
	}

	void AssetsExplorerInterface::RebuildTreeFolderCache()
	{
		const Project& project = Application::GetInstance().m_activeProject;
		m_cachedTreeFolder = BuildDirectoryNode(project.GetAssetsPath());
		m_dirtyFoldersTree = false;
	}

	AssetsExplorerInterface::CachedDirectoryNode AssetsExplorerInterface::BuildDirectoryNode(const std::filesystem::path& directory)
	{
		CachedDirectoryNode node;
		node.path = directory;

		for (auto& entry :
			std::filesystem::directory_iterator(directory, std::filesystem::directory_options::skip_permission_denied))
		{
			if (entry.is_directory())
			{
				node.isEmpty = false;
				node.children.push_back(BuildDirectoryNode(entry.path()));
			}
		}
		return node;
	}

	void AssetsExplorerInterface::RebuildFolderFilesCache()
	{
		m_cachedFolderFiles.clear();
		if (!std::filesystem::exists(m_currentDirectory))
			return;

		for (auto& entry :
			std::filesystem::directory_iterator(m_currentDirectory, std::filesystem::directory_options::skip_permission_denied))
		{
			if (!MetadataRegistry::IsMetadataFile(entry.path())) {
				m_cachedFolderFiles.push_back(entry.path());
			}
		}

		m_dirtyFolderFiles = false;
	}
	void AssetsExplorerInterface::RebuildSearchFilesCache()
	{
		m_cachedSearchFiles.clear();

		const Project& project = Application::GetInstance().m_activeProject;

		std::string query = m_searchBuffer;
		std::transform(query.begin(), query.end(), query.begin(), ::tolower);

		for (auto& entry :
			std::filesystem::recursive_directory_iterator(project.GetAssetsPath(), std::filesystem::directory_options::skip_permission_denied))
		{
			if (MetadataRegistry::IsMetadataFile(entry.path()))
				continue;

			std::string name = entry.path().filename().string();
			std::transform(name.begin(), name.end(), name.begin(), ::tolower);

			if (name.find(query) != std::string::npos)
				m_cachedSearchFiles.push_back(entry.path());
		}

		m_dirtyFileSearch = false;
	}
}