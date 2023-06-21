#pragma once

#include "Event.h"
#include "ZenEngine/Core/MouseCode.h"

namespace ZenEngine
{
    class MouseMovedEvent : public Event
    {
    public:
        IMPLEMENT_EVENT_CLASS(MouseMoved, EventCategoryInput | EventCategoryMouse)
        MouseMovedEvent(float inX, float inY)
            : mMouseX(inX), mMouseY(inY) {}

        float GetX() const { return mMouseX; }
        float GetY() const { return mMouseY; }

        std::string ToString() const override
        {
            std::stringstream ss;
            ss << "MouseMovedEvent: (" << mMouseX << ", " << mMouseY << ")";
            return ss.str();
        }
    private:
        float mMouseX, mMouseY;
    };

    class MouseScrolledEvent : public Event
    {
    public:
        IMPLEMENT_EVENT_CLASS(MouseScrolled, EventCategoryInput | EventCategoryMouse)

        MouseScrolledEvent(float inXOffset, float inYOffset)
            : mXOffset(inXOffset), mYOffset(inYOffset) {}

        float GetXOffset() const { return mXOffset; }
        float GetYOffset() const { return mYOffset; }

        std::string ToString() const override
        {
            std::stringstream ss;
            ss << "MouseScrolledEvent: " << mXOffset << ", " << mYOffset << ")";
            return ss.str();
        }
    private:
        float mXOffset, mYOffset;
    };

    class MouseButtonEvent : public Event
    {
    public:
        MouseCode GetMouseButton() const { return mButton; }
    protected:
        MouseButtonEvent(MouseCode inButton)
            : mButton(inButton) {}

        MouseCode mButton;
    };

    class MouseButtonPressedEvent : public MouseButtonEvent
    {
    public:
        IMPLEMENT_EVENT_CLASS(MouseButtonPressed, EventCategoryInput | EventCategoryMouse | EventCategoryMouseButton)

        MouseButtonPressedEvent(MouseCode inButton)
            : MouseButtonEvent(inButton) {}

        std::string ToString() const override
        {
            std::stringstream ss;
            ss << "MouseButtonPressedEvent: (" << mButton << ")";
            return ss.str();
        }
    };

    class MouseButtonReleasedEvent : public MouseButtonEvent
    {
    public:
        IMPLEMENT_EVENT_CLASS(MouseButtonReleased, EventCategoryInput | EventCategoryMouse | EventCategoryMouseButton)

        MouseButtonReleasedEvent(MouseCode inButton)
            : MouseButtonEvent(inButton) {}

        std::string ToString() const override
        {
            std::stringstream ss;
            ss << "MouseButtonReleasedEvent: (" << mButton << ")";
            return ss.str();
        }
    };
}