#pragma once

#include <queue>
#include <memory>
#include <string>
#include <sstream>
#include <mutex>
#include <functional>
#include <vector>

#include "ZenEngine/Core/Log.h"

#include "RendererAPI.h"
#include "RenderCommand.h"
#include "RenderContext.h"

#include "ZenEngine/Core/Window.h"

namespace ZenEngine
{
    class Renderer
    {
    public:
        static Renderer &Get()
        {
            static Renderer instance;
            return instance;
        }

        // MAIN THREAD FUNCTIONS
        void Init(const std::unique_ptr<Window> &inWindow);
        void Shutdown();


        template<typename T, typename ... Args>
        void Submit(Args&& ... args)
        {
            std::lock_guard guard(mQueueMutex);
            auto cmd = std::make_unique<T>(std::forward<Args>(args)...);
            ZE_CORE_TRACE("Render command pushed: {}", cmd->ToString());
            mCommandQueue.push(std::move(cmd));
        }
        
        // RENDER THREAD FUNCTIONS
        void SwapBuffers() { mRenderContext->SwapBuffers(); }
    private:
        std::mutex mQueueMutex;
        std::queue<std::unique_ptr<RenderCommand>> mCommandQueue;
        std::unique_ptr<RendererAPI> mRendererAPI;
        std::thread mRenderThread;
        bool mRenderThreadRunning = false;
        std::unique_ptr<RenderContext> mRenderContext;

        Renderer() = default;
        Renderer(const Renderer &) = delete;
        Renderer &operator =(const Renderer &) = delete;

        // PRIVATE RENDER THREAD FUNCTIONS
        void ExecuteCommands();
        void RenderLoop();
    };

    struct RenderResourceContainerBase
    {
        virtual ~RenderResourceContainerBase() = default;
    };

    /// @brief A RenderResourceContainer holds a std::shared_ptr<T> to a render resource. This RenderResources are in turn managed by the main thread by RenderHandles
    ///        in this way it is possible for the following behaviour:
    ///             - as long as a RenderHandle pointing to a RenderResourceContainer exists on the main thread there is at least one std::shared_ptr to the RenderResourceContainer
    ///             - on the render thread this std::shared_ptr might be copied around however on the main thread this shared_ptr is never touched and only
    ///               render handles should be copied around, which in turn make sure that there is at least one valid shared_ptr to the resource
    ///             - once all the handles on the main thread pointing to the specific resource are gone the RenderResourceContainer is queued for deletion on the render thread,
    ///               however if on the render thread references to the underlying shared_ptr still exist the actual resource will not be deleted as long as those references
    ///               are invalidated
    ///             - the render resource is actually deleted on the render thread so that the destructor of T is called on the render thread since it might call the underlying
    ///               rendering API
    /// @tparam T the render resource stored
    template<typename T>
    struct RenderResourceContainer : public RenderResourceContainerBase
    {
        std::shared_ptr<T> Resource;

        virtual ~RenderResourceContainer() = default;
    };
    
    /// @brief This class represents a renderer resource (such as vertex buffer, index buffer, vertex array, shader, etc.) on the main thread. 
    ///        When no more handles exist on the main thread the resource can be deleted
    /// @tparam T the resource class
    template<typename T>
    class RenderHandle
    {
    public:
        RenderHandle()
        {
            mResource = nullptr;
            mHandleCount = nullptr;
        }

        RenderHandle(RenderResourceContainer<T> *inResource)
            : mResource(inResource)
        {
            mHandleCount = new uint32_t;
            ZE_CORE_TRACE("Allocated render resource {:p}", (void*)mResource);
            *mHandleCount = 1;
        }

        RenderHandle(const RenderHandle &inOther)
        {
            mResource = inOther.mResource;
            mHandleCount = inOther.mHandleCount;
            ++(*mHandleCount);
        }

        RenderHandle(RenderHandle &&inOther)
        {
            mResource = inOther.mResource;
            mHandleCount = inOther.mHandleCount;
            inOther.mResource = nullptr;
            inOther.mHandleCount = nullptr;
        }

        RenderHandle<T> &operator =(const RenderHandle &inOther)
        {
            Release();
            mResource = inOther.mResource;
            mHandleCount = inOther.mHandleCount;
            ++(*mHandleCount);
            return *this;
        }

        RenderHandle<T> &operator =(RenderHandle &&inOther)
        {
            mResource = inOther.mResource;
            mHandleCount = inOther.mHandleCount;
            inOther.mResource = nullptr;
            inOther.mHandleCount = nullptr;
            return *this;
        }

        ~RenderHandle()
        {
            Release();
        }


        void Release()
        {
            if (mHandleCount != nullptr)
            {
                --(*mHandleCount);
                if (*mHandleCount == 0)
                {
                    delete mHandleCount;
                    // this is supposed to be called only on the main thread and thus has to submit a command to free the resource
                    Renderer::Get().Submit<ReleaseResource>(mResource);
                }
            }
        }

        RenderResourceContainer<T> *GetResourceContainer() const { return mResource; }

    private:
        RenderResourceContainer<T> *mResource;
        uint32_t *mHandleCount;
    };

    class DrawIndexed : public RenderCommand
    {
    public:
        DrawIndexed(const RenderHandle<class VertexArray> &inVAHandle, uint32_t inCount = 0)
            : mVAContainer(inVAHandle.GetResourceContainer()), mCount(inCount)
        {}

        virtual std::string ToString() const override
        {
            std::stringstream ss;
			ss << "DrawIndexed(" << mVAContainer << ")";
			return ss.str();
        }

        virtual void Execute(const std::unique_ptr<RendererAPI> &inAPI) const override;
    private:
        RenderResourceContainer<class VertexArray> *mVAContainer;
        uint32_t mCount;
    };

}