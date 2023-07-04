#pragma once

#include "Editor.h"
#include "EditorCamera.h"

#include "ZenEngine/Renderer/Renderer.h"
#include "ZenEngine/Renderer/Framebuffer.h"

namespace ZenEngine
{

    class EditorViewport : public EditorWindow
    {
    public:
        EditorViewport() : EditorWindow("Viewport", true) { ZE_ASSERT_CORE_MSG(sViewportInstance == nullptr, "An editor viewport already exists!"); sViewportInstance = this; }

        virtual void OnRegister() override;
        virtual void OnInitializeStyle() override;
        virtual void OnClearStyle() override;
        virtual void OnRenderWindow() override;

        void BeginScene();
        void FlushScene();

        virtual void OnUpdate(float inDeltaTime) override;
        virtual void OnEvent(const std::unique_ptr<Event> &inEvent) override;

        Renderer::CameraView GetCameraView() 
        {
            Renderer::CameraView view{};
            view.FarPlane = mCamera.GetFarPlane();
            view.NearPlane = mCamera.GetNearPlane();
            view.EyePosition = mCamera.GetPosition();
            view.ProjectionMatrix = mCamera.GetProjection();
            view.ViewMatrix = mCamera.GetView();
            return view;
        }

        const std::shared_ptr<Framebuffer> &GetFramebuffer() { return mViewportFramebuffer; }

        static EditorViewport &Get() { ZE_ASSERT_CORE_MSG(sViewportInstance != nullptr, "Viewport does not exist!"); return *sViewportInstance; }
    private:
        std::shared_ptr<Framebuffer> mViewportFramebuffer;
        EditorCamera mCamera;
        glm::vec2 mViewportDimensions;
        bool mViewportFocused;
        bool mViewportHovered;

        Renderer::BufferType mVisualizedBuffer = Renderer::BufferType::FinalScene;

        ImGuizmo::OPERATION mGizmoOperation = ImGuizmo::TRANSLATE;
    
        static EditorViewport *sViewportInstance;
    };
}