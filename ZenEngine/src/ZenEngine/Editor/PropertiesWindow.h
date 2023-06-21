#pragma once

#include "Editor.h"
#include <imgui.h>
#include <imgui_internal.h>

namespace ZenEngine
{

    class PropertyRenderer
    {
    public:
        virtual ~PropertyRenderer() = default;
        virtual void OnRenderProperties(Entity inSelectedEntity) = 0;
        virtual void OnRenderAddMenuItem(Entity inSelectedEntity) = 0;
    };

    template <typename T>
    class PropertyRendererFor : public PropertyRenderer
    {
    public:
        PropertyRendererFor(const std::string &inName) : mComponentName(inName) {}

        virtual void RenderProperties(Entity inSelectedEntity, T &inComponent) = 0;
        virtual void OnRenderProperties(Entity inSelectedEntity) override
        {
            if (inSelectedEntity == Entity::Null) return;
            if (inSelectedEntity.HasComponent<T>())
            {
                const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;
                auto &comp = inSelectedEntity.GetComponent<T>();

                ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
                float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
                ImGui::Separator();
                bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, mComponentName.c_str());
                ImGui::PopStyleVar();
                ImGui::SameLine(contentRegionAvailable.x - lineHeight * 0.5f);
                if (ImGui::Button("+", ImVec2{ lineHeight, lineHeight }))
                {
                    ImGui::OpenPopup("Component Settings");
                }

                bool removeComponent = false;
                if (ImGui::BeginPopup("Component Settings"))
                {
                    if (ImGui::MenuItem("Remove component"))
                        removeComponent = true;

                    ImGui::EndPopup();
                }

                if (open)
                {
                    RenderProperties(inSelectedEntity, comp);
                    ImGui::TreePop();
                }

                if (removeComponent)
                    inSelectedEntity.RemoveComponent<T>();
            }
        }

        virtual void OnRenderAddMenuItem(Entity inSelectedEntity)
        {
            if (!inSelectedEntity.HasComponent<T>())
            {
                if (ImGui::MenuItem(mComponentName.c_str()))
                {
                    inSelectedEntity.AddComponent<T>();
                    ImGui::CloseCurrentPopup();
                }
            }
        }
    private:
        std::string mComponentName;
    };

    class PropertiesWindow : public EditorWindow
    {
    public:
        PropertiesWindow() : EditorWindow("Properties", true) {}
        virtual void OnRegister() override;
        virtual void OnRenderWindow() override;

        void RegisterPropertyRenderer(std::unique_ptr<PropertyRenderer> inRenderer);

    private:
        std::vector<std::unique_ptr<PropertyRenderer>> mPropertyRenderers;
    };

}