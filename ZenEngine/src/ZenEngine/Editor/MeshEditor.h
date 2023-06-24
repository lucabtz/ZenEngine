#pragma once

#include "AssetEditor.h"
#include "ZenEngine/Asset/StaticMesh.h"

namespace ZenEngine
{
    class MeshEditor : public AssetEditorFor<StaticMesh>
    {
    public:
        virtual void OnRenderWindow() override;
    };
}
