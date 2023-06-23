#pragma once

#include "Editor.h"

namespace ZenEngine
{

    class AssetManagerDatabase : public EditorWindow
    {
    public:
        AssetManagerDatabase() : EditorWindow("Asset Manager Database", false) {}
        virtual void OnRenderWindow() override;
    };
}