#pragma once

#include <stdint.h>
#include <glm/glm.hpp>
#include <memory>
#include "ZenEngine/Core/Macros.h"

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

        enum class BlendMode
        {
            Add,
            Subtract,
            ReverseSubtract,
            Min,
            Max
        };

        enum class BlendFunction
        {
            Zero,
            One,
            SourceColor,
            OneMinusSourceColor,
            DestinationColor,
            OneMinusDestinationColor,
            SourceAlpha,
            OneMinusSourceAlpha,
            DestinationAlpha,
            OneMinusDestinationAlpha,
            ConstantColor,
            OneMinusConstantColor,
            ConstantAlpha,
            OneMinusConstantAlpha
        };

        enum ClearFlags : uint32_t
        {
            None = 0,
            ColorBuffer   = BIT(0),
            DepthBuffer   = BIT(1),
            StencilBuffer = BIT(2)
        };

    public:
        virtual ~RendererAPI() = default;

        virtual void Init() = 0;
        virtual void SetViewport(uint32_t inX, uint32_t inY, uint32_t inWidth, uint32_t inHeight) = 0;
        virtual void SetClearColor(const glm::vec4& inColor) = 0;
        virtual void Clear(uint32_t inFlags) = 0;

        // depth test
        virtual void EnableDepthTest() = 0;
        virtual void DisableDepthTest() = 0;
        virtual void SetDepthMask(bool inMask) = 0;

        // blend
        virtual void EnableBlend() = 0;
        virtual void DisableBlend() = 0;
        virtual void SetBlendMode(BlendMode inMode) = 0;
        virtual void SetBlendFunction(BlendFunction inSource, BlendFunction inDestination) = 0;

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