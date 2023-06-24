#pragma once

#include "EditorWindow.h"
#include "ZenEngine/Asset/AssetManager.h"

namespace ZenEngine
{
    class AssetEditor : public EditorWindow
    {
    public:
        AssetEditor(UUID inAssetId) : EditorWindow(fmt::format("{} Editor", GetAssetClass()->Name)) {}
        virtual bool HasViewMenuItem() const override { return false; }
        virtual const AssetClass *GetAssetClass() const = 0;

        // for warning the user before closing
        bool IsSaved() const { return !mEdited; }
        void Edited() { mEdited = true; }
        void Saved() { mEdited = false; }
    private:
        bool mEdited = false;
    };

    template <typename T>
    class AssetEditorFor : public AssetEditor
    {
    public:
        AssetEditorFor(UUID inAssetId) : AssetEditor(inAssetId) { mAssetInstance = AssetManager::Get().LoadAssetAs<T>(inUUID); }
        virtual const AssetClass *GetAssetClass() const override { return T::GetStaticAssetClass(); }

    protected:
        std::shared_ptr<T> mAssetInstance;
    };

    template <typename AssetInstanceClass>
    struct AssetEditorAssociation
    {};
}

#define REGISTER_ASSET_EDITOR(assetinstanceclass, asseteditorclass) namespace ZenEngine {\
    template <> struct AssetEditorAssociation<assetinstanceclass>\
    {\
        using EditorType = asseteditorclass;\
    }\
}