#include "CoreComponents.h"

#include "ZenEngine/Editor/EditorGUI.h"

namespace ZenEngine
{

    void NameComponentRenderer::RenderProperties(Entity inSelectedEntity, NameComponent &inNameComponent)
    {
        EditorGUI::InputText("Name", inNameComponent.Name);
    }

    void TransformComponentRenderer::RenderProperties(Entity inSelectedEntity, TransformComponent &inTransformComponent)
    {
        EditorGUI::InputVec3("Translation", inTransformComponent.Translation);
        glm::vec3 rotation = glm::degrees(inTransformComponent.Rotation);
        EditorGUI::InputVec3("Rotation", rotation);
        inTransformComponent.Rotation = glm::radians(rotation);
        EditorGUI::InputVec3("Scale", inTransformComponent.Scale, 1.0f);
    }

    void StaticMeshComponentRenderer::RenderProperties(Entity inSelectedEntity, StaticMeshComponent &inStaticMeshComponent)
    {
        EditorGUI::InputAsset<StaticMesh>("Mesh", inStaticMeshComponent.Mesh);   
        if (EditorGUI::InputAsset<ShaderAsset>("Shader", inStaticMeshComponent.Shader) && inStaticMeshComponent.Shader != nullptr)
            inStaticMeshComponent.Mat = Material::Create(inStaticMeshComponent.Shader->CreateOrGetShaderProgram());
        if (inStaticMeshComponent.Mat != nullptr)
        {
            auto params = inStaticMeshComponent.Mat->GetParameters();
            for (auto &[name, parameter] : params)
            {
                if (parameter.Info.Type == MaterialDataType::Float4)
                {
                    auto oldColor = inStaticMeshComponent.Mat->Get<MaterialDataType::Float4>(name);
                    glm::vec4 color = oldColor;
                    ImGui::ColorEdit4(name.c_str(), &color.x);
                    if (color != oldColor)
                        inStaticMeshComponent.Mat->Set<MaterialDataType::Float4>(name, color);
                }
            }
        }
    }
}