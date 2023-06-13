#include "OpenGLIndexBuffer.h"

#include <glad/glad.h>

namespace ZenEngine
{
    	OpenGLIndexBuffer::OpenGLIndexBuffer(const uint32_t* inIndices, uint32_t inCount)
		: mCount(inCount)
	{
		glCreateBuffers(1, &mRendererId);
		
		// GL_ELEMENT_ARRAY_BUFFER is not valid without an actively bound VAO
		// Binding with GL_ARRAY_BUFFER allows the data to be loaded regardless of VAO state. 
		glBindBuffer(GL_ARRAY_BUFFER, mRendererId);
		glBufferData(GL_ARRAY_BUFFER, inCount * sizeof(uint32_t), inIndices, GL_STATIC_DRAW);
	}

	OpenGLIndexBuffer::~OpenGLIndexBuffer()
	{
		glDeleteBuffers(1, &mRendererId);
	}

	void OpenGLIndexBuffer::Bind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mRendererId);
	}

	void OpenGLIndexBuffer::Unbind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}