#pragma once

#include "Event.h"
#include <queue>
#include <memory>

namespace ZenEngine
{
    class EventBus
    {
    public:
        static EventBus &Get()
        {
            static EventBus instance;
            return instance;
        }

        void PostEvent(std::unique_ptr<Event> inEvent);

        std::unique_ptr<Event> PopEvent();

        bool HasEvents() const { return !mEventQueue.empty(); }
    private:
        std::queue<std::unique_ptr<Event>> mEventQueue;
    
        EventBus() : mEventQueue() {}
    };
}