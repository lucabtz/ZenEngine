#pragma once

#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "ZenEngine/Asset/StaticMesh.h"
#include "ZenEngine/Renderer/Shader.h"
#include "Entity.h"
#include "ZenEngine/Editor/PropertiesWindow.h"
#include "ZenEngine/Asset/ShaderAsset.h"
#include "ZenEngine/Renderer/Material.h"

namespace ZenEngine
{

    struct NameComponent
    {
        std::string Name;

        NameComponent() = default;
        NameComponent(const NameComponent&) = default;
        NameComponent(const std::string& inName)
            : Name(inName) 
        {}
    };

    class NameComponentRenderer : public PropertyRendererFor<NameComponent>
    {
    public:
        NameComponentRenderer() : PropertyRendererFor("Name Component") {};
        virtual void RenderProperties(Entity inSelectedEntity, NameComponent &inNameComponent) override;
    };


    struct HierarchyComponent
    {
        Entity Parent = Entity::Null;
        uint32_t ChildrenCount = 0;
        Entity First = Entity::Null;
        Entity Previous = Entity::Null;
        Entity Next = Entity::Null;

        HierarchyComponent() = default;
        HierarchyComponent(const HierarchyComponent&) = default;
        HierarchyComponent(Entity inParent) : Parent(inParent) {}
    };

    struct TransformComponent
    {
        glm::vec3 Translation = { 0.0f, 0.0f, 0.0f };
        glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f };
        glm::vec3 Scale = { 1.0f, 1.0f, 1.0f };

        TransformComponent() = default;
        TransformComponent(const TransformComponent&) = default;
        TransformComponent(const glm::vec3& translation)
            : Translation(translation) {}

        glm::mat4 GetTransform() const
        {
            glm::mat4 rotation = glm::toMat4(glm::quat(Rotation));

            return glm::translate(glm::mat4(1.0f), Translation)
                * rotation
                * glm::scale(glm::mat4(1.0f), Scale);
        }
    };

    class TransformComponentRenderer : public PropertyRendererFor<TransformComponent>
    {
    public:
        TransformComponentRenderer() : PropertyRendererFor("Transform Component") {};
        virtual void RenderProperties(Entity inSelectedEntity, TransformComponent &inTransformComponent) override;
    };

    struct StaticMeshComponent
    {
        std::shared_ptr<StaticMesh> Mesh;
        std::shared_ptr<Material> Mat;

        // TODO add a ifdef WITH_EDITOR here this is not needed for runtime
        std::shared_ptr<ShaderAsset> Shader;

        StaticMeshComponent() = default;
        StaticMeshComponent(const StaticMeshComponent&) = default;
        StaticMeshComponent(const std::shared_ptr<StaticMesh> &inStaticMesh, const std::shared_ptr<Material> &inMaterial) 
            : Mesh(inStaticMesh), Mat(inMaterial) {}
    };

    class StaticMeshComponentRenderer : public PropertyRendererFor<StaticMeshComponent>
    {
    public:
        StaticMeshComponentRenderer() : PropertyRendererFor("Static Mesh Component") {}
        virtual void RenderProperties(Entity inSelectedEntity, StaticMeshComponent &inStaticMeshComponent) override;
    };

}