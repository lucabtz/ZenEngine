#pragma once

#include "ZenEngine/Renderer/RendererAPI.h"
#include "ZenEngine/Renderer/VertexArray.h"

namespace ZenEngine
{

    class OpenGLRendererAPI : public RendererAPI
    {
    public:
        OpenGLRendererAPI() {}
        virtual ~OpenGLRendererAPI();

        virtual void Init() override;
		virtual void SetViewport(uint32_t inX, uint32_t inY, uint32_t inWidth, uint32_t inHeight) override;
		virtual void SetClearColor(const glm::vec4 &inColor) override;
		virtual void Clear() override;

		virtual void DrawIndexed(const std::shared_ptr<VertexArray> &inVertexArray) override;
		virtual void DrawIndexed(const std::shared_ptr<VertexArray> &inVertexArray, uint32_t inIndexCount) override;
		virtual void DrawLines(const std::shared_ptr<VertexArray> &inVertexArray, uint32_t inVertexCount) override;
		
		virtual void SetLineWidth(float inWidth) override;
    };
}