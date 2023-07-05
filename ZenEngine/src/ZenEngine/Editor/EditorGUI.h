#pragma once

#include <mutex>

#include <imgui.h>
#include <imgui_internal.h>
#include <glm/glm.hpp>
#include <ImGuizmo.h>

#include "ZenEngine/Core/Macros.h"
#include "ZenEngine/Event/Event.h"
#include "ZenEngine/Asset/AssetManager.h"

namespace ZenEngine
{
    class EditorGUI
    {
    public:
        EditorGUI();
        void Init();
        void Shutdown();

        void OnEvent(const std::unique_ptr<Event> &inEvent);

        void BeginGUI();
        void EndGUI();

        static EditorGUI &Get() { ZE_ASSERT_CORE_MSG(sEditorGUIInstance != nullptr, "EditorGUI does not exist!"); return  *sEditorGUIInstance; }

        static void InputVec3(const std::string &inLabel, glm::vec3 &outValues, float inResetValue = 0.0f, float inColumnWidth = 100.0f);
        static bool InputAsset(const std::string &inLabel, const char *inAssetClassName, std::shared_ptr<Asset> &outAsset, float inColumnWidth = 100.0f);
        static bool InputAssetUUID(const std::string &inLabel, const char *inAssetClassName, UUID &outAssetId, float inColumnWidth = 100.0f);

        template <IsAsset T>
        static bool InputAsset(const std::string &inLabel, std::shared_ptr<T> &outAsset, float inColumnWidth = 100.0f)
        {
            std::shared_ptr<Asset> asset = outAsset;
            bool ret = InputAsset(inLabel, T::GetStaticAssetClassName(), asset, inColumnWidth);
            outAsset = std::static_pointer_cast<T>(asset);
            return ret;
        }

        template <IsAsset T>
        static bool InputAssetUUID(const std::string &inLabel, UUID &outAssetId, float inColumnWidth = 100.0f)
        {
            return InputAssetUUID(inLabel, T::GetStaticAssetClassName(), outAssetId, inColumnWidth);
        }

        template <size_t BufferSize = 256>
        static void InputText(const std::string &inLabel, std::string &outText, float inColumnWidth = 100.0f)
        {
            char buffer[BufferSize];
            memset(buffer, 0, sizeof(buffer));
            outText.copy(buffer, sizeof(buffer) - 1);
            ImGui::PushID(inLabel.c_str());

            ImGui::Columns(2);
            ImGui::SetColumnWidth(0, inColumnWidth);
            ImGui::Text(inLabel.c_str());
            ImGui::NextColumn();

            ImGui::InputText("##text", buffer, sizeof(buffer) - 1);
            outText = buffer;

            ImGui::Columns(1);
            
            ImGui::PopID();
        }

        template <size_t BufferSize = 256>
        static void InputTextNoLayout(const std::string &inLabel, std::string &outText)
        {
            char buffer[BufferSize];
            memset(buffer, 0, sizeof(buffer));
            outText.copy(buffer, sizeof(buffer) - 1);
            ImGui::PushID(inLabel.c_str());
            ImGui::InputText("##text", buffer, sizeof(buffer) - 1);
            outText = buffer;
            ImGui::PopID();
        }

        static void SelectableText(const std::string &inLabel, const std::string &inText, float inColumnWidth = 100.0f);

    private:
        static EditorGUI *sEditorGUIInstance;
    };
};