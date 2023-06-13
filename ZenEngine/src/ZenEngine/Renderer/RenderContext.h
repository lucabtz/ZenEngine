#pragma once

#include <memory>

namespace ZenEngine
{

    class RenderContext
    {
    public:
        virtual ~RenderContext() = default;
        
        virtual void Init() = 0;
        virtual void SwapBuffers() = 0;

        static std::unique_ptr<RenderContext> Create(void *inNativeWindow);
    };

}