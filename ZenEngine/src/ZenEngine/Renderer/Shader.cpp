#include "Shader.h"

#include "RendererAPI.h"
#include "Platform/OpenGL/OpenGLShader.h"
#include "ZenEngine/Core/Macros.h"

namespace ZenEngine
{

    std::shared_ptr<Shader> Shader::Create(const std::string& inFilepath)
    {
        switch (RendererAPI::GetAPI())
        {
        case RendererAPI::API::None:    ZE_ASSERT_CORE_MSG(false, "RendererAPI::None is currently not supported!"); return nullptr;
        case RendererAPI::API::OpenGL:  return std::make_shared<OpenGLShader>(inFilepath);
        }

        ZE_ASSERT_CORE_MSG(false, "Unknown RendererAPI!");
        return nullptr;
    }

    std::shared_ptr<Shader> Shader::Create(const std::string &inName, const std::string& inSrc)
    {
        switch (RendererAPI::GetAPI())
        {
        case RendererAPI::API::None:    ZE_ASSERT_CORE_MSG(false, "RendererAPI::None is currently not supported!"); return nullptr;
        case RendererAPI::API::OpenGL:  return std::make_shared<OpenGLShader>(inName, inSrc);
        }

        ZE_ASSERT_CORE_MSG(false, "Unknown RendererAPI!");
        return nullptr;
    }

    std::shared_ptr<Shader> Shader::Create(const std::string &inName, const std::vector<uint32_t> &inVtxSPIRV, const std::vector<uint32_t> &inPixSPIRV)
    {
        switch (RendererAPI::GetAPI())
        {
        case RendererAPI::API::None:    ZE_ASSERT_CORE_MSG(false, "RendererAPI::None is currently not supported!"); return nullptr;
        case RendererAPI::API::OpenGL:  return std::make_shared<OpenGLShader>(inName, inVtxSPIRV, inPixSPIRV);
        }

        ZE_ASSERT_CORE_MSG(false, "Unknown RendererAPI!");
        return nullptr;
    }
}