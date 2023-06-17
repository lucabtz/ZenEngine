#pragma once

#include "Editor.h"

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

        virtual void OnBeginRenderGame() override;
        virtual void OnEndRenderGame() override;
    private:
        std::shared_ptr<Framebuffer> mViewportFramebuffer;
        glm::vec2 mViewportDimensions;
        bool mViewportFocused;
        bool mViewportHovered;
    };
}