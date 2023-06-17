#include "Texture2D.h"

#include "RendererAPI.h"
#include "ZenEngine/Core/Macros.h"
#include "Platform/OpenGL/OpenGLTexture2D.h"

namespace ZenEngine
{
    std::shared_ptr<Texture2D> Texture2D::Create(const Texture2D::Properties &inProperties)
    {
        switch (RendererAPI::GetAPI())
        {
        case RendererAPI::API::None: ZE_ASSERT_CORE_MSG(false, "RendererAPI::None is currently not supported!"); return nullptr;
        case RendererAPI::API::OpenGL: return std::make_shared<OpenGLTexture2D>(inProperties);
        }
        ZE_ASSERT_CORE_MSG(false, "Unknown renderer API!");
    }
    
    std::shared_ptr<Texture2D> Texture2D::Create(const Texture2D::Properties &inProperties, void *inData, uint32_t inSize)
    {
        auto tex = Create(inProperties);
        tex->SetData(inData, inSize);
        return tex;
    }
}
