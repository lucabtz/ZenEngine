#pragma once

#include <string>
#include <memory>
#include <filesystem>
#include "ZenEngine/Core/Macros.h"
#include "ZenEngine/Core/Window.h"
#include "ZenEngine/Event/EventBus.h"
#include "ZenEngine/Editor/EditorGUI.h"

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
        void GameRender(float inDeltaTime);
        void GameUpdate(float inDeltaTime);

        void Run();

        void Close();

        // Client events
        virtual void OnInitialize() {}

        LayerStack &GetLayerStack() { return mLayerStack; }

        std::unique_ptr<Window> &GetWindow() { return mWindow; }

        static Game &Get() { ZE_ASSERT_CORE_MSG(sGameInstance != nullptr, "Game instance not initialized yet!"); return *sGameInstance; }
        static bool IsRunning() { return Get().mIsRunning; }
    private:
        std::string mName;
        RuntimeInfo mRuntimeInfo;
        double mLastFrameTime;
        LayerStack mLayerStack;

        std::unique_ptr<Window> mWindow;

        bool mIsRunning;

        void HandleEvent(std::unique_ptr<Event> &inEvent);
    
        static Game *sGameInstance;
    };

}