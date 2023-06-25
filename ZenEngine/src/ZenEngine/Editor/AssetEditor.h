#pragma once

#include "EditorWindow.h"
#include "ZenEngine/Asset/AssetManager.h"

namespace ZenEngine
{
    class AssetEditor : public EditorWindow
    {
    public:
        AssetEditor(const std::string &inName, UUID inAssetId) : EditorWindow(inName) {}
        virtual const char *GetAssetClassName() const = 0;
        virtual bool HasViewMenuItem() const override { return false; }

        // for warning the user before closing
        bool IsSaved() const { return !mEdited; }
        void Edited() { mEdited = true; }
        void Saved() { mEdited = false; }
    private:
        bool mEdited = false;
    };

    template <typename T>
    concept IsAssetEditor = std::derived_from<T, AssetEditor> && !std::is_abstract_v<T>;

    template <IsAssetInstance T>
    class AssetEditorFor : public AssetEditor
    {
    public:
        using AssetInstanceType = T;
        AssetEditorFor(UUID inAssetId) : AssetEditor(fmt::format("{} Editor", T::GetStaticAssetClassName()), inAssetId) 
        { mAssetInstance = AssetManager::Get().LoadAssetAs<T>(inAssetId); }
        virtual const char *GetAssetClassName() const override { return T::GetStaticAssetClassName(); }
    protected:
        std::shared_ptr<T> mAssetInstance;
    };

    template <typename T, typename AssetInstance>
    concept IsAssetEditorFor = std::derived_from<T, AssetEditorFor<AssetInstance>>;
}