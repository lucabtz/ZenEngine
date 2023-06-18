#include "OpenGLUniformBuffer.h"

#include <glad/glad.h>

namespace ZenEngine
{
    OpenGLUniformBuffer::OpenGLUniformBuffer(uint32_t inSize, uint32_t inBinding)
        : mBinding(inBinding)
    {
        glCreateBuffers(1, &mRendererId);
		glNamedBufferData(mRendererId, inSize, nullptr, GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_UNIFORM_BUFFER, inBinding, mRendererId);
    }

    OpenGLUniformBuffer::~OpenGLUniformBuffer()
    {
        glDeleteBuffers(1, &mRendererId);
    }

    void OpenGLUniformBuffer::Bind()
    {
        Bind(mBinding);
    }

    void OpenGLUniformBuffer::Bind(uint32_t inBinding)
    {
        glBindBufferBase(GL_UNIFORM_BUFFER, inBinding, mRendererId);
    }

    void OpenGLUniformBuffer::SetData(const void *inData, uint32_t inSize, uint32_t inOffset)
    {
        glNamedBufferSubData(mRendererId, inOffset, inSize, inData);
    }
}