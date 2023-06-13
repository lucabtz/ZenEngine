#include "RendererAPI.h"

#include "ZenEngine/Core/Platform.h"
#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace ZenEngine
{
    RendererAPI::API RendererAPI::sAPI = RendererAPI::API::None;

    std::unique_ptr<RendererAPI> RendererAPI::Create()
    {
#ifdef ZE_RENDERER_PLATFORM_OPENGL
        sAPI = RendererAPI::API::OpenGL;
        return std::make_unique<OpenGLRendererAPI>();
#else
    #error "Renderer unsupported!"
#endif
    }
}