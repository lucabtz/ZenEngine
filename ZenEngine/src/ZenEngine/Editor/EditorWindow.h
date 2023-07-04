#pragma once

#include <string>
#include "ZenEngine/Event/Event.h"

namespace ZenEngine
{
    class EditorWindow
    {
    public:
        EditorWindow(const std::string inName, bool inIsDefaulOpen = false, bool inCanBeClosed = true) : mName(inName), mIsOpen(inIsDefaulOpen), mCanBeClosed(inCanBeClosed) {}
        virtual ~EditorWindow() = default;

        virtual void OnRegister() {}
        virtual void OnRenderWindow() {}
        virtual void OnInitializeStyle() {}
        virtual void OnClearStyle() {}

        virtual void OnUpdate(float inDeltaTime) {}
        virtual void OnEvent(const std::unique_ptr<Event> &inEvent) {}

        virtual bool HasViewMenuItem() const { return true; }

        void Open() { if (!mIsOpen) mIsOpen = true; }

        bool IsOpen() { return mIsOpen; }
        bool CanBeClosed() { return mCanBeClosed; }

        bool *GetOpenHandle() { return &mIsOpen; }

        const std::string &GetName() { return mName; }

    private:
        std::string mName;
        bool mCanBeClosed;

        bool mIsOpen;
    };

}