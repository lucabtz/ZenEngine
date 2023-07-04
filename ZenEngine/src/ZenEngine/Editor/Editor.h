#pragma once

#include "ZenEngine/ECS/Entity.h"
#include "ZenEngine/ECS/Scene.h"

#include "EditorWindow.h"
#include "AssetEditor.h"

namespace ZenEngine
{
    class Editor
    {
    public:
        Editor();

        void Init();
        void OnRenderEditorGUI();
        void OnUpdate(float inDeltaTime);
        void OnRender(float inDeltaTime);
        void BeginScene();
        void FlushScene();

        void OnEvent(const std::unique_ptr<Event> &inEvent);

        void RegisterEditorWindow(std::unique_ptr<EditorWindow> inWindow);

        std::shared_ptr<Scene> &GetActiveScene() { return mActiveScene; }

        void OpenAsset(UUID inUUID);

        template <IsAssetEditor T>
        void RegisterAssetEditor()
        {
            mAssetEditorInstatiators[T::AssetInstanceType::GetStaticAssetClassName()] = [](UUID id)
            {
                return std::make_unique<T>(id);
            };
        }

        static Editor &Get() { ZE_ASSERT_CORE_MSG(sEditorInstance != nullptr, "Editor does not exist!"); return *sEditorInstance; }
        
        Entity CurrentlySelectedEntity = Entity::Null;
    private:
        std::vector<std::unique_ptr<EditorWindow>> mEditorWindows;
        std::unordered_map<UUID, std::unique_ptr<AssetEditor>> mAssetEditors;
        
        using AssetEditorInstantiator = std::function<std::unique_ptr<AssetEditor>(UUID)>;
        std::unordered_map<const char*, AssetEditorInstantiator> mAssetEditorInstatiators;
        
        std::shared_ptr<Scene> mActiveScene;

        static Editor *sEditorInstance;

        void ImportClicked();
        void NewScene();
    };
    
}