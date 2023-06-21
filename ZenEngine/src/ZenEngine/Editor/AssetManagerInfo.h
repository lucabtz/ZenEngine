#pragma once

#include "Editor.h"

namespace ZenEngine
{

    class AssetManagerInfo : public EditorWindow
    {
    public:
        AssetManagerInfo() : EditorWindow("Asset Manager Info", false) {}
        virtual void OnRenderWindow() override;
    };
}