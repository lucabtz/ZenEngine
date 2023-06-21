#pragma once

#include "System.h"

namespace ZenEngine
{

    class StaticMeshRendererSystem : public System
    {
    public:
        IMPLEMENT_SYSTEM_CLASS(StaticMeshRendererSystem)

        virtual void OnRender(float inDeltaTime) override;
    };

}