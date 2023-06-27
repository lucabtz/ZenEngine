#include "EditorViewport.h"

#include <glm/gtc/type_ptr.hpp>

#include "EditorGUI.h"
#include "ZenEngine/Core/Log.h"
#include "ZenEngine/Core/Math.h"
#include "ZenEngine/Event/MouseEvents.h"
#include "ZenEngine/ECS/CoreComponents.h"

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
            mCamera.Resize(mViewportDimensions);
        }

        mViewportFocused = ImGui::IsWindowFocused();
        mViewportHovered = ImGui::IsWindowHovered();
        mCamera.SetShouldZoom(mViewportHovered && mViewportFocused);

        ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
        mViewportDimensions = { viewportPanelSize.x, viewportPanelSize.y };

        uint64_t textureID = mViewportFramebuffer->GetColorAttachmentRendererId();
        ImGui::Image(reinterpret_cast<void*>(textureID), ImVec2{ mViewportDimensions.x, mViewportDimensions.y }, { 0, 1 }, { 1, 0 });

        auto selectedEntity = Editor::Get().CurrentlySelectedEntity;
        if (selectedEntity != Entity::Null && selectedEntity.HasComponent<TransformComponent>())
        {
            ImGuizmo::SetOrthographic(false);
            ImGuizmo::SetDrawlist();
            ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, ImGui::GetWindowWidth(), ImGui::GetWindowHeight());
            
            auto &tc = selectedEntity.GetComponent<TransformComponent>();
            //glm::mat4 transform = selectedEntity.GetLocalTransform();
            glm::mat4 worldTransform = selectedEntity.GetWorldTransform();
            ImGuizmo::Manipulate(
                glm::value_ptr(mCamera.GetView()), glm::value_ptr(mCamera.GetProjection()), 
                mGizmoOperation, ImGuizmo::LOCAL, glm::value_ptr(worldTransform)
            );
            Math::DecomposeMatrix(glm::inverse(selectedEntity.GetParentTransform()) * worldTransform, tc.Position, tc.Rotation, tc.Scale);
        }
    }

    void EditorViewport::OnUpdate(float inDeltaTime)
    {
        mCamera.OnUpdate(inDeltaTime);

        if (ImGui::IsKeyDown(ImGuiKey_R))
            mGizmoOperation = ImGuizmo::ROTATE;
        
        if (ImGui::IsKeyDown(ImGuiKey_W))
            mGizmoOperation = ImGuizmo::TRANSLATE;

        if (ImGui::IsKeyDown(ImGuiKey_S))
            mGizmoOperation = ImGuizmo::SCALE;
    }

    void EditorViewport::OnEvent(const std::unique_ptr<Event> &inEvent)
    {
        EventHandler handler(inEvent);
        handler.Handle<MouseScrolledEvent>([this](auto event)
        {
            mCamera.MouseScrolled(event->GetYOffset());
        });
    }

    void EditorViewport::OnBeginRenderGame()
    {
        mViewportFramebuffer->Bind();
        Renderer::CameraView view{};
        view.ProjectionMatrix = mCamera.GetProjection();
        view.ViewMatrix = mCamera.GetView();
        Renderer::Get().BeginScene(view);
    }

    void EditorViewport::OnEndRenderGame()
    {
        mViewportFramebuffer->Unbind();
    }
}