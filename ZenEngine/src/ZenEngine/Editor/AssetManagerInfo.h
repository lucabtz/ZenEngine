#pragma once

#include "Editor.h"
#include "EditorCamera.h"

#include "ZenEngine/Renderer/Framebuffer.h"

namespace ZenEngine
{

    class AssetManagerInfo : public EditorWindow
    {
    public:
        AssetManagerInfo() : EditorWindow("Asset Manager Info", false) {}
        virtual void OnRenderWindow() override;
    };
}