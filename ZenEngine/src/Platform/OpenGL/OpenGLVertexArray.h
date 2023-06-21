#pragma once

#include "ZenEngine/Renderer/VertexArray.h"


namespace ZenEngine
{

    class OpenGLVertexArray : public VertexArray
    {
    public:
        OpenGLVertexArray();
        virtual ~OpenGLVertexArray();

        virtual void Bind() const override;
        virtual void Unbind() const override;

        virtual void AddVertexBuffer(const std::shared_ptr<VertexBuffer> &inVertexBuffer) override;
        virtual void SetIndexBuffer(const std::shared_ptr<IndexBuffer> &inIndexBuffer) override;

        virtual const std::vector<std::shared_ptr<VertexBuffer>> &GetVertexBuffers() const { return mVertexBuffers; }
        virtual const std::shared_ptr<IndexBuffer> &GetIndexBuffer() const { return mIndexBuffer; }
    private:
        uint32_t mRendererId;
        uint32_t mVertexBufferIndex = 0;
        std::vector<std::shared_ptr<VertexBuffer>> mVertexBuffers;
        std::shared_ptr<IndexBuffer> mIndexBuffer;
    };

}