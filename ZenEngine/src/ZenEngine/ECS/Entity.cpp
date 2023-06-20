#include "Entity.h"
#include "Level.h"

namespace ZenEngine
{
    Entity Entity::Null;

    Entity::Entity()
        : mHandle(entt::null), mLevel(nullptr)
    {
    }

    Entity::Entity(entt::entity inHandle, Level *inLevel)
        : mHandle(inHandle), mLevel(inLevel)
    {
    }

}