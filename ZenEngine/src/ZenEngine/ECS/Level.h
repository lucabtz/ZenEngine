#pragma once

#include <entt/entt.hpp>

namespace ZenEngine
{
    class Entity;

    class Level
    {
    public:
        Entity CreateEntity();
        Entity CreateEntity(const std::string &inTag);

        std::vector<Entity> GetEntitiesByTag(const std::string &inTag);
        Entity GetEntityByTag(const std::string &inTag);
    private:
        entt::registry mRegistry;

        friend class Entity;
    };
}