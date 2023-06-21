#include "CoreComponents.h"

#include "ZenEngine/Editor/EditorGUI.h"

namespace ZenEngine
{

    void NameComponentRenderer::RenderProperties(Entity inSelectedEntity, NameComponent &inNameComponent)
    {
        char buffer[256];
        memset(buffer, 0, sizeof(buffer));
        inNameComponent.Name.copy(buffer, sizeof(buffer) - 1);
        ImGui::InputText("Name", buffer, sizeof(buffer));
        inNameComponent.Name = buffer;
    }

    void TransformComponentRenderer::RenderProperties(Entity inSelectedEntity, TransformComponent &inTransformComponent)
    {
        EditorGUI::InputVec3("Translation", inTransformComponent.Translation);
        glm::vec3 rotation = glm::degrees(inTransformComponent.Rotation);
        EditorGUI::InputVec3("Rotation", rotation);
        inTransformComponent.Rotation = glm::radians(rotation);
        EditorGUI::InputVec3("Scale", inTransformComponent.Scale, 1.0f);
    }
}