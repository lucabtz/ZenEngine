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

        for (int i = 0; i < 3; ++i)
        {
            // avoid display flickering between 180 and -180
            if (glm::epsilonEqual<float>(rotation[i], -180.0f, glm::epsilon<float>()))
                rotation[i] = 180.0f;
            
            // avoid display flickering between 0 and -0
            if (glm::epsilonEqual<float>(rotation[i], 0.0f, glm::epsilon<float>()))
                rotation[i] = 0.0f;
        }

        EditorGUI::InputVec3("Rotation", rotation);
        inTransformComponent.SetRotation(rotation);

        EditorGUI::InputVec3("Scale", inTransformComponent.Scale, 1.0f);
    }

    template <MaterialDataType Type>
    static void RenderShaderParam(
        const std::string &inName,
        const MaterialParameter &inParam,
        const std::shared_ptr<Material> &inMaterial,
        std::function<void(const std::string &, typename MaterialDataTypeCppType<Type>::Type &)> inUiFunc)
    {
        auto old = inMaterial->Get<Type>(inName);
        auto newValue = old;
        inUiFunc(inName, newValue);
        if (newValue != old)
            inMaterial->Set<Type>(inName, newValue);
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

            #define RENDER_PARAM(type, uifunc)\
                case type: RenderShaderParam<type>(name, parameter, inStaticMeshComponent.Mat, [](const std::string &displayName, typename MaterialDataTypeCppType<type>::Type &value){uifunc});\
                break;

                switch (parameter.Info.Type)
                {
                RENDER_PARAM(MaterialDataType::Float,   ImGui::InputFloat(displayName.c_str(), &value);)
                RENDER_PARAM(MaterialDataType::Float2,  ImGui::InputFloat2(displayName.c_str(), &value[0]);)
                RENDER_PARAM(MaterialDataType::Float3,  ImGui::ColorEdit3(displayName.c_str(), &value[0]);)
                RENDER_PARAM(MaterialDataType::Float4,  ImGui::ColorEdit4(displayName.c_str(), &value[0]);)
                RENDER_PARAM(MaterialDataType::Int,     ImGui::InputInt(displayName.c_str(), &value);)
                RENDER_PARAM(MaterialDataType::Int2,    ImGui::InputInt2(displayName.c_str(), &value[0]);)
                RENDER_PARAM(MaterialDataType::Int3,    ImGui::InputInt3(displayName.c_str(), &value[0]);)
                RENDER_PARAM(MaterialDataType::Int4,    ImGui::InputInt4(displayName.c_str(), &value[0]);) 
                RENDER_PARAM(MaterialDataType::Bool,    ImGui::Checkbox(displayName.c_str(), &value);)
                }
            #undef RENDER_PARAM
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

    void AmbientLightComponentRenderer::RenderProperties(Entity inSelectedEntity, AmbientLightComponent &inAmbientLightComponent)
    {
        ImGui::ColorEdit3("Light Color", &inAmbientLightComponent.Info.AmbientLightColor[0]);
        ImGui::InputFloat("Intensity", &inAmbientLightComponent.Info.AmbientLightIntensity);
    }

    void DirectionalLightComponentRenderer::RenderProperties(Entity inSelectedEntity, DirectionalLightComponent &inDirectionalLightComponent)
    {
        ImGui::ColorEdit3("Light Color", &inDirectionalLightComponent.Color[0]);
        ImGui::InputFloat("Intensity", &inDirectionalLightComponent.Intensity);
    }
}