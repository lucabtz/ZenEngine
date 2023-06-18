#include "UniformBuffer.h"

#include "RendererAPI.h"

#include "ZenEngine/Core/Macros.h"

#include "Platform/OpenGL/OpenGLUniformBuffer.h"

namespace ZenEngine
{
    std::shared_ptr<UniformBuffer> UniformBuffer::Create(uint32_t inSize, uint32_t inBinding)
    {
        switch (RendererAPI::GetAPI())
        {
        case RendererAPI::API::None: ZE_ASSERT_CORE_MSG(false, "RendererAPI::None is not supported!"); return nullptr;
        case RendererAPI::API::OpenGL: return std::make_unique<OpenGLUniformBuffer>(inSize, inBinding);
        }
        ZE_ASSERT_CORE_MSG(false, "Unknown Renderer API!");
        return nullptr;
    }
}