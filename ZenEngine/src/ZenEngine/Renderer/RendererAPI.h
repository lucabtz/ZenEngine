#pragma once

#include <stdint.h>
#include <glm/glm.hpp>
#include <memory>

namespace ZenEngine
{
	class RendererAPI
	{
	public:
		enum class API
		{
			None = 0, 
            OpenGL
		};
	public:
		virtual ~RendererAPI() = default;

		virtual void Init() = 0;
		virtual void SetViewport(uint32_t inX, uint32_t inY, uint32_t inWidth, uint32_t inHeight) = 0;
		virtual void SetClearColor(const glm::vec4& inColor) = 0;
		virtual void Clear() = 0;

		virtual void DrawIndexed(const std::shared_ptr<class VertexArray> &inVertexArray) = 0;
		virtual void DrawIndexed(const std::shared_ptr<class VertexArray> &inVertexArray, uint32_t inIndexCount) = 0;
		virtual void DrawLines(const std::shared_ptr<class VertexArray> &inVertexArray, uint32_t inVertexCount) = 0;
		
		virtual void SetLineWidth(float inWidth) = 0;

		static API GetAPI() { return sAPI; }
		static std::unique_ptr<RendererAPI> Create();
	private:
		static API sAPI;
	};

}