#include "PropertiesWindow.h"

#include "Editor.h"
#include "ZenEngine/ECS/CoreComponents.h"

namespace ZenEngine
{
    void PropertiesWindow::OnRegister()
    {
        RegisterPropertyRenderer(std::make_unique<NameComponentRenderer>());
        RegisterPropertyRenderer(std::make_unique<TransformComponentRenderer>());
    }

    void PropertiesWindow::OnRenderWindow()
    {
        auto &selectedEntity = Editor::Get().CurrentlySelectedEntity;
        if (selectedEntity == Entity::Null) return;
        ImGui::PushItemWidth(-1);

        if (ImGui::Button("Add Component"))
            ImGui::OpenPopup("Add Component Popup");

        if (ImGui::BeginPopup("Add Component Popup"))
        {
            for (auto &renderer : mPropertyRenderers) renderer->OnRenderAddMenuItem(selectedEntity);

            ImGui::EndPopup();
        }

        ImGui::PopItemWidth();

        for (auto &renderer : mPropertyRenderers) renderer->OnRenderProperties(selectedEntity);
    }
    
    void PropertiesWindow::RegisterPropertyRenderer(std::unique_ptr<PropertyRenderer> inRenderer)
    {
        mPropertyRenderers.push_back(std::move(inRenderer));
    }
}