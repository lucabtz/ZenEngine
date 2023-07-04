#include "Renderer.h"

#include "Shader.h"
#include "Material.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

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

        Framebuffer::Properties props;
        props.Width = inWindow->GetWidth();
        props.Height = inWindow->GetHeight();
        props.AttachmentProps = { 
            Framebuffer::TextureFormat::RGBA8,   // base color buffer and specular
            Framebuffer::TextureFormat::RGBA8,   // normal buffer
            Framebuffer::TextureFormat::RGBA8,   // at the moment contains shininess in the red channel. in the future when will use PBR
                                                 // will contain roughness, metallic, AO
            Framebuffer::TextureFormat::Depth24Stencil8 // depth stencil buffer
        };
        mGBuffer = Framebuffer::Create(props);

        auto vbo = VertexBuffer::Create({
            1.0f, 1.0f,
            -1.0f, 1.0f,
            -1.0f, -1.0f,
            1.0f, -1.0f
        });
        auto ibo = IndexBuffer::Create({
            0, 1, 2,
            2, 3, 0
        });
        BufferLayout layout{
            { ShaderDataType::Float2, "Position", 0 }
        };
        vbo->SetLayout(layout);
        mFullScreenQuad = VertexArray::Create();
        mFullScreenQuad->AddVertexBuffer(vbo);
        mFullScreenQuad->SetIndexBuffer(ibo);

        RecompileLightingModelShader();
        mBlitRGBShader = Shader::Create("resources/Shaders/BlitRGB.hlsl");
        mBlitAlphaShader = Shader::Create("resources/Shaders/BlitAlpha.hlsl");
        mBlitDepth = Shader::Create("resources/Shaders/BlitDepth.hlsl");
        mBlitWorldPositionShader = Shader::Create("resources/Shaders/BlitWorldPosition.hlsl");
    }

    void Renderer::Shutdown()
    {
        mEditorGUI->Shutdown();
    }

    void Renderer::BeginScene(const CameraView &inCameraView, const LightInfo &inLightInfo)
    {
        mShaderGlobals.ViewProjectionMatrix = inCameraView.ProjectionMatrix * inCameraView.ViewMatrix;
        mShaderGlobals.InverseViewMatrix = glm::inverse(inCameraView.ViewMatrix);
        mShaderGlobals.InverseProjectionMatrix = glm::inverse(inCameraView.ProjectionMatrix);
        mShaderGlobals.EyePosition = inCameraView.EyePosition;
        mShaderGlobals.FarPlane = inCameraView.FarPlane;
        mShaderGlobals.NearPlane = inCameraView.NearPlane;
        mShaderGlobals.AmbientLightColor = inLightInfo.Ambient.AmbientLightColor;
        mShaderGlobals.AmbientLightIntensity = inLightInfo.Ambient.AmbientLightIntensity;
        mShaderGlobals.DirectionalLightColor = inLightInfo.Directional.DirectionalLightColor;
        mShaderGlobals.DirectionalLightIntensity = inLightInfo.Directional.DirectionalLightIntensity;
        mShaderGlobals.DirectionalLightDirection = inLightInfo.Directional.DirectionalLightDirection;
        mShaderGlobalsBuffer->SetData(&mShaderGlobals, sizeof(ShaderGlobals));
    }

    void Renderer::Flush(std::shared_ptr<Framebuffer> inTargetFramebuffer, BufferType inBufferType)
    {
        RenderCommand::SetClearColor({ 0.0f, 0.0f, 0.0f, 0.0f });
        // geometry pass
        mShaderGlobalsBuffer->Bind();
        mGBuffer->Bind();
        RenderCommand::Clear();
        mRendererAPI->EnableDepthTest();
        mRendererAPI->DisableBlend();
        mRendererAPI->SetDepthMask(true);
        while (!mGeometryQueue.empty())
        {
            auto &geometry = mGeometryQueue.front();
            mGeometryQueue.pop();
            mShaderGlobals.ModelMatrix = geometry.Transform;
            mShaderGlobalsBuffer->SetData(&mShaderGlobals.ModelMatrix, sizeof(glm::mat4), offsetof(ShaderGlobals, ModelMatrix));
            geometry.Mat->Bind();
            mRendererAPI->DrawIndexed(geometry.VertexArray);
        }

        mGBuffer->Unbind();

        if (inTargetFramebuffer != nullptr) 
            inTargetFramebuffer->Bind();
    
        mRendererAPI->DisableDepthTest();
        switch (inBufferType)
        {
        case BufferType::FinalScene:
            mLightingModelShader->Bind();
            mGBuffer->BindAllAttachments();
            mRendererAPI->DrawIndexed(mFullScreenQuad);
            break;
        case BufferType::BaseColor:
            mBlitRGBShader->Bind();
            mGBuffer->BindColorAttachmentTexture(0, 0);
            mRendererAPI->DrawIndexed(mFullScreenQuad);
            break;
        case BufferType::Normal:
            mBlitRGBShader->Bind();
            mGBuffer->BindColorAttachmentTexture(1, 0);
            mRendererAPI->DrawIndexed(mFullScreenQuad);
            break;
        case BufferType::Specular:
            mBlitAlphaShader->Bind();
            mGBuffer->BindColorAttachmentTexture(0, 0);
            mRendererAPI->DrawIndexed(mFullScreenQuad);
            break;
        case BufferType::Depth:
            mBlitDepth->Bind();
            mGBuffer->BindDepthAttachmentTexture(0);
            mRendererAPI->DrawIndexed(mFullScreenQuad);
            break;
        case BufferType::WorldPosition:
            mBlitWorldPositionShader->Bind();
            mGBuffer->BindDepthAttachmentTexture(0);
            mRendererAPI->DrawIndexed(mFullScreenQuad);
            break;
        }

        if (inTargetFramebuffer != nullptr) 
            inTargetFramebuffer->Unbind();
    }

    void Renderer::Submit(const std::shared_ptr<class VertexArray> &inVertexArray, const glm::mat4 &inTransform, const std::shared_ptr<Material> &inMaterial)
    {
        mGeometryQueue.push({ inVertexArray, inMaterial, inTransform });
    }

    void Renderer::SetViewport(uint32_t inX, uint32_t inY, uint32_t inWidth, uint32_t inHeight)
    {
        mGBuffer->Resize(inWidth, inHeight);
        mRendererAPI->SetViewport(inX, inY, inWidth, inHeight);
    }

    void Renderer::RecompileLightingModelShader()
    {
        mLightingModelShader = Shader::Create("resources/Shaders/DeferredShading.hlsl");
    }


    void RenderCommand::Clear(uint32_t inFlags)
    {
        Renderer::Get().GetRendererAPI()->Clear(inFlags);
    }

    void RenderCommand::SetClearColor(const glm::vec4 &inColor)
    {
        Renderer::Get().GetRendererAPI()->SetClearColor(inColor);
    }
}
