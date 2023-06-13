#include "ZenEngine.h"

using namespace ZenEngine;

class SandboxLayer : public Layer
{
public:
    virtual void OnAttach() override
    {
        mShader = ShaderProxy::Create("resources/Shaders/Magenta.hlsl");

        BufferLayout layout{
            { ShaderDataType::Float3, "Position" }
        };
        auto vb = VertexBufferProxy::Create({ 
            -0.5f, -0.5f, 0.0f,
            0.5f, -0.5f, 0.0f,
            0.0f,  0.5f, 0.0f
        }, 9 * sizeof(float));
        vb.SetLayout(layout);

        auto ib = IndexBufferProxy::Create({
            0, 1, 2
        }, 3);

        mVA = VertexArrayProxy::Create();
        mVA.AddVertexBuffer(vb.GetHandle());
        mVA.SetIndexBuffer(ib.GetHandle());

        Renderer::Get().Submit<SetClearColor>(glm::vec4{ 0.0f, 0.0f, 0.0f, 1.0f });
    }

    virtual void OnUpdate(float inDeltaTime) override
    {
        Renderer::Get().Submit<Clear>();
        mShader.Bind();
        Renderer::Get().Submit<DrawIndexed>(mVA.GetHandle());
        Renderer::Get().Submit<SwapBuffers>();
    }

private:
    VertexArrayProxy mVA;
    ShaderProxy mShader;
};

class SandboxGame : public Game
{
public:
    IMPLEMENT_GAME_CLASS(SandboxGame, "Sandbox Game")

    virtual void OnInitialize() override
    {
        GetLayerStack().PushLayer(std::make_unique<SandboxLayer>());
    }
private:

};

SET_GAME_CLASS(SandboxGame);

#include "ZenEngine/Core/EntryPoint.h"