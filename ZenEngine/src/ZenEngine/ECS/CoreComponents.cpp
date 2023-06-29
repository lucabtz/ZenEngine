#include "CoreComponents.h"

#include "ZenEngine/Core/Math.h"
#include "ZenEngine/Editor/EditorGUI.h"
#include "ZenEngine/Asset/Texture2DAsset.h"

namespace ZenEngine
{

    void NameComponentRenderer::RenderProperties(Entity inSelectedEntity, NameComponent &inNameComponent)
    {
        EditorGUI::InputText("Name", inNameComponent.Name);
    }

    void TransformComponentRenderer::RenderProperties(Entity inSelectedEntity, TransformComponent &inTransformComponent)
    {
        EditorGUI::InputVec3("Position", inTransformComponent.Position);

        glm::vec3 rotation = inTransformComponent.GetEulerAngles();
        EditorGUI::InputVec3("Rotation", rotation);
        inTransformComponent.SetRotation(rotation);

        EditorGUI::InputVec3("Scale", inTransformComponent.Scale, 1.0f);
    }

    void StaticMeshComponentRenderer::RenderProperties(Entity inSelectedEntity, StaticMeshComponent &inStaticMeshComponent)
    {
        if (EditorGUI::InputAssetUUID<StaticMesh>("Mesh", inStaticMeshComponent.MeshId))
        {
            inStaticMeshComponent.MeshVertexArray = AssetManager::Get().LoadAssetAs<StaticMesh>(inStaticMeshComponent.MeshId)->CreateOrGetVertexArray();
        }
        if (EditorGUI::InputAssetUUID<ShaderAsset>("Shader", inStaticMeshComponent.ShaderId))
        {
            auto shader = AssetManager::Get().LoadAssetAs<ShaderAsset>(inStaticMeshComponent.ShaderId);
            inStaticMeshComponent.Mat = Material::Create(shader->CreateOrGetShaderProgram());
        }
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

            auto textures = inStaticMeshComponent.Mat->GetTextures();
            for (auto &[name, texture] : textures)
            {
                if (!inStaticMeshComponent.TextureUUID.contains(name)) inStaticMeshComponent.TextureUUID[name] = 0;
                
                if (EditorGUI::InputAssetUUID<Texture2DAsset>(name, inStaticMeshComponent.TextureUUID[name]))
                {
                    auto tex = AssetManager::Get().LoadAssetAs<Texture2DAsset>(inStaticMeshComponent.TextureUUID[name]);
                    inStaticMeshComponent.Mat->SetTexture(name, tex->CreateOrGetTexture2D());
                }
                
            }
        }
    }
}