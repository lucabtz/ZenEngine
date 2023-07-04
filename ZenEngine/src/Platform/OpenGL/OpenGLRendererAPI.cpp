#include "OpenGLRendererAPI.h"

#include <glad/glad.h>
#include "ZenEngine/Core/Log.h"
#include "ZenEngine/Core/Macros.h"
#include "ZenEngine/Renderer/IndexBuffer.h"

namespace ZenEngine
{
    static void OpenGLMessageCallback(
        unsigned source,
        unsigned type,
        unsigned id,
        unsigned severity,
        int length,
        const char* message,
        const void* userParam)
    {
        switch (severity)
        {
        case GL_DEBUG_SEVERITY_HIGH:         ZE_CORE_CRITICAL(message); return;
        case GL_DEBUG_SEVERITY_MEDIUM:       ZE_CORE_ERROR(message); return;
        case GL_DEBUG_SEVERITY_LOW:          ZE_CORE_WARN(message); return;
        case GL_DEBUG_SEVERITY_NOTIFICATION: ZE_CORE_TRACE(message); return;
        }
        
        ZE_ASSERT_CORE_MSG(false, "Unknown severity level!");
    }

    static GLuint ClearFlagsToOpenGLFlags(uint32_t inFlags)
    {
        GLuint ret = 0;
        if (inFlags & RendererAPI::ColorBuffer)   ret |= GL_COLOR_BUFFER_BIT;
        if (inFlags & RendererAPI::DepthBuffer)   ret |= GL_DEPTH_BUFFER_BIT;
        if (inFlags & RendererAPI::StencilBuffer) ret |= GL_STENCIL_BUFFER_BIT;
        return ret;
    }

    static GLuint BlendModeToOpenGLBLendMode(RendererAPI::BlendMode inMode)
    {
        switch (inMode)
        {
        case RendererAPI::BlendMode::Add:               return GL_FUNC_ADD;
        case RendererAPI::BlendMode::Subtract:          return GL_FUNC_SUBTRACT;
        case RendererAPI::BlendMode::ReverseSubtract:   return GL_FUNC_REVERSE_SUBTRACT;
        case RendererAPI::BlendMode::Max:               return GL_MAX;
        case RendererAPI::BlendMode::Min:               return GL_MIN;
        }
        ZE_ASSERT_CORE_MSG(false, "Unsupported blend mode!");
    }

    static GLuint BlendFunctionToOpenGLBlendFunction(RendererAPI::BlendFunction inFunc)
    {
        switch (inFunc)
        {
        case RendererAPI::BlendFunction::Zero:                      return GL_ZERO;
        case RendererAPI::BlendFunction::One:                       return GL_ONE;
        case RendererAPI::BlendFunction::SourceColor:               return GL_SRC_COLOR;
        case RendererAPI::BlendFunction::OneMinusSourceColor:       return GL_ONE_MINUS_SRC_COLOR;
        case RendererAPI::BlendFunction::DestinationColor:          return GL_DST_COLOR;
        case RendererAPI::BlendFunction::OneMinusDestinationColor:  return GL_ONE_MINUS_DST_COLOR;
        case RendererAPI::BlendFunction::SourceAlpha:               return GL_SRC_ALPHA;
        case RendererAPI::BlendFunction::OneMinusSourceAlpha:       return GL_ONE_MINUS_SRC_ALPHA;
        case RendererAPI::BlendFunction::DestinationAlpha:          return GL_DST_ALPHA;
        case RendererAPI::BlendFunction::OneMinusDestinationAlpha:  return GL_ONE_MINUS_DST_ALPHA;
        case RendererAPI::BlendFunction::ConstantColor:             return GL_CONSTANT_COLOR;
        case RendererAPI::BlendFunction::OneMinusConstantColor:     return GL_ONE_MINUS_CONSTANT_COLOR;
        case RendererAPI::BlendFunction::ConstantAlpha:             return GL_CONSTANT_ALPHA;
        case RendererAPI::BlendFunction::OneMinusConstantAlpha:     return GL_ONE_MINUS_CONSTANT_ALPHA;
        }
        ZE_ASSERT_CORE_MSG(false, "Unsupported blend func!");
    }

    OpenGLRendererAPI::~OpenGLRendererAPI()
    {
    }

    void OpenGLRendererAPI::Init()
    {
#ifdef ZE_DEBUG
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(OpenGLMessageCallback, nullptr);
        
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE);
#endif

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_LINE_SMOOTH);
    }

    void OpenGLRendererAPI::SetViewport(uint32_t inX, uint32_t inY, uint32_t inWidth, uint32_t inHeight)
    {
        glViewport(inX, inY, inWidth, inHeight);
    }

    void OpenGLRendererAPI::SetClearColor(const glm::vec4 &inColor)
    {
        glClearColor(inColor.r, inColor.g, inColor.b, inColor.a);
    }

    void OpenGLRendererAPI::Clear(uint32_t inFlags)
    {
        glClear(ClearFlagsToOpenGLFlags(inFlags));
    }

    void OpenGLRendererAPI::EnableDepthTest()
    {
        glEnable(GL_DEPTH_TEST);
    }

    void OpenGLRendererAPI::DisableDepthTest()
    {
        glDisable(GL_DEPTH_TEST);
    }

    void OpenGLRendererAPI::SetDepthMask(bool inMask)
    {
        glDepthMask(inMask? GL_TRUE : GL_FALSE);
    }

    void OpenGLRendererAPI::EnableBlend()
    {
        glEnable(GL_BLEND);
    }

    void OpenGLRendererAPI::DisableBlend()
    {
        glDisable(GL_BLEND);
    }

    void OpenGLRendererAPI::SetBlendMode(BlendMode inMode)
    {
        glBlendEquation(BlendModeToOpenGLBLendMode(inMode));
    }

    void OpenGLRendererAPI::SetBlendFunction(BlendFunction inSource, BlendFunction inDestination)
    {
        glBlendFunc(BlendFunctionToOpenGLBlendFunction(inSource), BlendFunctionToOpenGLBlendFunction(inDestination));
    }

    void OpenGLRendererAPI::DrawIndexed(const std::shared_ptr<VertexArray> &inVertexArray)
    {
        inVertexArray->Bind();
        glDrawElements(GL_TRIANGLES, inVertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
        inVertexArray->Unbind();
    }

    void OpenGLRendererAPI::DrawIndexed(const std::shared_ptr<VertexArray> &inVertexArray, uint32_t inIndexCount)
    {
        inVertexArray->Bind();
        glDrawElements(GL_TRIANGLES, inIndexCount, GL_UNSIGNED_INT, nullptr);
        inVertexArray->Unbind();
    }

    void OpenGLRendererAPI::DrawLines(const std::shared_ptr<VertexArray> &inVertexArray, uint32_t inVertexCount)
    {
        inVertexArray->Bind();
        glDrawArrays(GL_LINES, 0, inVertexCount);
    }

    void OpenGLRendererAPI::SetLineWidth(float inWidth)
    {
        glLineWidth(inWidth);
    }
}