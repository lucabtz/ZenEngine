#include "Level.h"
#include "Entity.h"

#include "CoreComponents.h"

namespace ZenEngine
{

    Entity Level::CreateEntity()
    {
        return CreateEntity("Entity");
    }

    Entity Level::CreateEntity(const std::string &inTag)
    {
        Entity e(mRegistry.create(), this);
        e.AddComponent<TagComponent>(inTag);
        return e;
    }

    std::vector<Entity> Level::GetEntitiesByTag(const std::string &inTag)
    {
        std::vector<Entity> result;
        auto view = mRegistry.view<TagComponent>();
		for (auto entity : view)
		{
			const TagComponent& tc = view.get<TagComponent>(entity);
			if (tc.Tag == inTag)
				result.push_back(Entity(entity, this));
		}
		return result;
    }

    Entity Level::GetEntityByTag(const std::string &inTag)
    {
        auto entities = GetEntitiesByTag(inTag);
        if (entities.size() > 0) return entities[0];
        return Entity::Null;
    }
}