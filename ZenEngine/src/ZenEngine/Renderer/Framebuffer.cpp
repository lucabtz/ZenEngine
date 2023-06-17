#include "Framebuffer.h"

#include "RendererAPI.h"

#include "ZenEngine/Core/Macros.h"
#include "Platform/OpenGL/OpenGLFramebuffer.h"

namespace ZenEngine
{

    std::shared_ptr<Framebuffer> Framebuffer::Create(const Properties &inProperties)
    {
        switch (RendererAPI::GetAPI())
        {
        case RendererAPI::API::None: ZE_ASSERT_CORE_MSG(false, "RendererAPI::None is not supported!"); return nullptr;
        case RendererAPI::API::OpenGL: return std::make_unique<OpenGLFramebuffer>(inProperties);
        }
        ZE_ASSERT_CORE_MSG(false, "Unknown renderer API!")
    }
}