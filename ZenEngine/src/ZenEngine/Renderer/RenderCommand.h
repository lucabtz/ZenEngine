#pragma once

#include <memory>
#include <string>
#include <sstream>
#include <functional>
#include <vector>

#include <glm/glm.hpp>

#include "RendererAPI.h"

namespace ZenEngine
{    
    class RenderCommand
    {
    public:
        virtual ~RenderCommand() = default; 
        virtual void Execute(const std::unique_ptr<RendererAPI> &inAPI) const = 0;
        virtual std::string ToString() const =  0;
    };

    class SetViewport : public RenderCommand
    {
    public:
        SetViewport(uint32_t inX, uint32_t inY, uint32_t inWidth, uint32_t inHeight)
            : mX(inX), mY(inY), mWidth(inWidth), mHeight(inHeight) 
        {}

        virtual std::string ToString() const override
        {
            std::stringstream ss;
			ss << "SetViewport(" << mX << ", " << mY << ", " << mWidth << ", " << mHeight << ")";
			return ss.str();
        }

        virtual void Execute(const std::unique_ptr<RendererAPI> &inAPI) const override;

    private:
        uint32_t mX, mY, mWidth, mHeight;
    };

    class SetClearColor : public RenderCommand
    {
    public:
        SetClearColor(const glm::vec4& inColor)
            : mColor(inColor)
        {}

        virtual std::string ToString() const override
        {
            std::stringstream ss;
			ss << "SetClearColor(" << mColor.r << ", " << mColor.g << ", " << mColor.b << ", " << mColor.a << ")";
			return ss.str();
        }

        virtual void Execute(const std::unique_ptr<RendererAPI> &inAPI) const override;
    private:
        glm::vec4 mColor;
    };

    class Clear : public RenderCommand
    {
    public:
        Clear() {}

        virtual std::string ToString() const override
        {
			return "Clear()";
        }
        
        virtual void Execute(const std::unique_ptr<RendererAPI> &inAPI) const override;
    };

    class ReleaseResource : public RenderCommand
    {
    public:
        ReleaseResource(struct RenderResourceContainerBase *inResource) : mResource(inResource) {}

        virtual std::string ToString() const override
        {
            std::stringstream ss;
			ss << "DeleteResource(" << mResource << ")";
			return ss.str();
        }

        virtual void Execute(const std::unique_ptr<RendererAPI> &inAPI) const override;
    private:
        struct RenderResourceContainerBase *mResource;
    };

    class SwapBuffers : public RenderCommand
    {
    public:
        SwapBuffers() {}

        virtual std::string ToString() const override
        {
            return "SwapBuffers()";
        }

        virtual void Execute(const std::unique_ptr<RendererAPI> &inAPI) const override;
    };

}

#include "Renderer.h"

#define PROXY(proxyclass) class proxyclass ## Proxy

#define SETUP_PROXY(proxyclass) using Handle = RenderHandle<proxyclass>;\
	    using Resource = RenderResourceContainer<proxyclass>;\
        using Proxy = proxyclass ## Proxy;\
    \
    public:\
    	proxyclass ## Proxy() : mHandle() {}\
    	proxyclass ## Proxy(Resource *inResource) : mHandle(inResource) {}\
        proxyclass ## Proxy(const proxyclass ## Proxy &inOther) : mHandle(inOther.mHandle) {}\
        proxyclass ## Proxy(proxyclass ## Proxy &&inOther) : mHandle(std::move(inOther.mHandle)) {}\
        proxyclass ## Proxy &operator =(const proxyclass ## Proxy &inOther) {  mHandle = inOther.mHandle; return *this; }\
        proxyclass ## Proxy &operator =(proxyclass ## Proxy &&inOther) {  mHandle = std::move(inOther.mHandle); return *this; }\
        Handle &GetHandle() { return mHandle; }\
    private:\
		Handle mHandle;

#define CREATE_METHOD_COMMAND_NO_PARAM(cmdname) class cmdname : public RenderCommand\
		{\
		public:\
			cmdname(const Handle &inHandle) : mResource(inHandle.GetResourceContainer()) {}\
\
            virtual std::string ToString() const override\
            {\
                std::stringstream ss;\
			    ss << #cmdname << "()";\
			    return ss.str();\
            }\
			virtual void Execute(const std::unique_ptr<RendererAPI> &inAPI) const override;\
		private:\
			Resource *mResource;\
		};

#define CREATE_METHOD_COMMAND_ONE_PARAM(cmdname, paramtype, paramname) class cmdname : public RenderCommand\
		{\
		public:\
			cmdname(const Handle &inHandle, paramtype & in ## paramname) : mResource(inHandle.GetResourceContainer()), paramname(in ## paramname) {}\
\
            virtual std::string ToString() const override\
            {\
                std::stringstream ss;\
			    ss << #cmdname << "(...)";\
			    return ss.str();\
            }\
			virtual void Execute(const std::unique_ptr<RendererAPI> &inAPI) const override;\
		private:\
			Resource *mResource;\
            paramtype paramname;\
		};

#define CREATE_METHOD_COMMAND_ONE_PARAM_NO_REF(cmdname, paramtype, paramname) class cmdname : public RenderCommand\
		{\
		public:\
			cmdname(const Handle &inHandle, paramtype in ## paramname) : mResource(inHandle.GetResourceContainer()), paramname(in ## paramname) {}\
\
            virtual std::string ToString() const override\
            {\
                std::stringstream ss;\
			    ss << #cmdname << "(...)";\
			    return ss.str();\
            }\
			virtual void Execute(const std::unique_ptr<RendererAPI> &inAPI) const override;\
		private:\
			Resource *mResource;\
            paramtype paramname;\
		};

#define CREATE_METHOD_COMMAND_TWO_PARAM(cmdname, paramtype1, paramname1, paramtype2, paramname2) class cmdname : public RenderCommand\
		{\
		public:\
			cmdname(const Handle &inHandle, paramtype1 & in ## paramname1, paramtype2 & in ## paramname2) \
                : mResource(inHandle.GetResourceContainer()), paramname1(in ## paramname1), paramname2(in ## paramname2) {}\
\
            virtual std::string ToString() const override\
            {\
                std::stringstream ss;\
			    ss << #cmdname << "(...)";\
			    return ss.str();\
            }\
			virtual void Execute(const std::unique_ptr<RendererAPI> &inAPI) const override;\
		private:\
			Resource *mResource;\
            paramtype1 paramname1;\
            paramtype2 paramname2;\
		};

#define IMPLEMENT_METHOD(cmdname, proxyclass) 	void proxyclass ## Proxy::cmdname::Execute(const std::unique_ptr<RendererAPI> &inAPI) const
