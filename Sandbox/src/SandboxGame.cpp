#include "ZenEngine.h"

using namespace ZenEngine;

class SandboxLayer : public Layer
{
public:
    SandboxLayer() : Layer("SandboxLayer") {}

    virtual void OnAttach() override
    {
        mShader = Shader::Create("resources/Shaders/FlatColor.hlsl");

        BufferLayout layout{
            { ShaderDataType::Float3, "Position" }
        };
        float vertices[9] = { 
            0.0f, 0.0f, 0.0f,
            -10.0f, 10.0f, 0.0f,
            -10.0f, -10.0f, 0.0f,
        };
        auto vb = VertexBuffer::Create(vertices, 9 * sizeof(float));
        vb->SetLayout(layout);

        uint32_t indices[6] = {0, 1, 2};
        auto ib = IndexBuffer::Create(indices, 6);

        mVertexArray = VertexArray::Create();
        mVertexArray->AddVertexBuffer(vb);
        mVertexArray->SetIndexBuffer(ib);

        mShader->SetFloat4("Color", { 0.0f, 1.0f, 0.0f, 1.0f });
    }

    virtual void OnRender(float inDeltaTime) override
    {
        Renderer::Get().Submit(mVertexArray, glm::mat4(1.0f), mShader);
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