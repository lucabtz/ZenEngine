#include "RenderContext.h"

#include "RendererAPI.h"

#include "ZenEngine/Core/Macros.h"
#include "ZenEngine/Core/Window.h"

#include "Platform/OpenGL/OpenGLGLFWRenderContext.h"

#include <GLFW/glfw3.h>


namespace ZenEngine
{

    std::unique_ptr<RenderContext> RenderContext::Create(void *inNativeWindow)
    {
        switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:    ZE_ASSERT_CORE_MSG(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  
        {
            switch (Window::GetWindowPlatform())
            {
            case WindowPlatform::GLFW: return std::make_unique<OpenGLGLFWRenderContext>(static_cast<GLFWwindow*>(inNativeWindow));
            default:                   ZE_ASSERT_CORE_MSG(false, "The window platform is currently not supported by OpenGL!"); return nullptr;
            }
        }
        }

		ZE_ASSERT_CORE_MSG(false, "Unknown RendererAPI!");
		return nullptr;
    }

}