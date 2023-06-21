#include "Entity.h"
#include "Scene.h"
#include "CoreComponents.h"

namespace ZenEngine
{
    Entity Entity::Null;

    Entity::Entity()
        : mHandle(entt::null), mScene(nullptr)
    {
    }

    Entity::Entity(entt::entity inHandle, Scene *inScene)
        : mHandle(inHandle), mScene(inScene)
    {
    }

    void Entity::Destroy()
    {
        ZE_ASSERT_CORE_MSG(mScene != nullptr, "Scene is null!");
        mScene->mRegistry.destroy(mHandle);
        mHandle = entt::null;
        mScene = nullptr;
    }

    void Entity::ForEachChildInterruptable(const std::function<bool(Entity)> &inLambda)
    {
        if (!HasComponent<HierarchyComponent>())
        {
            ZE_CORE_WARN("The entity has no children!");
            return;
        }

        auto &h = GetComponent<HierarchyComponent>();
        auto curr = h.First;

        while (curr != Entity::Null) 
        {
            if (inLambda(curr)) break;
            curr = curr.GetComponent<HierarchyComponent>().Next;
        }
    }

    void Entity::ForEachAncestorInterruptable(const std::function<bool(Entity)> &inLambda)
    {
        if (!HasComponent<HierarchyComponent>())
        {
            ZE_CORE_WARN("The entity has no father!");
            return;
        }
        auto &h = GetComponent<HierarchyComponent>();
        auto curr = h.Parent;

        while (curr != Entity::Null)
        {
            if (inLambda(curr)) break;
            curr = curr.GetComponent<HierarchyComponent>().Parent;
        }
    }

    void Entity::AddChild(Entity &inEntity)
    {

        // make sure it is the same scene
        if (mScene != inEntity.mScene)
        {
            ZE_CORE_ERROR("It is not possible to parent entities belonging to different scenes");
            return;
        }
        // TODO: technically we might want to check we are not creating a loop of entities
        // Something like
        if (inEntity.IsAncestorOf(*this))
        {
            ZE_CORE_ERROR("It is not possible for an entity to be both a child and an ancestor!");
            return;
        }
     
        // make sure we have a hierarchy component
        if (!HasComponent<HierarchyComponent>()) AddComponent<HierarchyComponent>();
        if (inEntity.HasComponent<HierarchyComponent>())
        {
            auto &hierarchy = inEntity.GetComponent<HierarchyComponent>();
            if (hierarchy.Parent != Entity::Null)
            {
                // we are reparenting, for now write something to console
                ZE_CORE_WARN("Entity already has a parent. Reparenting");
                hierarchy.Parent.RemoveChild(inEntity);
                hierarchy.Parent = *this;
            }
        }
        else
        {
            // otherwise just add a new hierarchy component with this as a father
            inEntity.AddComponent<HierarchyComponent>(*this);
        }

        // now setup the fathers hierarchy component
        auto &childHierarchy = inEntity.GetComponent<HierarchyComponent>();
        auto &fatherHierarchy = GetComponent<HierarchyComponent>();
        Entity oldFirstChild = fatherHierarchy.First;
        if (oldFirstChild != Entity::Null)
        {
            auto &oldFirstChildHierarchy = oldFirstChild.GetComponent<HierarchyComponent>();
            // update the previous value for the old first child
            oldFirstChildHierarchy.Previous = inEntity;
        }
        // add the child as a first child
        fatherHierarchy.First = inEntity;
        fatherHierarchy.ChildrenCount++;

        // update the next value for the new child
        childHierarchy.Next = oldFirstChild;
    }

    void Entity::RemoveChild(Entity &inEntity)
    {
        if (mScene != inEntity.mScene)
        {
            ZE_CORE_ERROR("It is not possible to unparent entities belonging to different scenes");
            return;
        }
        ForEachChildInterruptable([this, &inEntity](Entity child)
        {
            if (child == inEntity)
            {
                Entity prev = child.GetComponent<HierarchyComponent>().Previous;
                Entity next = child.GetComponent<HierarchyComponent>().Next;
                if (prev != Entity::Null)
                {
                    auto &prevH = prev.GetComponent<HierarchyComponent>();
                    prevH.Next = next;
                }
                if (next != Entity::Null)
                {
                    auto &nextH = next.GetComponent<HierarchyComponent>();
                    nextH.Previous = prev;
                }
                // here we can be sure inEntity has a HierarchyComponent since it is a child
                inEntity.GetComponent<HierarchyComponent>().Parent = Entity::Null;
            
                GetComponent<HierarchyComponent>().ChildrenCount--;
                return true;
            }
            return false;
        });    
    }

    bool Entity::IsAncestorOf(Entity &inEntity)
    {
        if (!inEntity.HasComponent<HierarchyComponent>()) return false;
        auto &hc = inEntity.GetComponent<HierarchyComponent>();
        bool isAncestor = false;
        ForEachChildInterruptable([&inEntity, &isAncestor](Entity child)
        {
            if (child == inEntity)
            {
                isAncestor = true;
                return true;
            }
            return false;
        });
        return isAncestor;
    }

}