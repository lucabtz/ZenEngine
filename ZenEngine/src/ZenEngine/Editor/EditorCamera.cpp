#include "EditorCamera.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "ZenEngine/Core/Input.h"

#include "ZenEngine/Core/Log.h"
#include "ZenEngine/ECS/CoreComponents.h"
#include "Editor.h"

namespace ZenEngine
{
    EditorCamera::EditorCamera(float inFOV, float inNearClip, float inFarClip)
        : mFOV(inFOV), mNearClip(inNearClip), mFarClip(inFarClip)
    {
        UpdateView();
        UpdateProjection();
    }

    void EditorCamera::OnUpdate(float inDeltaTime)
    {
        if (ImGui::IsKeyDown(ImGuiKey_LeftShift))
        {
            const glm::vec2& mouse{ Input::GetMouseX(), Input::GetMouseY() };
            glm::vec2 delta = (mouse - mInitialMousePosition) * 0.003f;
            mInitialMousePosition = mouse;

            if (ImGui::IsMouseDown(ImGuiMouseButton_Middle))
                MousePan(delta);
            else if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
                MouseRotate(delta);
            else if (ImGui::IsMouseDown(ImGuiMouseButton_Right))
                MouseZoom(delta.y);
            
            
            UpdateView();
        }

        if (ImGui::IsKeyDown(ImGuiKey_F))
        {
            auto selected = Editor::Get().CurrentlySelectedEntity;
            if (selected != Entity::Null && selected.HasComponent<TransformComponent>())
                SetFocusPointAndDistance(selected.GetComponent<TransformComponent>().Translation, 10.0f);
            else
                SetFocusPointAndDistance({ 0.0f, 0.0f, 0.0f }, 10.0f);
        }
    }

    void EditorCamera::MouseScrolled(float inDelta)
    {
        if (!mShouldZoom) return;
        float delta = inDelta * 0.1f;
        MouseZoom(delta);
        UpdateView();
    }

    void EditorCamera::Resize(const glm::vec2 &inViewportDimensions)
    {
        mViewportWidth = inViewportDimensions.x;
        mViewportHeight = inViewportDimensions.y;
        UpdateProjection();
    }

    void EditorCamera::SetFocusPoint(const glm::vec3 &inFocusPoint)
    {
        mFocusPoint = inFocusPoint;
        UpdateView();
    }

    void EditorCamera::SetDistance(float inDistance)
    {
        mDistance = inDistance;
        UpdateView();
    }

    void EditorCamera::SetFocusPointAndDistance(const glm::vec3 &inFocusPoint, float inDistance)
    {
        mFocusPoint = inFocusPoint;
        mDistance = inDistance;
        UpdateView();
    }

    void EditorCamera::UpdateView()
    {
        mPosition = CalculatePosition();

        glm::quat orientation = GetOrientation();
        mViewMatrix = glm::translate(glm::mat4(1.0f), mPosition) * glm::toMat4(orientation);
        mViewMatrix = glm::inverse(mViewMatrix);
    }

    void EditorCamera::UpdateProjection()
    {
        mAspectRatio = mViewportWidth / mViewportHeight;
        mProjectionMatrix = glm::perspective(mFOV, mAspectRatio, mNearClip, mFarClip);
    }

    glm::vec3 EditorCamera::CalculatePosition() const
    {
        return mFocusPoint - mDistance * GetForwardDirection();
    }

    glm::vec3 EditorCamera::GetForwardDirection() const
    {
        return glm::rotate(GetOrientation(), glm::vec3(0.0f, 0.0f, -1.0f));
    }

    glm::vec3 EditorCamera::GetRightDirection() const
    {
        return glm::rotate(GetOrientation(), glm::vec3(1.0f, 0.0f, 0.0f));
    }

    glm::vec3 EditorCamera::GetUpDirection() const
    {
        return glm::rotate(GetOrientation(), glm::vec3(0.0f, 1.0f, 0.0f));
    }

    glm::quat EditorCamera::GetOrientation() const
    {
        return glm::quat(glm::vec3(-mPitch, 0.0f, -mYaw));
    }


    std::pair<float, float> EditorCamera::PanSpeed() const
    {
        float x = std::min(mViewportWidth / 1000.0f, 2.4f); // max = 2.4f
        float xFactor = 0.0366f * (x * x) - 0.1778f * x + 0.3021f;

        float y = std::min(mViewportHeight / 1000.0f, 2.4f); // max = 2.4f
        float yFactor = 0.0366f * (y * y) - 0.1778f * y + 0.3021f;

        return { xFactor, yFactor };
    }

    float EditorCamera::RotationSpeed() const
    {
        return 0.8f;
    }


    float EditorCamera::ZoomSpeed() const
    {
        float distance = mDistance * 0.2f;
        distance = std::max(distance, 0.0f);
        float speed = distance * distance;
        speed = std::min(speed, 100.0f); // max speed = 100
        return speed;
    }

    void EditorCamera::MousePan(const glm::vec2 &inDelta)
    {
        auto [xSpeed, ySpeed] = PanSpeed();
        mFocusPoint += -GetRightDirection() * inDelta.x * xSpeed * mDistance;
        mFocusPoint += GetUpDirection() * inDelta.y * ySpeed * mDistance;
    }

    void EditorCamera::MouseRotate(const glm::vec2 &inDelta)
    {
        float yawSign = GetUpDirection().z < 0 ? -1.0f : 1.0f;
        mYaw += yawSign * inDelta.x * RotationSpeed();
        mPitch -= inDelta.y * RotationSpeed();
    }

    void EditorCamera::MouseZoom(float inDelta)
    {
        mDistance -= inDelta * ZoomSpeed();
        if (mDistance < 1.0f)
        {
            mFocusPoint += GetForwardDirection();
            mDistance = 1.0f;
        }
    }
}