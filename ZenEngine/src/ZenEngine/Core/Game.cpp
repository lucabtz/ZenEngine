#include "Game.h"
#include "ZenEngine/Event/WindowEvents.h"

#include "Time.h"
#include "ZenEngine/Renderer/Renderer.h"
#include "ZenEngine/Editor/Editor.h"
#include "ZenEngine/Asset/AssetManager.h"

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

        RenderCommand::SetClearColor({ 0.0f, 0.0f, 0.0f, 0.0f });

        AssetManager::Get().Init();

        EDITOR_ONLY(
        mEditor = std::make_unique<Editor>(); 
        mEditor->Init();
        )
    
        mIsRunning = true;
    }

    void Game::GameUpdate(float inDeltaTime)
    {
        for (auto &layer : mLayerStack)
            layer->OnUpdate(inDeltaTime);
        EDITOR_ONLY(mEditor->OnUpdate(inDeltaTime);)
    }

    void Game::GameRender(float inDeltaTime)
    {
    #ifdef WITH_EDITOR
        mEditor->BeginScene();
    #else
        // TODO
    #endif

        for (auto &layer : mLayerStack)
            layer->OnRender(inDeltaTime);
        EDITOR_ONLY(mEditor->OnRender(inDeltaTime);)
    
    #ifdef WITH_EDITOR
        mEditor->FlushScene();
    #else
        // TODO
    #endif
    }

    void Game::Run()
    {
        OnInitialize();

        mLastFrameTime = Time::GetTime();
        while (mIsRunning)
        {
            double time = Time::GetTime();
            float ellapsed = (float)(time - mLastFrameTime);
            mLastFrameTime = time;

            GameRender(ellapsed);
           
        // we renderer EditorGUI only if we are using the editor or we are targetting a debug build (for debug console and such)
        #if defined(WITH_EDITOR) || defined(ZE_DEBUG)
            EditorGUI::Get().BeginGUI();
            
            for (auto &layer : mLayerStack)
                layer->OnRenderEditorGUI();
            
            EDITOR_ONLY(mEditor->OnRenderEditorGUI();)
            
            EditorGUI::Get().EndGUI();
        #endif
        
            mWindow->OnUpdate();
            while (EventBus::Get().HasEvents())
            {
                auto event = EventBus::Get().PopEvent();
                HandleEvent(event);
            }

            GameUpdate(ellapsed);
        
            Renderer::Get().SwapBuffers();
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

        EDITOR_ONLY(mEditor->OnEvent(inEvent);)

        for (auto it = mLayerStack.rbegin(); it != mLayerStack.rend(); ++it)
        {
            auto & layer = *it;
            layer->OnEvent(inEvent);
            if (inEvent->Handled) break;
        }
    }

}