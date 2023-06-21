#pragma once

#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "ZenEngine/Asset/StaticMesh.h"
#include "ZenEngine/Renderer/Shader.h"
#include "Entity.h"

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

    struct StaticMeshComponent
    {
        std::shared_ptr<StaticMesh> Mesh;
        std::shared_ptr<Shader> ShaderProgram;

        StaticMeshComponent() = default;
        StaticMeshComponent(const StaticMeshComponent&) = default;
        StaticMeshComponent(const std::shared_ptr<StaticMesh> &inStaticMesh, const std::shared_ptr<Shader> &inShader) 
            : Mesh(inStaticMesh), ShaderProgram(inShader) {}
    };

}