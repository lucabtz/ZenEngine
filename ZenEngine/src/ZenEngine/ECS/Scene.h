#pragma once

#include <entt/entt.hpp>
#include "System.h"
#include "ZenEngine/Renderer/Renderer.h"

namespace ZenEngine
{
    class Entity;

    class Scene
    {
    public:
        Scene();

        Entity CreateEntity();
        Entity CreateEntity(const std::string &inName);

        std::vector<Entity> GetEntitiesByName(const std::string &inTag);
        Entity GetEntityByName(const std::string &inTag);

        template <typename T>
        void RegisterSystem()
        {
            mSystems.push_back(std::make_unique<T>(this));
        }
    
        void OnRender(float inDeltaTime);
        void OnUpdate(float inDeltaTime);

        Renderer::LightInfo GetLights();

        template <typename ... T>
        auto View()
        {
            return mRegistry.view<T...>();
        }

        void Each(const std::function<void(Entity)> &inFunc)
        {
            auto wrapper = [this, &inFunc](auto enttEnt){ Entity e(enttEnt, this); inFunc(e); };
            mRegistry.each(wrapper);
        }

    private:
        entt::registry mRegistry;
        std::vector<std::unique_ptr<System>> mSystems;

        friend class Entity;
    };
}