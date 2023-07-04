#pragma once

#include <glm/glm.hpp>

namespace ZenEngine
{
    class EditorCamera
    {
    public:
        EditorCamera() = default;
        EditorCamera(float inFOV, float inNearClip, float inFarClip);

        void OnUpdate(float inDeltaTime);

        void MouseScrolled(float inDelta);

        void Resize(const glm::vec2 &inViewportDimensions);
        void SetFocusPoint(const glm::vec3 &inFocusPoint);
        void SetDistance(float inDistance);
        void SetFocusPointAndDistance(const glm::vec3 &inFocusPoint, float inDistance);

        void SetShouldZoom(bool inShouldZoom) { mShouldZoom = inShouldZoom; }

        const glm::mat4 &GetProjection() { return mProjectionMatrix; }
        const glm::mat4 &GetView() { return mViewMatrix; }
        glm::vec3 GetPosition() const;

        float GetFarPlane() const { return mFarClip; }
        float GetNearPlane() const { return mNearClip; }

    private:
        float mFOV = glm::radians(45.0f);
        float mAspectRatio;
        float mNearClip = 0.1f; 
        float mFarClip = 1000.0f;

        glm::mat4 mViewMatrix;
        glm::mat4 mProjectionMatrix;
        glm::vec3 mPosition = { 0.0f, 0.0f, 0.0f };
        glm::vec3 mFocusPoint = { 0.0f, 0.0f, 0.0f };

        glm::vec2 mInitialMousePosition = { 0.0f, 0.0f };

        float mDistance = 10.0f;
        float mPitch = 0.0f;
        float mYaw = 0.0f;

        float mViewportWidth = 1280;
        float mViewportHeight = 720;

        bool mShouldZoom = false;

        void UpdateView();
        void UpdateProjection();

        glm::vec3 GetForwardDirection() const;
        glm::vec3 GetRightDirection() const;
        glm::vec3 GetUpDirection() const;
        glm::quat GetOrientation() const;

        void MousePan(const glm::vec2 &inDelta);
        void MouseRotate(const glm::vec2 &inDelta);
        void MouseZoom(float inDelta);

        std::pair<float, float> PanSpeed() const;
        float RotationSpeed() const;
        float ZoomSpeed() const;
        
    };
}