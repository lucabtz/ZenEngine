#include "OpenGLRendererAPI.h"

#include <glad/glad.h>
#include "ZenEngine/Core/Log.h"
#include "ZenEngine/Core/Macros.h"
#include "ZenEngine/Renderer/IndexBuffer.h"

namespace ZenEngine
{
    void OpenGLMessageCallback(
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

    void OpenGLRendererAPI::Clear()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void OpenGLRendererAPI::DrawIndexed(const std::shared_ptr<VertexArray> &inVertexArray)
    {
        inVertexArray->Bind();
        //inVertexArray->GetIndexBuffer()->Bind();
        glDrawElements(GL_TRIANGLES, inVertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
        inVertexArray->Unbind();
    }

    void OpenGLRendererAPI::DrawIndexed(const std::shared_ptr<VertexArray> &inVertexArray, uint32_t inIndexCount)
    {
        inVertexArray->Bind();
        //inVertexArray->GetIndexBuffer()->Bind();
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