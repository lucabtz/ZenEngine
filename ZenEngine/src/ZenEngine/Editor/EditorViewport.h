#pragma once

#include "Editor.h"
#include "EditorCamera.h"

#include "ZenEngine/Renderer/Framebuffer.h"

namespace ZenEngine
{

    class EditorViewport : public EditorWindow
    {
    public:
        EditorViewport() : EditorWindow("Viewport", true) {}

        virtual void OnRegister() override;
        virtual void OnInitializeStyle() override;
        virtual void OnClearStyle() override;
        virtual void OnRenderWindow() override;

        virtual void OnUpdate(float inDeltaTime) override;
        virtual void OnEvent(const std::unique_ptr<Event> &inEvent) override;

        virtual void OnBeginRenderGame() override;
        virtual void OnEndRenderGame() override;
    private:
        std::shared_ptr<Framebuffer> mViewportFramebuffer;
        EditorCamera mCamera;
        glm::vec2 mViewportDimensions;
        bool mViewportFocused;
        bool mViewportHovered;

        ImGuizmo::OPERATION mGizmoOperation = ImGuizmo::TRANSLATE;
    };
}