#pragma once

#include <queue>
#include <memory>
#include <string>
#include <sstream>
#include <functional>
#include <vector>

#include "RendererAPI.h"
#include "RenderContext.h"

#include "ZenEngine/Core/Log.h"
#include "ZenEngine/Core/Game.h"
#include "ZenEngine/Core/Window.h"
#include "ZenEngine/Editor/EditorGUI.h"

namespace ZenEngine
{
    class Renderer
    {
    public:
        static Renderer &Get()
        {
            static Renderer instance;
            return instance;
        }

        void Init(const std::unique_ptr<Window> &inWindow);
        void Shutdown();

        static void Submit(const std::shared_ptr<class VertexArray> &inVertexArray);

        static void SwapBuffers() { Get().mRenderContext->SwapBuffers(); }

        const std::unique_ptr<RendererAPI> &GetRendererAPI() const { return mRendererAPI; }

    private:
        std::unique_ptr<RendererAPI> mRendererAPI;
        std::unique_ptr<RenderContext> mRenderContext;

        std::unique_ptr<EditorGUI> mEditorGUI;

        Renderer() = default;
        Renderer(const Renderer &) = delete;
        Renderer &operator =(const Renderer &) = delete;

    };

    namespace RenderCommand
    {
        void Clear();
        void SetClearColor(const glm::vec4 &inColor);
    };

}    