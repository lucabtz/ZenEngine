#include "SceneHierarchy.h"

#include "ZenEngine/ECS/CoreComponents.h"

#include <imgui.h>


namespace ZenEngine
{
    void SceneHierarchy::OnRenderWindow()
    {
        auto &activeScene = Editor::Get().GetActiveScene();
        auto &selectedEntity = Editor::Get().CurrentlySelectedEntity;
        if (activeScene != nullptr)
        {
            // Right-click on blank space
            if (ImGui::BeginPopupContextWindow())
            {
                if (ImGui::MenuItem("Create Entity"))
                    activeScene->CreateEntity();

                ImGui::EndPopup();
            }

            activeScene->Each([this](Entity entity)
            {
                if (!entity.HasComponent<HierarchyComponent>() || entity.GetComponent<HierarchyComponent>().Parent == Entity::Null)
                    DrawEntityNode(entity);
            });

            if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
                selectedEntity = Entity::Null;

        }
    }

    void SceneHierarchy::DrawEntityNode(Entity &entity)
    {
        std::string name;
        if (entity.HasComponent<NameComponent>())
            name = entity.GetComponent<NameComponent>().Name;
        else
            name = fmt::format("{}", (uint64_t)entity);

        auto &activeScene = Editor::Get().GetActiveScene();
        auto &selectedEntity = Editor::Get().CurrentlySelectedEntity;
        
        ImGuiTreeNodeFlags flags = ((selectedEntity == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
        flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
        bool opened = ImGui::TreeNodeEx((void*)(uint64_t)entity, flags, name.c_str());
        if (ImGui::IsItemClicked())
        {
            selectedEntity = entity;
        }

        bool entityDeleted = false;
        if (ImGui::BeginPopupContextItem())
        {
            if (ImGui::MenuItem("Create Child Entity"))
            {
                auto newEntity = activeScene->CreateEntity();
                entity.AddChild(newEntity);
            }
            if (ImGui::MenuItem("Delete Entity"))
                entityDeleted = true;

            ImGui::EndPopup();
        }
        
        if (opened)
        {
            if (entity.HasComponent<HierarchyComponent>())
            {
                entity.ForEachChild([this](auto child)
                {
                    DrawEntityNode(child);
                });
            }
            ImGui::TreePop();
        }

        if (entityDeleted)
        {
            if (selectedEntity == entity)
                selectedEntity = Entity::Null;
            entity.Destroy();
        }
    }
}