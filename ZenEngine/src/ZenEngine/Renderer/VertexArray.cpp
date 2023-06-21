#include "VertexArray.h"

#include "VertexBuffer.h"
#include "IndexBuffer.h"

#include "RendererAPI.h"
#include "Platform/OpenGL/OpenGLVertexArray.h"

namespace ZenEngine
{
    std::shared_ptr<VertexArray> VertexArray::Create()
    {
        switch (RendererAPI::GetAPI())
        {
        case RendererAPI::API::None:    ZE_ASSERT_CORE_MSG(false, "RendererAPI::None is currently not supported!"); return nullptr;
        case RendererAPI::API::OpenGL:  return std::make_shared<OpenGLVertexArray>();
        }

        ZE_ASSERT_CORE_MSG(false, "Unknown RendererAPI!");
        return nullptr;
    }

}

