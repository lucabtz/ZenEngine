#pragma once

#include "Event.h"
#include <stdint.h>

namespace ZenEngine
{

    class WindowCloseEvent : public Event
    {
    public:
        IMPLEMENT_EVENT_CLASS(WindowClose, EventCategoryWindow)
        WindowCloseEvent() = default;
    };

    class WindowResizeEvent : public Event
    {
    public:
        IMPLEMENT_EVENT_CLASS(WindowResize, EventCategoryWindow)

        WindowResizeEvent(uint32_t inWidth, uint32_t inHeight) : mWidth(inWidth), mHeight(inHeight) {}

        uint32_t GetWidth() const { return mWidth; }
        uint32_t GetHeight() const { return mHeight; }

        std::string ToString() const override
        {
            std::stringstream ss;
            ss << "WindowResizeEvent: (" << mWidth << ", " << mHeight << ")";
            return ss.str();
        }
    private:
        uint32_t mWidth;
        uint32_t mHeight;
    };
}