#include "Renderer.h"

#include "RenderCommand.h"

namespace ZenEngine
{
    void Renderer::Init(const std::unique_ptr<Window> &inWindow)
    {
        mRenderThreadRunning = true;
        mRenderThread = std::thread([this, &inWindow] 
        {
            mRendererAPI = RendererAPI::Create();
            mRenderContext = RenderContext::Create(inWindow->GetNativeWindow());
            mRenderContext->Init();
            mRendererAPI->Init();
            RenderLoop(); 
        });
    }

    void Renderer::Shutdown()
    {
        mRenderThreadRunning = false;
        mRenderThread.join();
    }

    void Renderer::ExecuteCommands()
    {
        std::lock_guard guard(mQueueMutex);
        while (!mCommandQueue.empty())
        {
            auto cmd = std::move(mCommandQueue.front());
            mCommandQueue.pop();
            ZE_CORE_TRACE("Executing {}", cmd->ToString());
            cmd->Execute(mRendererAPI);
        }
    }


    void Renderer::RenderLoop()
    {
        while (mRenderThreadRunning || !mCommandQueue.empty())
        {
            ExecuteCommands();
        }
    }

    void DrawIndexed::Execute(const std::unique_ptr<RendererAPI> &inAPI) const
    {
        if (mCount == 0)
            inAPI->DrawIndexed(mVAContainer->Resource);
        else
            inAPI->DrawIndexed(mVAContainer->Resource, mCount);
    }

}
    