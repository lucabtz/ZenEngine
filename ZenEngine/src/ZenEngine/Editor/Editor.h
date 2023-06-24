#pragma once

#include "ZenEngine/Core/Layer.h"
#include "ZenEngine/ECS/Scene.h"
#include "ZenEngine/ECS/Entity.h"

#include "EditorWindow.h"
#include "AssetEditor.h"

namespace ZenEngine
{
    class Editor : public Layer
    {
    public:
        Editor();

        virtual void OnAttach() override;
        virtual void OnRenderEditorGUI() override;
        virtual void OnUpdate(float inDeltaTime) override;
        virtual void OnRender(float inDeltaTime) override;

        virtual void OnEvent(const std::unique_ptr<Event> &inEvent) override;

        void BeginRenderGame();
        void EndRenderGame();

        void RegisterEditorWindow(std::unique_ptr<EditorWindow> inWindow);

        Entity CurrentlySelectedEntity = Entity::Null;
        std::shared_ptr<Scene> &GetActiveScene() { return mActiveScene; }

        template <typename T>
        void OpenAsset(UUID inUUID)
        {
            mAssetEditors.push_back(std::make_unique<typename AssetEditorAssociation<T>::EditorType>(inUUID));
        }

        static Editor &Get() { ZE_ASSERT_CORE_MSG(sEditorInstance != nullptr, "Editor does not exist!"); return *sEditorInstance; }
    private:
        std::vector<std::unique_ptr<EditorWindow>> mEditorWindows;
        std::vector<std::unique_ptr<AssetEditor>> mAssetEditors;
        
        std::shared_ptr<Scene> mActiveScene;

        static Editor *sEditorInstance;

        void ImportClicked();
        void NewScene();
    };
    
}