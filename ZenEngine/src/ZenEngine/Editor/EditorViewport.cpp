#include "EditorViewport.h"

#include <glm/gtc/type_ptr.hpp>

#include "EditorGUI.h"
#include "ZenEngine/Core/Log.h"
#include "ZenEngine/Core/Math.h"
#include "ZenEngine/Event/MouseEvents.h"
#include "ZenEngine/ECS/CoreComponents.h"

namespace ZenEngine
{
    EditorViewport *EditorViewport::sViewportInstance;

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
            Renderer::Get().SetViewport(0.0f, 0.0f, (uint32_t)mViewportDimensions.x, (uint32_t)mViewportDimensions.y);
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
            glm::mat4 worldTransform = selectedEntity.GetWorldTransform();
            ImGuizmo::Manipulate(
                glm::value_ptr(mCamera.GetView()), glm::value_ptr(mCamera.GetProjection()), 
                mGizmoOperation, ImGuizmo::LOCAL, glm::value_ptr(worldTransform)
            );
            if (ImGuizmo::IsUsing())
            {
                glm::vec3 newPosition, newScale;
                glm::quat newRotation;

                if (Math::DecomposeMatrix(glm::inverse(selectedEntity.GetParentTransform()) * worldTransform, newPosition, newRotation, newScale))
                {
                    tc.Position = newPosition;
                    tc.Rotation = newRotation;
                    tc.Scale = newScale;
                }
            }
        }
    }

    void EditorViewport::BeginScene()
    {
        auto activeScene = Editor::Get().GetActiveScene();
        if (activeScene == nullptr)
            Renderer::Get().BeginScene(GetCameraView(), {});
        else
            Renderer::Get().BeginScene(GetCameraView(), activeScene->GetLights());
    }

    void EditorViewport::FlushScene()
    {
        Renderer::Get().Flush(mViewportFramebuffer, mVisualizedBuffer);
    }

    void EditorViewport::OnUpdate(float inDeltaTime)
    {
        mCamera.OnUpdate(inDeltaTime);

        bool ctrl = ImGui::IsKeyDown(ImGuiKey_LeftCtrl) || ImGui::IsKeyDown(ImGuiKey_RightCtrl);

        if (ImGui::IsKeyDown(ImGuiKey_R))
            mGizmoOperation = ImGuizmo::ROTATE;
        
        if (ImGui::IsKeyDown(ImGuiKey_W))
            mGizmoOperation = ImGuizmo::TRANSLATE;

        if (ImGui::IsKeyDown(ImGuiKey_S))
            mGizmoOperation = ImGuizmo::SCALE;

        if (ctrl)
        {
            if (ImGui::IsKeyDown(ImGuiKey_1))
                mVisualizedBuffer = Renderer::BufferType::FinalScene;

            if (ImGui::IsKeyDown(ImGuiKey_2))
                mVisualizedBuffer = Renderer::BufferType::BaseColor;

            if (ImGui::IsKeyDown(ImGuiKey_3))
                mVisualizedBuffer = Renderer::BufferType::Normal;

            if (ImGui::IsKeyDown(ImGuiKey_4))
                mVisualizedBuffer = Renderer::BufferType::Specular;

            if (ImGui::IsKeyDown(ImGuiKey_5))
                mVisualizedBuffer = Renderer::BufferType::Depth;

            if (ImGui::IsKeyDown(ImGuiKey_6))
                mVisualizedBuffer = Renderer::BufferType::WorldPosition;
        }
    }

    void EditorViewport::OnEvent(const std::unique_ptr<Event> &inEvent)
    {
        EventHandler handler(inEvent);
        handler.Handle<MouseScrolledEvent>([this](auto event)
        {
            mCamera.MouseScrolled(event->GetYOffset());
        });
    }
}