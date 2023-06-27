#pragma once

#include "AssetEditor.h"
#include "ZenEngine/Asset/Texture2DAsset.h"

namespace ZenEngine
{
    class Texture2DEditor : public AssetEditorFor<Texture2DAsset>
    {
    public:
        Texture2DEditor(UUID inUUID) : AssetEditorFor(inUUID) {}
        virtual void OnRenderWindow() override;
    };
}