#pragma once

#include <entt/entt.hpp>
#include <glm/glm.hpp>

#include "Scene.h"
#include "ZenEngine/Core/Macros.h"

namespace ZenEngine
{
    class Entity
    {
    public:
        static Entity Null;

        Entity();
        Entity(entt::entity inHandle, Scene *inScene);

        template <typename T, typename... Args>
        T &AddComponent(Args &&...args)
        {
            ZE_ASSERT_CORE_MSG(!HasComponent<T>(), "Entity already has component!");
            T &component = mScene->mRegistry.emplace<T>(mHandle, std::forward<Args>(args)...);
            return component;
        }

        template <typename T, typename... Args>
        T &AddOrReplaceComponent(Args &&...args)
        {
            T &component = mScene->mRegistry.emplace_or_replace<T>(mHandle, std::forward<Args>(args)...);
            return component;
        }

        template <typename T>
        T &GetComponent()
        {
            ZE_ASSERT_CORE_MSG(HasComponent<T>(), "Entity does not have component!");
            return mScene->mRegistry.get<T>(mHandle);
        }

        template <typename T>
        bool HasComponent() const
        {
            return mScene->mRegistry.all_of<T>(mHandle);
        }

        template <typename... T>
        bool HasComponents() const
        {
            return mScene->mRegistry.all_of<T...>(mHandle);
        }

        template <typename T>
        void RemoveComponent()
        {
            ZE_ASSERT_CORE_MSG(HasComponent<T>(), "Entity does not have component!");
            mScene->mRegistry.remove<T>(mHandle);
        }

        void Destroy();

        void ForEachChildInterruptable(const std::function<bool(Entity)> &inLambda);
        void ForEachChild(const std::function<void(Entity)> &inLambda) 
        { 
            std::function<bool(Entity)> wrapper = [&inLambda](Entity e){ inLambda(e); return false; }; 
            ForEachChildInterruptable(wrapper);
        }
        void ForEachAncestorInterruptable(const std::function<bool(Entity)> &inLambda);
        void ForEachAncestor(const std::function<void(Entity)> &inLambda)
        {
            std::function<bool(Entity)> wrapper = [&inLambda](Entity e){ inLambda(e); return false; }; 
            ForEachAncestorInterruptable(wrapper);
        }

        glm::mat4 GetLocalTransform();
        glm::mat4 GetWorldTransform();
        glm::mat4 GetParentTransform();

        void AddChild(Entity &inEntity);
        void RemoveChild(Entity &inEntity);
        bool IsAncestorOf(Entity &inEntity);
        void Unparent();

        operator bool() const { return mHandle != entt::null && mScene != nullptr; }
        bool operator==(const Entity &inOther) { return mHandle == inOther.mHandle && mScene == inOther.mScene; }
        bool operator!=(const Entity &inOther) { return !(*this == inOther); }
        operator entt::entity() const { return mHandle; }
        operator uint64_t() const { return (uint64_t)mHandle; }

    private:
        entt::entity mHandle;
        Scene *mScene;
    };
}