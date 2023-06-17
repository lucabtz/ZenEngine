#include "ZenEngine.h"

using namespace ZenEngine;

class SandboxLayer : public Layer
{
public:
    SandboxLayer() : Layer("SandboxLayer") {}

    virtual void OnAttach() override
    {
        mShader = Shader::Create("resources/Shaders/Magenta.hlsl");

        BufferLayout layout{
            { ShaderDataType::Float3, "Position" }
        };
        float vertices[9] = { 
            -0.5f, -0.5f, 0.0f,
            0.5f, -0.5f, 0.0f,
            0.0f,  0.5f, 0.0f
        };
        auto vb = VertexBuffer::Create(vertices, 9 * sizeof(float));
        vb->SetLayout(layout);

        uint32_t indices[3] = {0, 1, 2};
        auto ib = IndexBuffer::Create(indices, 3);

        mVertexArray = VertexArray::Create();
        mVertexArray->AddVertexBuffer(vb);
        mVertexArray->SetIndexBuffer(ib);
    }

    virtual void OnRender(float inDeltaTime) override
    {
        mShader->Bind();
        Renderer::Submit(mVertexArray);
    }

private:
    std::shared_ptr<Shader> mShader;
    std::shared_ptr<VertexArray> mVertexArray;
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