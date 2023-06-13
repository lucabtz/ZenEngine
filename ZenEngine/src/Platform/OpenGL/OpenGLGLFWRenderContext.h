#pragma once

#include "ZenEngine/Renderer/RenderContext.h"

class GLFWwindow;

namespace ZenEngine
{

    class OpenGLGLFWRenderContext : public RenderContext
    {
    public:
        OpenGLGLFWRenderContext(GLFWwindow *inWindowHandle)
            : mWindowHandle(inWindowHandle)
        {}

        virtual void Init() override;
        virtual void SwapBuffers() override;
    private:
        GLFWwindow *mWindowHandle;
    };

}