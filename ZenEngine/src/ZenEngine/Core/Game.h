#pragma once

#include <string>
#include <memory>
#include <filesystem>
#include "ZenEngine/Core/Macros.h"
#include "ZenEngine/Core/Window.h"
#include "ZenEngine/Event/EventBus.h"

#include "Layer.h"

namespace ZenEngine
{

    struct CommandLineArgs
    {
        int Count;
        char **Args;

        char *operator[] (int Index)
        {
            ZE_ASSERT(Index < Count);
            return Args[Index];
        }
    };

    struct RuntimeInfo
    {
        std::filesystem::path WorkingDirectory;
        CommandLineArgs CmdLine;
    };

    class Game
    {
    public:
        Game(const std::string &inName, const RuntimeInfo &inRuntimeInfo);
        virtual ~Game();

        void Init();
        void Update(float inDeltaTime);

        void Run();

        // Client events
        virtual void OnInitialize() {}

        LayerStack &GetLayerStack() { return mLayerStack; }
    private:
        std::string mName;
        RuntimeInfo mRuntimeInfo;
        std::unique_ptr<Window> mWindow;
        double mLastFrameTime;
        LayerStack mLayerStack;

        bool mRunning;

        void HandleEvent(std::unique_ptr<Event> &inEvent);
    };

}