#pragma once


namespace ZenEngine
{
    class Scene;
    
    class System
    {
    public:
        System(Scene *inScene) : mScene(inScene) {};

        virtual ~System() = default;

        virtual void OnUpdate(float inDeltaTime) {}
        virtual void OnRender(float inDeltaTime) {}
    protected:
        Scene *mScene;
    };
}

#define IMPLEMENT_SYSTEM_CLASS(classname) classname(Scene *inScene) : System(inScene) {}