#include "OpenGLVertexBuffer.h"

#include <glad/glad.h>

namespace ZenEngine
{
    OpenGLVertexBuffer::OpenGLVertexBuffer(uint32_t inSize)
	{
		glCreateBuffers(1, &mRendererId);
		glBindBuffer(GL_ARRAY_BUFFER, mRendererId);
		glBufferData(GL_ARRAY_BUFFER, inSize, nullptr, GL_DYNAMIC_DRAW);
	}

	OpenGLVertexBuffer::OpenGLVertexBuffer(const float* inVertices, uint32_t inSize)
	{
		glCreateBuffers(1, &mRendererId);
		glBindBuffer(GL_ARRAY_BUFFER, mRendererId);
		glBufferData(GL_ARRAY_BUFFER, inSize, inVertices, GL_STATIC_DRAW);
	}

    OpenGLVertexBuffer::~OpenGLVertexBuffer()
	{
		glDeleteBuffers(1, &mRendererId);
	}

	void OpenGLVertexBuffer::Bind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, mRendererId);
	}

	void OpenGLVertexBuffer::Unbind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

    void OpenGLVertexBuffer::SetData(const void* inData, uint32_t inSize)
	{
		glBindBuffer(GL_ARRAY_BUFFER, mRendererId);
		glBufferSubData(GL_ARRAY_BUFFER, 0, inSize, inData);
	}
}