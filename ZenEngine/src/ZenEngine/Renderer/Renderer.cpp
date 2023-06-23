#include "Renderer.h"

#include "Shader.h"
#include "Material.h"

namespace ZenEngine
{

    void Renderer::Init(const std::unique_ptr<Window> &inWindow)
    {
        mRendererAPI = RendererAPI::Create();
        mRenderContext = RenderContext::Create(inWindow->GetNativeWindow());
        mRenderContext->Init();
        mRendererAPI->Init();
        mEditorGUI = std::make_unique<EditorGUI>();
        mEditorGUI->Init();

        mShaderGlobalsBuffer = UniformBuffer::Create(sizeof(ShaderGlobals), 1);
    }

    void Renderer::Shutdown()
    {
        mEditorGUI->Shutdown();
    }

    void Renderer::BeginScene(const CameraView &inCameraView)
    {
        mShaderGlobals.ViewProjectionMatrix = inCameraView.ProjectionMatrix * inCameraView.ViewMatrix;
        mShaderGlobalsBuffer->SetData(&mShaderGlobals, sizeof(ShaderGlobals));
    }

    void Renderer::Flush()
    {
    }

    void Renderer::SubmitWithShader(const std::shared_ptr<class VertexArray> &inVertexArray, const glm::mat4 &inTransform, const std::shared_ptr<Shader> &inShader)
    {
        mShaderGlobals.ModelMatrix = inTransform;
        mShaderGlobalsBuffer->SetData(&mShaderGlobals.ModelMatrix, sizeof(glm::mat4), sizeof(glm::mat4));
        inShader->Bind();
        Get().mRendererAPI->DrawIndexed(inVertexArray);
    }

    void Renderer::Submit(const std::shared_ptr<class VertexArray> &inVertexArray, const glm::mat4 &inTransform, const std::shared_ptr<Material> &inMaterial)
    {
        mShaderGlobals.ModelMatrix = inTransform;
        mShaderGlobalsBuffer->SetData(&mShaderGlobals.ModelMatrix, sizeof(glm::mat4), sizeof(glm::mat4));
        inMaterial->Bind();
        Get().mRendererAPI->DrawIndexed(inVertexArray);
    }

    void RenderCommand::Clear()
    {
        Renderer::Get().GetRendererAPI()->Clear();
    }

    void RenderCommand::SetClearColor(const glm::vec4 &inColor)
    {
        Renderer::Get().GetRendererAPI()->SetClearColor(inColor);
    }
}
