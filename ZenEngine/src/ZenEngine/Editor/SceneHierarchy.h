#pragma once

#include "Editor.h"
#include "ZenEngine/ECS/Entity.h"

namespace ZenEngine
{

    class SceneHierarchy : public EditorWindow
    {
    public:
        SceneHierarchy() : EditorWindow("Scene Hierarchy", true) {}
        virtual void OnRenderWindow() override;

    private:
        void DrawEntityNode(Entity &entity);
    };
}