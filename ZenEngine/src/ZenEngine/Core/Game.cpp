#include "Game.h"
#include "ZenEngine/Event/WindowEvents.h"

#include "Time.h"
#include "ZenEngine/Renderer/Renderer.h"
#include "ZenEngine/Editor/Editor.h"

namespace ZenEngine
{
    Game *Game::sGameInstance = nullptr;


    Game::Game(const std::string &inName, const RuntimeInfo &inRuntimeInfo)
        : mName(inName),
        mRuntimeInfo(inRuntimeInfo),
        mIsRunning(false)
    {
        ZE_ASSERT_CORE_MSG(sGameInstance == nullptr, "A Game already exists!");
        sGameInstance = this;
    }

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
        RenderCommand::SetClearColor({ 0.0f, 0.0f, 0.0f, 1.0f });

        mIsRunning = true;
    }

    void Game::GameUpdate(float inDeltaTime)
    {
        for (auto &layer : mLayerStack)
			layer->OnUpdate(inDeltaTime);
    }

    void Game::GameRender(float inDeltaTime)
    {
        for (auto &layer : mLayerStack)
			layer->OnRender(inDeltaTime);
    }

    void Game::Run()
    {

    #ifdef WITH_EDITOR
        ZE_CORE_INFO("Pushing editor layer");
        mLayerStack.PushLayer(std::make_unique<Editor>());
    #endif
        OnInitialize();

        mLastFrameTime = Time::GetTime();
        while (mIsRunning)
        {
            double time = Time::GetTime();
            float ellapsed = (float)(time - mLastFrameTime);
            mLastFrameTime = time;



        #ifdef WITH_EDITOR
            Editor::Get().BeginRenderGame();
        #endif
            RenderCommand::Clear();
            GameRender(ellapsed);

        #ifdef WITH_EDITOR
            Editor::Get().EndRenderGame();
        #endif
           
        // we renderer EditorGUI only if we are using the editor or we are targetting a debug build (for debug console and such)
        #if defined(WITH_EDITOR) || defined(ZE_DEBUG)
            EditorGUI::Get().BeginGUI();

            for (auto &layer : mLayerStack)
            {
                layer->OnRenderEditorGUI();
            }

            EditorGUI::Get().EndGUI();
        #endif
        
            mWindow->OnUpdate();
            while (EventBus::Get().HasEvents())
            {
                auto event = EventBus::Get().PopEvent();
                HandleEvent(event);
            }

            GameUpdate(ellapsed);
            Renderer::SwapBuffers();
        }

    }

    void Game::Close()
    {
        mIsRunning = false;
    }

    void Game::HandleEvent(std::unique_ptr<Event> &inEvent)
    {
        EventHandler handler(inEvent);
        handler.Handle<WindowCloseEvent>([this](auto event)
        {
            ZE_CORE_INFO("Closing");
            Close();
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