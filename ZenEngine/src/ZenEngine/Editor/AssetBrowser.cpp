#include "AssetBrowser.h"

#include <stb_image.h>
#include "EditorGUI.h"

#define ICON_RESOLUTION 512

namespace ZenEngine
{
    void AssetBrowser::OnRegister()
    {
        SetAssetIcon("ZenEngine::ShaderAsset", "resources/Icons/ShaderIcon.png");
        SetAssetIcon("ZenEngine::StaticMesh", "resources/Icons/MeshIcon.png");

        Texture2D::Properties props;
        props.Width = ICON_RESOLUTION;
        props.Height = ICON_RESOLUTION;
        props.Format = Texture2D::Format::RGBA8;
        mFolderIcon = Texture2D::Create(props);
        auto img = LoadIcon("resources/Icons/FolderIcon.png");
        mFolderIcon->SetData(img, ICON_RESOLUTION * ICON_RESOLUTION * 4);
        FreeIcon(img);
    
        mDefaultFileIcon = Texture2D::Create(props);
        img = LoadIcon("resources/Icons/FileIcon.png");
        mDefaultFileIcon->SetData(img, ICON_RESOLUTION * ICON_RESOLUTION * 4);
        FreeIcon(img);
    }


    // begin spaghetti code
    // after all im italian
    void AssetBrowser::OnRenderWindow()
    {
        static float padding = 16.0f;
		static float thumbnailSize = 128.0f;
		float cellSize = thumbnailSize + padding;

		float panelWidth = ImGui::GetContentRegionAvail().x;
		int columnCount = (int)(panelWidth / cellSize);
		if (columnCount < 1)
			columnCount = 1;

        ImGui::PushStyleColor(ImGuiCol_Button, { 0, 0, 0, 0 });
        std::filesystem::path currentPath = "";
        for (auto it = mCurrentDirectory.begin(); it != mCurrentDirectory.end(); ++it)
        {
            if (it != mCurrentDirectory.begin())
            {                
                ImGui::Text(">");
                ImGui::SameLine();
            }
            auto dir = *it;
            currentPath /= dir;
            if (ImGui::Button(dir.string().c_str()))
            {
                mCurrentDirectory = currentPath;
            }
            if (ImGui::BeginDragDropTarget())
            {
                if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("FileItem"))
                {
                    std::filesystem::path targetFile = reinterpret_cast<const char *>(payload->Data);
                    Move(targetFile, currentPath);
                }
                ImGui::EndDragDropTarget();
            }
            ImGui::SameLine();
        }
        ImGui::PopStyleColor();

        if (mCurrentDirectory != mBaseDirectory && ImGui::Button("Back"))
        {
            mCurrentDirectory = mCurrentDirectory.parent_path();
        }
        ImGui::NewLine();

        if (ImGui::BeginPopupContextWindow())
        {
            if (ImGui::MenuItem("Create Folder"))
                CreateFolder();

            ImGui::EndPopup();
        }

		ImGui::Columns(columnCount, 0, false);

		for (auto& directoryEntry : std::filesystem::directory_iterator(mCurrentDirectory))
		{
			const auto& path = directoryEntry.path();

            // ignore metadata files
            if (path.extension().string() == ".zmeta") continue;
			
            std::string filenameString = path.filename().string();
			
			ImGui::PushID(filenameString.c_str());

			std::shared_ptr<Texture2D> icon;
            bool isDirectory = std::filesystem::is_directory(path);
            bool isRenamingThisFile = mCurrentlyRenaming.has_value() && mCurrentlyRenaming == path;
            const AssetClass *clas = nullptr;
            std::optional<UUID> id = std::nullopt;

            if (isDirectory)
            {
                icon = mFolderIcon;
            }
            else
            {
                clas = AssetManager::Get().QueryFilepathAssetClass(path);
                id = AssetManager::Get().QueryFilepathAssetId(path);
                if (clas != nullptr && mAssetIcons.contains(clas->Name) && mAssetIcons[clas->Name] != nullptr)
                {
                    icon = mAssetIcons[clas->Name];
                }
                else
                {
                    // display a default icon here
                    icon = mDefaultFileIcon;
                }
            }
			
            ImGui::PushStyleColor(ImGuiCol_Button, { 0, 0, 0, 0 });
			ImGui::ImageButton(reinterpret_cast<ImTextureID>(icon->GetRendererID()), { thumbnailSize, thumbnailSize });
			ImGui::PopStyleColor();
            if (isDirectory)
            {
                if (ImGui::BeginDragDropTarget())
                {
                    if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("FileItem"))
                    {
                        std::filesystem::path targetFile = reinterpret_cast<const char *>(payload->Data);
                        Move(targetFile, path);
                    }
                    ImGui::EndDragDropTarget();
                }
            }
            else
            {

                if (ImGui::BeginDragDropSource())
                {
                    if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl))
                    {
                        if (id.has_value())
                        { 
                            ImGui::SetDragDropPayload(clas->Name.c_str(), (uint64_t*)&id, sizeof(uint64_t));
                        }
                    }
                    else
                    {
                        std::string stringPayload = path.string();
                        ImGui::SetDragDropPayload("FileItem", stringPayload.c_str(), stringPayload.size() + 1);
                    }

                    ImGui::EndDragDropSource();
                }
            }

            if (ImGui::BeginPopupContextItem())
            {
                if (ImGui::MenuItem("Delete"))
                {
                    std::filesystem::remove(path);
                    AssetManager::Get().BuildAssetDatabase();
                }
                
                if (ImGui::MenuItem("Rename"))
                    StartRenaming(path);

                ImGui::EndPopup();
            }

			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				if (directoryEntry.is_directory())
					mCurrentDirectory /= path.filename();
                else if (id.has_value())
                {
                    //Editor::Get().OpenAsset();
                }
			}
            if (isRenamingThisFile)
            {
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetColumnWidth()/2 - ImGui::CalcTextSize(mNewFilename.c_str()).x/2 - ImGui::GetStyle().ItemSpacing.x);
                if (ImGui::IsKeyDown(ImGuiKey_Enter) || ImGui::IsKeyDown(ImGuiKey_Escape) ||  ImGui::IsMouseClicked(0))
                {
                    DoneRenaming();
                }
                else
                {
                    ImGui::SetKeyboardFocusHere(0);
                }
                EditorGUI::InputTextNoLayout("", mNewFilename);
            }
            else
            {
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetColumnWidth()/2 - ImGui::CalcTextSize(path.stem().string().c_str()).x/2 - ImGui::GetStyle().ItemSpacing.x);
			    ImGui::TextWrapped(path.stem().string().c_str());
            }

			ImGui::NextColumn();

			ImGui::PopID();
		}

		ImGui::Columns(1);
    }

    void AssetBrowser::SetAssetIcon(const std::string &inAssetClassName, const std::filesystem::path &inIconPath)
    {
        Texture2D::Properties props;
        props.Width = ICON_RESOLUTION;
        props.Height = ICON_RESOLUTION;
        props.Format = Texture2D::Format::RGBA8;
        mAssetIcons[inAssetClassName] = Texture2D::Create(props);
        auto img = LoadIcon(inIconPath);
        mAssetIcons[inAssetClassName]->SetData(img, ICON_RESOLUTION * ICON_RESOLUTION * 4);
        FreeIcon(img);
    }

    uint8_t *AssetBrowser::LoadIcon(const std::filesystem::path &inIconPath)
    {
        int width;
        int height;
        int channels;
        uint8_t *ret = stbi_load(inIconPath.string().c_str(), &width, &height, &channels, 4);
        ZE_ASSERT_CORE_MSG(width == ICON_RESOLUTION && height == ICON_RESOLUTION && channels == 4, "An icon should be {0}x{0} pixels and have 4 channels", ICON_RESOLUTION);
        return ret;
    }

    void AssetBrowser::FreeIcon(uint8_t *inImage)
    {
        stbi_image_free(inImage);
    }

    void AssetBrowser::CreateFolder()
    {
        for (int i = 0; true; ++i)
        {
            std::filesystem::path path = mCurrentDirectory / ((i==0)?"NewDirectory" : fmt::format("NewDirectory{}", i));
            if (!std::filesystem::exists(path))
            {
                std::filesystem::create_directory(path);
                StartRenaming(path);
                break;
            }
        }
    }

    void AssetBrowser::DoneRenaming()
    {
        if (std::filesystem::is_directory(mCurrentlyRenaming.value()))
            std::filesystem::rename(mCurrentlyRenaming.value(), mCurrentlyRenaming->parent_path() / mNewFilename);
        else
        {
            std::filesystem::path destination = fmt::format("{}{}", mNewFilename, mCurrentlyRenaming->extension().string());
            std::filesystem::rename(mCurrentlyRenaming.value(), mCurrentlyRenaming->parent_path() / destination);
            // check for meta files
            auto metaPath = mCurrentlyRenaming->replace_extension(".zmeta");
            if (std::filesystem::exists(metaPath))
                std::filesystem::rename(mCurrentlyRenaming.value(), mCurrentlyRenaming->parent_path() / destination.replace_extension(".zmeta"));
            AssetManager::Get().BuildAssetDatabase();
        }
        mNewFilename = "";
        mCurrentlyRenaming.reset();
    }

    void AssetBrowser::StartRenaming(const std::filesystem::path &inFilepath)
    {
        mCurrentlyRenaming = inFilepath;
        if (std::filesystem::is_directory(inFilepath))
            mNewFilename = inFilepath.filename().string();
        else
            mNewFilename = inFilepath.stem().string();
    }

    void AssetBrowser::Move(const std::filesystem::path &inOld, const std::filesystem::path &inDirectory)
    {
        std::filesystem::path targetPath = inOld;
        std::filesystem::rename(targetPath, inDirectory / targetPath.filename());
        // check if meta file exists and move it too
        auto metaFile = targetPath.replace_extension(".zmeta");
        if (std::filesystem::exists(metaFile))
        {
            std::filesystem::rename(metaFile, inDirectory / metaFile.filename());
        }
        AssetManager::Get().BuildAssetDatabase();
    }
}