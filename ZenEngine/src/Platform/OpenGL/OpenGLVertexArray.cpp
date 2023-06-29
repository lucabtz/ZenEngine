#include "OpenGLVertexArray.h"

#include <glad/glad.h>

#include "ZenEngine/Core/Macros.h"
#include "ZenEngine/Renderer/VertexBuffer.h"
#include "ZenEngine/Renderer/IndexBuffer.h"

namespace ZenEngine
{
    static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
    {
        switch (type)
        {
        case ShaderDataType::Float:    return GL_FLOAT;
        case ShaderDataType::Float2:   return GL_FLOAT;
        case ShaderDataType::Float3:   return GL_FLOAT;
        case ShaderDataType::Float4:   return GL_FLOAT;
        case ShaderDataType::Mat3:     return GL_FLOAT;
        case ShaderDataType::Mat4:     return GL_FLOAT;
        case ShaderDataType::Int:      return GL_INT;
        case ShaderDataType::Int2:     return GL_INT;
        case ShaderDataType::Int3:     return GL_INT;
        case ShaderDataType::Int4:     return GL_INT;
        case ShaderDataType::Bool:     return GL_BOOL;
        }

        ZE_ASSERT_CORE_MSG(false, "Unknown ShaderDataType!");
        return 0;
    }

    OpenGLVertexArray::OpenGLVertexArray()
    {
        glCreateVertexArrays(1, &mRendererId);
    }

    OpenGLVertexArray::~OpenGLVertexArray()
    {
        glDeleteVertexArrays(1, &mRendererId);
    }

    void OpenGLVertexArray::Bind() const
    {
        glBindVertexArray(mRendererId);
    }

    void OpenGLVertexArray::Unbind() const
    {
        glBindVertexArray(0);
    }

    void OpenGLVertexArray::AddVertexBuffer(const std::shared_ptr<VertexBuffer>& inVertexBuffer)
    {
        ZE_ASSERT_CORE_MSG(inVertexBuffer->GetLayout().GetElements().size(), "Vertex Buffer has no layout!");

        glBindVertexArray(mRendererId);
        inVertexBuffer->Bind();

        const auto& layout = inVertexBuffer->GetLayout();
        for (const auto& element : layout)
        {
            switch (element.Type)
            {
            case ShaderDataType::Float:
            case ShaderDataType::Float2:
            case ShaderDataType::Float3:
            case ShaderDataType::Float4:
            {
                glEnableVertexAttribArray(mVertexBufferIndex);
                glVertexAttribPointer(mVertexBufferIndex,
                    element.GetComponentCount(),
                    ShaderDataTypeToOpenGLBaseType(element.Type),
                    element.Normalized ? GL_TRUE : GL_FALSE,
                    layout.GetStride(),
                    (const void*)element.Offset);
                mVertexBufferIndex++;
                break;
            }
            case ShaderDataType::Int:
            case ShaderDataType::Int2:
            case ShaderDataType::Int3:
            case ShaderDataType::Int4:
            case ShaderDataType::Bool:
            {
                glEnableVertexAttribArray(mVertexBufferIndex);
                glVertexAttribIPointer(mVertexBufferIndex,
                    element.GetComponentCount(),
                    ShaderDataTypeToOpenGLBaseType(element.Type),
                    layout.GetStride(),
                    (const void*)element.Offset);
                mVertexBufferIndex++;
                break;
            }
            case ShaderDataType::Mat3:
            case ShaderDataType::Mat4:
            {
                uint8_t count = element.GetComponentCount();
                for (uint8_t i = 0; i < count; i++)
                {
                    glEnableVertexAttribArray(mVertexBufferIndex);
                    glVertexAttribPointer(mVertexBufferIndex,
                        count,
                        ShaderDataTypeToOpenGLBaseType(element.Type),
                        element.Normalized ? GL_TRUE : GL_FALSE,
                        layout.GetStride(),
                        (const void*)(element.Offset + sizeof(float) * count * i));
                    glVertexAttribDivisor(mVertexBufferIndex, 1);
                    mVertexBufferIndex++;
                }
                break;
            }
            default:
                ZE_ASSERT_CORE_MSG(false, "Unknown ShaderDataType!");
            }
        }

        mVertexBuffers.push_back(inVertexBuffer);
    }

    void OpenGLVertexArray::SetIndexBuffer(const std::shared_ptr<IndexBuffer> &indexBuffer)
    {
        glBindVertexArray(mRendererId);
        indexBuffer->Bind();
        glBindVertexArray(0);

        mIndexBuffer = indexBuffer;
    }
}