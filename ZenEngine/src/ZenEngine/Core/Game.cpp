#include "Game.h"
#include "ZenEngine/Event/WindowEvents.h"

#include "ZenEngine/Renderer/RenderCommand.h"

#include "Time.h"

namespace ZenEngine
{

    Game::Game(const std::string &inName, const RuntimeInfo &inRuntimeInfo)
        : mName(inName),
        mRuntimeInfo(inRuntimeInfo),
        mRunning(false)
    {}

    Game::~Game()
    {
        Renderer::Get().Shutdown();
    }

    void Game::Init()
    {
        ZE_CORE_INFO("Initializing game {}", mName);


        WindowInfo windowInfo{};
        windowInfo.Title = mName;
        windowInfo.Width = 1920;
        windowInfo.Height = 1280;
        mWindow = Window::Create(windowInfo);
        
        Renderer::Get().Init(mWindow);

        mRunning = true;

        OnInitialize();
    }

    void Game::Update(float inDeltaTime)
    {
        for (auto &layer : mLayerStack)
			layer->OnUpdate(inDeltaTime);
    }

    void Game::Run()
    {
        mLastFrameTime = Time::GetTime();
        while (mRunning)
        {
            double time = Time::GetTime();
            float ellapsed = (float)(time - mLastFrameTime);
            mLastFrameTime = time;

            mWindow->OnUpdate();

            Update(ellapsed);

            while (EventBus::Get().HasEvents())
            {
                auto event = EventBus::Get().PopEvent();
                HandleEvent(event);
            }
        }
    }

    void Game::HandleEvent(std::unique_ptr<Event> &inEvent)
    {
        EventHandler handler(inEvent);
        handler.Handle<WindowCloseEvent>([this](auto event)
        {
            mRunning = false;
            event->Handled = true;
        });

        if (inEvent->Handled) return;

        for (auto it = mLayerStack.rbegin(); it != mLayerStack.rend(); ++it)
        {
            auto & layer = *it;
            layer->OnEvent(inEvent);
            if (inEvent->Handled) break;
        }
    }

}