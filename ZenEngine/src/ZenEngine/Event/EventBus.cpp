#include "EventBus.h"

namespace ZenEngine
{
    void EventBus::PostEvent(std::unique_ptr<Event> inEvent)
    {
        mEventQueue.push(std::move(inEvent));
    }

    std::unique_ptr<Event> EventBus::PopEvent()
    {
        if (mEventQueue.empty()) return nullptr;
        std::unique_ptr<Event> event = std::move(mEventQueue.front());
        mEventQueue.pop();
        return event;
    }
}