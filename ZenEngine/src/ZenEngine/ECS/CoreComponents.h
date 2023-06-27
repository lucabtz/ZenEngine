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
        glm::vec3 Position;
        glm::quat Rotation;
        glm::vec3 Scale;

        TransformComponent() : Position(0.0f, 0.0f, 0.0f), Rotation(), Scale(1.0f, 1.0f, 1.0f) {}
        TransformComponent(const TransformComponent&) = default;

        glm::mat4 GetTransform()
        {
            glm::mat4 rotation = glm::toMat4(Rotation);
            return glm::translate(glm::mat4(1.0f), Position) * rotation * glm::scale(glm::mat4(1.0f), Scale);
        }

        void SetRotation(const glm::vec3 &inEulerAngles)
        {
            Rotation = glm::quat(glm::radians(inEulerAngles));
        }

        glm::vec3 GetEulerAngles()
        {
            return glm::degrees(glm::eulerAngles(Rotation));
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
        UUID ShaderId = 0;
        UUID MeshId = 0;
    
        std::shared_ptr<VertexArray> MeshVertexArray;
        std::shared_ptr<Material> Mat;

        StaticMeshComponent() = default;
        StaticMeshComponent(const StaticMeshComponent&) = default;
    };

    class StaticMeshComponentRenderer : public PropertyRendererFor<StaticMeshComponent>
    {
    public:
        StaticMeshComponentRenderer() : PropertyRendererFor("Static Mesh Component") {}
        virtual void RenderProperties(Entity inSelectedEntity, StaticMeshComponent &inStaticMeshComponent) override;

        
    };

}