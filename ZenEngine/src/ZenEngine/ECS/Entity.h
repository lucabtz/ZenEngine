#pragma once

#include <entt/entt.hpp>

#include "Level.h"
#include "ZenEngine/Core/Macros.h"

namespace ZenEngine
{
    class Entity
    {
    public:
        static Entity Null;

        Entity();
        Entity(entt::entity inHandle, Level *inLevel);

        template<typename T, typename... Args>
		T& AddComponent(Args&&... args)
		{
			ZE_ASSERT_CORE_MSG(!HasComponent<T>(), "Entity already has component!");
			T& component = mLevel->mRegistry.emplace<T>(mHandle, std::forward<Args>(args)...);
			return component;
		}

        template<typename T, typename... Args>
		T& AddOrReplaceComponent(Args&&... args)
		{
			T& component = mLevel->mRegistry.emplace_or_replace<T>(mHandle, std::forward<Args>(args)...);
			return component;
		}

		template<typename T>
		T& GetComponent()
		{
			ZE_ASSERT_CORE_MSG(HasComponent<T>(), "Entity does not have component!");
			return mLevel->mRegistry.get<T>(mHandle);
		}

		template<typename T>
		bool HasComponent()
		{
			//return mLevel->mRegistry.has<T>(mHandle);
			return true;
		}

		template<typename T>
		void RemoveComponent()
		{
			ZE_ASSERT_CORE_MSG(HasComponent<T>(), "Entity does not have component!");
			mLevel->mRegistry.remove<T>(mHandle);
		}
        operator bool() const { return mHandle != entt::null && mLevel != nullptr; }
        bool operator ==(const Entity &inOther) { return mHandle == inOther.mHandle && mLevel == inOther.mLevel; }
        operator entt::entity() const { return mHandle; }
    private:
        entt::entity mHandle;
        Level *mLevel;
    };
}