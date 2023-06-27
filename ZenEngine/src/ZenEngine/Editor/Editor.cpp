#include "Editor.h"

#include <imgui.h>
#include "ZenEngine/Core/Game.h"
#include "ZenEngine/Asset/AssetManager.h"

#include "FileDialog.h"

#include "EditorViewport.h"
#include "AssetManagerDatabase.h"
#include "SceneHierarchy.h"
#include "PropertiesWindow.h"
#include "AssetBrowser.h"
#include "MeshEditor.h"

namespace ZenEngine
{
    Editor *Editor::sEditorInstance = nullptr;

    Editor::Editor() : Layer("ZenEditor")
    {
        ZE_ASSERT_CORE_MSG(sEditorInstance == nullptr, "An Editor already exists!");
        sEditorInstance = this;
    }

    void Editor::OnAttach()
    {
        ZE_CORE_INFO("Attaching editor");
        RegisterEditorWindow(std::make_unique<EditorViewport>());
        RegisterEditorWindow(std::make_unique<AssetManagerDatabase>());
        RegisterEditorWindow(std::make_unique<SceneHierarchy>());
        RegisterEditorWindow(std::make_unique<PropertiesWindow>());
        RegisterEditorWindow(std::make_unique<AssetBrowser>());
        RegisterAssetEditor<MeshEditor>();
    }

    void Editor::OnRenderEditorGUI()
    {
        static bool dockspaceOpen = true;
        static bool optFullscreenPersistant = true;
        bool optFullscreen = optFullscreenPersistant;
        static ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_None;

        // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
        // because it would be confusing to have two docking targets within each others.
        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        if (optFullscreen)
        {
            ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->Pos);
            ImGui::SetNextWindowSize(viewport->Size);
            ImGui::SetNextWindowViewport(viewport->ID);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
            windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        }

        // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
        if (dockspaceFlags & ImGuiDockNodeFlags_PassthruCentralNode)
            windowFlags |= ImGuiWindowFlags_NoBackground;

        // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
        // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive, 
        // all active windows docked into it will lose their parent and become undocked.
        // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise 
        // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("DockSpace", &dockspaceOpen, windowFlags);
        ImGui::PopStyleVar();

        if (optFullscreen)
            ImGui::PopStyleVar(2);

        // DockSpace
        ImGuiIO& io = ImGui::GetIO();
        ImGuiStyle& style = ImGui::GetStyle();
        float minWinSizeX = style.WindowMinSize.x;
        style.WindowMinSize.x = 370.0f;
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
        {
            ImGuiID dockspaceId = ImGui::GetID("DockSpace");
            ImGui::DockSpace(dockspaceId, ImVec2(0.0f, 0.0f), dockspaceFlags);
        }

        style.WindowMinSize.x = minWinSizeX;

        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("New Scene")) NewScene();

                if (ImGui::MenuItem("Save Scene")) ZE_CORE_WARN("TODO: SaveLevel");

                if (ImGui::MenuItem("Save Scene As...")) ZE_CORE_WARN("TODO: SaveLevelAs");

                ImGui::Separator();

                if (ImGui::MenuItem("Import...")) ImportClicked();

                ImGui::Separator();

                if (ImGui::MenuItem("Exit"))
                    Game::Get().Close();
                
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("View"))
            {
                for (const auto &window : mEditorWindows)
                {
                    if (window->HasViewMenuItem() && window->CanBeClosed())
                    {
                        if (ImGui::MenuItem(window->GetName().c_str())) window->Open();
                    }
                }
                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }
        for (const auto &window : mEditorWindows)
        {
            if (!window->IsOpen()) continue;

            window->OnInitializeStyle();
            if (window->CanBeClosed())
                ImGui::Begin(window->GetName().c_str(), window->GetOpenHandle());
            else
                ImGui::Begin(window->GetName().c_str());
            
            window->OnRenderWindow();	
            ImGui::End();
            window->OnClearStyle();
        }

        std::vector<UUID> toBeRemoved;
        for (const auto &[id, assetEditor] : mAssetEditors)
        {
            if (!assetEditor->IsOpen())
            {
                if (assetEditor->IsSaved())
                {
                    toBeRemoved.push_back(id);
                    continue;
                }
                else
                {
                    // warn here that the asset is not saved
                }
            }
            assetEditor->OnInitializeStyle();
            ImGui::Begin(fmt::format("{} ## {}", assetEditor->GetName(), id).c_str() , assetEditor->GetOpenHandle());
            assetEditor->OnRenderWindow();
            ImGui::End();
            assetEditor->OnClearStyle();
        }
        for (auto id : toBeRemoved)
            mAssetEditors.erase(id);
        ImGui::End();
    }

    void Editor::OnUpdate(float inDeltaTime)
    {
        for (auto &window : mEditorWindows)
        {
            if (!window->IsOpen()) continue;
            window->OnUpdate(inDeltaTime);
        }
        if (mActiveScene != nullptr)
            mActiveScene->OnUpdate(inDeltaTime);
    }

    void Editor::OnRender(float inDeltaTime)
    {
        if (mActiveScene != nullptr)
            mActiveScene->OnRender(inDeltaTime);
    }

    void Editor::OnEvent(const std::unique_ptr<Event> &inEvent)
    {
        for (auto &window : mEditorWindows)
        {
            if (!window->IsOpen()) continue;
            window->OnEvent(inEvent);
        }
    }

    void Editor::BeginRenderGame()
    {
        for (auto &window : mEditorWindows)
            window->OnBeginRenderGame();

        for (auto &[_, assetEditor] : mAssetEditors)
            assetEditor->OnBeginRenderGame();
    }

    void Editor::EndRenderGame()
    {
        for (auto &window : mEditorWindows)
            window->OnEndRenderGame();

        for (auto &[_, assetEditor] : mAssetEditors)
            assetEditor->OnEndRenderGame();
    }

    void Editor::RegisterEditorWindow(std::unique_ptr<EditorWindow> inWindow)
    {
        inWindow->OnRegister();
        ZE_CORE_INFO("Registered editor window {}", inWindow->GetName());
        mEditorWindows.push_back(std::move(inWindow));
    }

    void Editor::OpenAsset(UUID inUUID)
    {
        const char *className = AssetManager::Get().GetAssetClassName(inUUID);
        if (!mAssetEditors.contains(inUUID)) 
        {
            mAssetEditors[inUUID] = mAssetEditorInstatiators[className](inUUID);
            mAssetEditors[inUUID]->Open();
        }

        // maybe here focus window
    }

    void Editor::ImportClicked()
    {
        std::string filename = FileDialog::OpenFile("");
        if (!filename.empty())
            AssetManager::Get().Import(filename);
    }

    void Editor::NewScene()
    {
        mActiveScene = std::make_shared<Scene>();
    }

}