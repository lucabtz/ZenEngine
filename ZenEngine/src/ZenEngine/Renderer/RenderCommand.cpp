#include "RenderCommand.h"

namespace ZenEngine
{
    void SetViewport::Execute(const std::unique_ptr<RendererAPI> &inAPI) const
    {
        inAPI->SetViewport(mX, mY, mWidth, mHeight);
    }

    void SetClearColor::Execute(const std::unique_ptr<RendererAPI> &inAPI) const
    {
        inAPI->SetClearColor(mColor);
    }

    void Clear::Execute(const std::unique_ptr<RendererAPI> &inAPI) const
    {
        inAPI->Clear();
    }

    void ReleaseResource::Execute(const std::unique_ptr<RendererAPI> &inAPI) const
    {
        delete mResource;
    }

    void SwapBuffers::Execute(const std::unique_ptr<RendererAPI>& inAPI) const
    {
        Renderer::Get().SwapBuffers();
    }
}