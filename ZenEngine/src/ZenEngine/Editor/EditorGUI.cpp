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

    void EditorGUI::InputVec3(const std::string &inLabel, glm::vec3 &outValues, float inResetValue, float inColumnWidth)
    {
        ImGuiIO& io = ImGui::GetIO();

        ImGui::PushID(inLabel.c_str());

        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, inColumnWidth);
        ImGui::Text(inLabel.c_str());
        ImGui::NextColumn();

        ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

        float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
        ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
        if (ImGui::Button("X", buttonSize))
            outValues.x = inResetValue;
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ImGui::DragFloat("##X", &outValues.x, 0.1f, 0.0f, 0.0f, "%.2f");
        ImGui::PopItemWidth();
        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
        if (ImGui::Button("Y", buttonSize))
            outValues.y = inResetValue;
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ImGui::DragFloat("##Y", &outValues.y, 0.1f, 0.0f, 0.0f, "%.2f");
        ImGui::PopItemWidth();
        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
        if (ImGui::Button("Z", buttonSize))
            outValues.z = inResetValue;
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ImGui::DragFloat("##Z", &outValues.z, 0.1f, 0.0f, 0.0f, "%.2f");
        ImGui::PopItemWidth();

        ImGui::PopStyleVar();

        ImGui::Columns(1);

        ImGui::PopID();
    }

    bool EditorGUI::InputAsset(const std::string &inLabel, const AssetClass *inAssetClass, std::shared_ptr<AssetInstance> &outAsset, float inColumnWidth)
    {
        bool ret = false;

        ImGui::PushID(inLabel.c_str());

        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, inColumnWidth);
        ImGui::Text(inLabel.c_str());
        ImGui::NextColumn();

        ImGui::Text("%s", (outAsset != nullptr)? fmt::format("{}", (uint64_t)outAsset->GetAssetId()).c_str() : "Drag an asset here");
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(inAssetClass->Name.c_str()))
            {
                UUID assetId = *reinterpret_cast<uint64_t*>(payload->Data);
                outAsset = AssetManager::Get().LoadAsset(assetId);
                ret = true;
            }
            ImGui::EndDragDropTarget();
        }

        ImGui::Columns(1);
        
        ImGui::PopID();
        return ret;
    }


    void EditorGUI::SelectableText(const std::string &inLabel, const std::string &inText, float inColumnWidth)
    {
        ImGui::PushID(inLabel.c_str());

        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, inColumnWidth);
        ImGui::Text(inLabel.c_str());
        ImGui::NextColumn();

        ImGui::InputText("##text", const_cast<char*>(inText.c_str()), inText.size(), ImGuiInputTextFlags_ReadOnly);
        
        ImGui::Columns(1);

        ImGui::PopID();
    }
}