#include "Editor.h"

#include <imgui.h>
#include "ZenEngine/Core/Game.h"

#include "EditorViewport.h"

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
		ImGui::Begin("DockSpace Demo", &dockspaceOpen, windowFlags);
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
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspaceFlags);
		}

		style.WindowMinSize.x = minWinSizeX;

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Open Project...", "Ctrl+O")) ZE_CORE_WARN("TODO: OpenProject");

				ImGui::Separator();

				if (ImGui::MenuItem("New Level", "Ctrl+N")) ZE_CORE_WARN("TODO: NewLevel");

				if (ImGui::MenuItem("Save Level", "Ctrl+S")) ZE_CORE_WARN("TODO: SaveLevel");

				if (ImGui::MenuItem("Save Level As...", "Ctrl+Shift+S")) ZE_CORE_WARN("TODO: SaveLevelAs");

				ImGui::Separator();

				if (ImGui::MenuItem("Exit"))
					Game::Get().Close();
				
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("View"))
			{
				for (const auto &window : mEditorWindows)
				{
					if (window->CanBeClosed())
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
			ImGui::Begin(window->GetName().c_str());
			window->OnRenderWindow();
			ImGui::End();
			window->OnClearStyle();
		}
    }

    void Editor::BeginRenderGame()
    {
		for (auto &window : mEditorWindows)
			window->OnBeginRenderGame();
    }

    void Editor::EndRenderGame()
    {
		for (auto &window : mEditorWindows)
			window->OnEndRenderGame();
    }

    void Editor::RegisterEditorWindow(std::unique_ptr<EditorWindow> inWindow)
    {
		inWindow->OnRegister();
		ZE_CORE_INFO("Registered editor window {}", inWindow->GetName());
		mEditorWindows.push_back(std::move(inWindow));
    }

    void EditorWindow::Open()
    {
		if (!mIsOpen)
		{
			mIsOpen = true;
			OnOpen();
		}
    }

	void EditorWindow::Close()
	{
		if (mIsOpen && mCanBeClosed)
		{
			mIsOpen = false;
			OnClose();
		}
	}
}