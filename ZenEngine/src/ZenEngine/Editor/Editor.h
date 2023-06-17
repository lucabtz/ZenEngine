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
        virtual void OnOpen() {}
        virtual void OnClose() {}
        virtual void OnInitializeStyle() {}
        virtual void OnClearStyle() {}

        virtual void OnBeginRenderGame() {}
        virtual void OnEndRenderGame() {}

        void Open();
        void Close();


        bool IsOpen() { return mIsOpen; }
        bool CanBeClosed() { return mCanBeClosed; }

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

        void BeginRenderGame();
        void EndRenderGame();

        void RegisterEditorWindow(std::unique_ptr<EditorWindow> inWindow);

        static Editor &Get() { ZE_ASSERT_CORE_MSG(sEditorInstance != nullptr, "Editor does not exist!"); return *sEditorInstance; }
    private:
        std::vector<std::unique_ptr<EditorWindow>> mEditorWindows;

        static Editor *sEditorInstance;
    };
    
}