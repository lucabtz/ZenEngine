#pragma once

#include "ZenEngine/Core/Layer.h"

namespace ZenEngine
{
    class EditorWindow
    {
    public:
        EditorWindow(const std::string inName, bool inIsDefaulOpen = false, bool inCanBeClosed = true) : mName(inName), mIsOpen(inIsDefaulOpen), mCanBeClosed(inCanBeClosed) {}

        virtual void OnRegister() {}
        virtual void OnRenderWindow() {}
        virtual void OnInitializeStyle() {}
        virtual void OnClearStyle() {}

        virtual void OnUpdate(float inDeltaTime) {}
        virtual void OnEvent(const std::unique_ptr<Event> &inEvent) {}

        virtual void OnBeginRenderGame() {}
        virtual void OnEndRenderGame() {}

        void Open();

        bool IsOpen() { return mIsOpen; }
        bool CanBeClosed() { return mCanBeClosed; }

        bool *GetOpenHandle() { return &mIsOpen; }

        const std::string &GetName() { return mName; }

    private:
        std::string mName;
        bool mCanBeClosed;

        bool mIsOpen;
    };

    class Editor : public Layer
    {
    public:
        Editor();

        virtual void OnAttach() override;
        virtual void OnRenderEditorGUI() override;
        virtual void OnUpdate(float inDeltaTime) override;

        virtual void OnEvent(const std::unique_ptr<Event> &inEvent) override;

        void BeginRenderGame();
        void EndRenderGame();

        void RegisterEditorWindow(std::unique_ptr<EditorWindow> inWindow);

        static Editor &Get() { ZE_ASSERT_CORE_MSG(sEditorInstance != nullptr, "Editor does not exist!"); return *sEditorInstance; }
    private:
        std::vector<std::unique_ptr<EditorWindow>> mEditorWindows;

        static Editor *sEditorInstance;

        void ImportClicked();
    };
    
}