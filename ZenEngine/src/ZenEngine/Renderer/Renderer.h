#pragma once

#include <queue>
#include <memory>
#include <string>
#include <sstream>
#include <functional>
#include <vector>

#include "RendererAPI.h"
#include "RenderContext.h"
#include "UniformBuffer.h"
#include "Framebuffer.h"
#include "VertexArray.h"
#include "Material.h"

#include "ZenEngine/Core/Log.h"
#include "ZenEngine/Core/Window.h"
#include "ZenEngine/Editor/EditorGUI.h"


namespace ZenEngine
{
    class Renderer
    {
    public:
        struct AmbientLightInfo
        {
            glm::vec3 AmbientLightColor;
            float AmbientLightIntensity;
        };

        struct DirectionalLightInfo
        {
            glm::vec3 DirectionalLightColor;
            glm::vec3 DirectionalLightDirection;
            float DirectionalLightIntensity;
        };
        
        struct ShaderGlobals
        {
            glm::mat4 ViewProjectionMatrix;
            glm::mat4 InverseViewMatrix;
            glm::mat4 InverseProjectionMatrix;
            glm::mat4 ModelMatrix;
            glm::vec3 EyePosition;
            float FarPlane;
            float NearPlane;

            UB_STRUCT_PADDING(3);

            glm::vec3 AmbientLightColor;
            float AmbientLightIntensity;

            glm::vec3 DirectionalLightColor;
            float DirectionalLightIntensity;
            glm::vec3 DirectionalLightDirection;
        };
        UB_STRUCT_MAT4(ShaderGlobals, ViewProjectionMatrix);
        UB_STRUCT_MAT4(ShaderGlobals, InverseViewMatrix);
        UB_STRUCT_MAT4(ShaderGlobals, InverseProjectionMatrix);
        UB_STRUCT_MAT4(ShaderGlobals, ModelMatrix);
        UB_STRUCT_VEC3(ShaderGlobals, EyePosition);
        UB_STRUCT_FLOAT(ShaderGlobals, FarPlane);
        UB_STRUCT_FLOAT(ShaderGlobals, NearPlane);
        UB_STRUCT_VEC3(ShaderGlobals, AmbientLightColor);
        UB_STRUCT_FLOAT(ShaderGlobals, AmbientLightIntensity);
        UB_STRUCT_VEC3(ShaderGlobals, DirectionalLightColor);
        UB_STRUCT_FLOAT(ShaderGlobals, DirectionalLightIntensity);
        UB_STRUCT_VEC3(ShaderGlobals, DirectionalLightDirection);

        struct CameraView
        {
            bool IsPerspective = true;
            glm::vec3 EyePosition;
            glm::mat4 ProjectionMatrix;
            glm::mat4 ViewMatrix;
            float FarPlane;
            float NearPlane;
        };

        
        struct LightInfo
        {
            AmbientLightInfo Ambient;
            DirectionalLightInfo Directional;
        };

        struct GeometryInfo
        {
            std::shared_ptr<VertexArray> VertexArray;
            std::shared_ptr<Material> Mat;
            glm::mat4 Transform;
        };

        enum class BufferType : uint32_t
        {
            None = 0,
            FinalScene = 1,
            BaseColor,
            Normal,
            Specular,
            Depth,
            WorldPosition
        };

        static Renderer &Get()
        {
            static Renderer instance;
            return instance;
        }

        void Init(const std::unique_ptr<Window> &inWindow);
        void Shutdown();

        void BeginScene(const CameraView &inCameraView, const LightInfo &inLightInfo);
        void Flush(std::shared_ptr<Framebuffer> inTargetFramebuffer = nullptr, BufferType inBufferType = BufferType::FinalScene);
        void Submit(const std::shared_ptr<VertexArray> &inVertexArray, const glm::mat4 &inTransform,  const std::shared_ptr<Material> &inMaterial);

        void SetViewport(uint32_t inX, uint32_t inY, uint32_t inWidth, uint32_t inHeight);

        void SwapBuffers() { Get().mRenderContext->SwapBuffers(); }

        const std::unique_ptr<RendererAPI> &GetRendererAPI() const { return mRendererAPI; }


        void RecompileLightingModelShader();
    private:
        std::unique_ptr<RendererAPI> mRendererAPI;
        std::unique_ptr<RenderContext> mRenderContext;
        std::shared_ptr<UniformBuffer> mShaderGlobalsBuffer;
        ShaderGlobals mShaderGlobals;

        std::shared_ptr<Framebuffer> mGBuffer;
        std::shared_ptr<Shader> mLightingModelShader;
        std::shared_ptr<Shader> mBlitRGBShader;
        std::shared_ptr<Shader> mBlitDepth;
        std::shared_ptr<Shader> mBlitAlphaShader;
        std::shared_ptr<Shader> mBlitWorldPositionShader;
        std::shared_ptr<VertexArray> mFullScreenQuad;

        std::unique_ptr<EditorGUI> mEditorGUI;

        std::queue<GeometryInfo> mGeometryQueue;

        Renderer() = default;
        Renderer(const Renderer &) = delete;
        Renderer &operator =(const Renderer &) = delete;
    };

    namespace RenderCommand
    {
        void Clear(uint32_t inFlags = RendererAPI::ColorBuffer | RendererAPI::DepthBuffer | RendererAPI::StencilBuffer);        
        void SetClearColor(const glm::vec4 &inColor);
    };

}    