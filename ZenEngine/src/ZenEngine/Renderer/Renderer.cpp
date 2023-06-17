#include "Renderer.h"

namespace ZenEngine
{

    void Renderer::Init(const std::unique_ptr<Window> &inWindow)
    {
        mRendererAPI = RendererAPI::Create();
        mRenderContext = RenderContext::Create(inWindow->GetNativeWindow());
        mRenderContext->Init();
        mRendererAPI->Init();
        mEditorGUI = std::make_unique<EditorGUI>();
        mEditorGUI->Init();
    }

    void Renderer::Shutdown()
    {
        mEditorGUI->Shutdown();
    }

    void Renderer::Submit(const std::shared_ptr<class VertexArray> &inVertexArray)
    {
        Get().mRendererAPI->DrawIndexed(inVertexArray);
    }

    void RenderCommand::Clear()
    {
        Renderer::Get().GetRendererAPI()->Clear();
    }

    void RenderCommand::SetClearColor(const glm::vec4 &inColor)
    {
        Renderer::Get().GetRendererAPI()->SetClearColor(inColor);
    }
}
