#include "EditorGUI.h"

#include "ZenEngine/Core/Game.h"
#include "ZenEngine/Core/Platform.h"
#include "ZenEngine/Renderer/Renderer.h"

#ifdef ZE_WINDOW_PLATFORM_GLFW
    #include <backends/imgui_impl_glfw.h>
    #include <GLFW/glfw3.h>
#else
    #error "Editor only supports GLFW as window platform at the moment!"
#endif
#ifdef ZE_RENDERER_PLATFORM_OPENGL
    #include <backends/imgui_impl_opengl3.h>
#else
    #error "Editor only supports OpenGL as renderer at the moment!"
#endif

namespace ZenEngine
{
	static void SetDarkThemeColors()
	{
		auto& colors = ImGui::GetStyle().Colors;
		colors[ImGuiCol_WindowBg] = ImVec4{ 0.1f, 0.105f, 0.11f, 1.0f };

		// Headers
		colors[ImGuiCol_Header] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
		colors[ImGuiCol_HeaderHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
		colors[ImGuiCol_HeaderActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		
		// Buttons
		colors[ImGuiCol_Button] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
		colors[ImGuiCol_ButtonHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
		colors[ImGuiCol_ButtonActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

		// Frame BG
		colors[ImGuiCol_FrameBg] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
		colors[ImGuiCol_FrameBgHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
		colors[ImGuiCol_FrameBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

		// Tabs
		colors[ImGuiCol_Tab] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TabHovered] = ImVec4{ 0.38f, 0.3805f, 0.381f, 1.0f };
		colors[ImGuiCol_TabActive] = ImVec4{ 0.28f, 0.2805f, 0.281f, 1.0f };
		colors[ImGuiCol_TabUnfocused] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };

		// Title
		colors[ImGuiCol_TitleBg] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TitleBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
	}

    EditorGUI *EditorGUI::sEditorGUIInstance;


    EditorGUI::EditorGUI()
    {
        ZE_ASSERT_CORE_MSG(sEditorGUIInstance == nullptr, "An EditorGUI already exists!");
        sEditorGUIInstance = this;
    }

    void EditorGUI::Init()
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
        //io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
        //io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;
        io.ConfigWindowsMoveFromTitleBarOnly = true;

        float fontSize = 16.0f;
        io.Fonts->AddFontFromFileTTF("resources/Fonts/WorkSans/WorkSans-Regular.ttf", fontSize);

        ImGui::StyleColorsDark();
        SetDarkThemeColors();
            
        void *nativeWindow = Game::Get().GetWindow()->GetNativeWindow();
    #ifdef ZE_RENDERER_PLATFORM_OPENGL
        #ifdef ZE_WINDOW_PLATFORM_GLFW
        ImGui_ImplGlfw_InitForOpenGL((GLFWwindow *)nativeWindow, true);
        #endif
        ImGui_ImplOpenGL3_Init("#version 410");
    #endif
    }

    void EditorGUI::Shutdown()
    {

    #ifdef ZE_RENDERER_PLATFORM_OPENGL
        ImGui_ImplOpenGL3_Shutdown();
    #endif
    #ifdef ZE_WINDOW_PLATFORM_GLFW
        ImGui_ImplGlfw_Shutdown();
    #endif

        ImGui::DestroyContext();
    }

    void EditorGUI::OnEvent(const std::unique_ptr<Event> &inEvent)
    {
        ImGuiIO& io = ImGui::GetIO();
        inEvent->Handled |= inEvent->IsInCategory(EventCategoryMouse) & io.WantCaptureMouse;
        inEvent->Handled |= inEvent->IsInCategory(EventCategoryKeyboard) & io.WantCaptureKeyboard;
    }

    void EditorGUI::BeginGUI()
    {
    #ifdef ZE_RENDERER_PLATFORM_OPENGL
        ImGui_ImplOpenGL3_NewFrame();
        #ifdef ZE_WINDOW_PLATFORM_GLFW
        ImGui_ImplGlfw_NewFrame();
        #endif
    #endif
		ImGui::NewFrame();
    }

    void EditorGUI::EndGUI()
    {
        ImGuiIO& io = ImGui::GetIO();
        auto& window = Game::Get().GetWindow();
        io.DisplaySize = ImVec2((float)window->GetWidth(), (float)window->GetHeight());

        ImGui::Render();

        #ifdef ZE_RENDERER_PLATFORM_OPENGL
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
         #else
            #error "Renderer platform not supported by ImGui!"
        #endif


        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
        #ifdef ZE_WINDOW_PLATFORM_GLFW
            GLFWwindow *window = glfwGetCurrentContext();
        #endif

            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();

        #ifdef ZE_WINDOW_PLATFORM_GLFW
            if (window != nullptr)
                glfwMakeContextCurrent(window);
        #endif
        }
    }
}