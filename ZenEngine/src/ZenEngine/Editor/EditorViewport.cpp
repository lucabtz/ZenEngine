#include "EditorViewport.h"

#include "ZenEngine/Core/Log.h"

namespace ZenEngine
{
    void EditorViewport::OnRegister()
    {
        Framebuffer::Properties props;
        props.AttachmentProps = { Framebuffer::TextureFormat::RGBA8, Framebuffer::TextureFormat::Depth };
        props.Width = 1280;
		props.Height = 720;
        mViewportFramebuffer = Framebuffer::Create(props);
    }

    void EditorViewport::OnInitializeStyle()
    {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
    }
    
    void EditorViewport::OnClearStyle()
    {
        ImGui::PopStyleVar();
    }

    void EditorViewport::OnRenderWindow()
    {
        Framebuffer::Properties props = mViewportFramebuffer->GetProperties();
        if (mViewportDimensions.x > 0.0f && mViewportDimensions.y > 0.0f && // zero sized framebuffer is invalid
			(props.Width != mViewportDimensions.x || props.Height != mViewportDimensions.y))
		{
            ZE_CORE_TRACE("EditorViewport: FB resized to {} {}", mViewportDimensions.x, mViewportDimensions.y);
			mViewportFramebuffer->Resize((uint32_t)mViewportDimensions.x, (uint32_t)mViewportDimensions.y);
		}

		mViewportFocused = ImGui::IsWindowFocused();
		mViewportHovered = ImGui::IsWindowHovered();

		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		mViewportDimensions = { viewportPanelSize.x, viewportPanelSize.y };

		uint64_t textureID = mViewportFramebuffer->GetColorAttachmentRendererId();
		ImGui::Image(reinterpret_cast<void*>(textureID), ImVec2{ mViewportDimensions.x, mViewportDimensions.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
    }

    void EditorViewport::OnBeginRenderGame()
    {
        mViewportFramebuffer->Bind();
    }

    void EditorViewport::OnEndRenderGame()
    {
        mViewportFramebuffer->Unbind();
    }
}