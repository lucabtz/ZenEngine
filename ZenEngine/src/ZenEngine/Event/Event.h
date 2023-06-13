#pragma once

#include "ZenEngine/Core/Macros.h"

namespace ZenEngine
{
    enum class EventType
	{
		None = 0,
		WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
		GameTick, GameUpdate, GameRender,
		KeyPressed, KeyReleased, KeyTyped,
		MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
	};

	enum EventCategory
	{
		None = 0,
		EventCategoryGame           = BIT(0),
		EventCategoryInput          = BIT(1),
		EventCategoryKeyboard       = BIT(2),
		EventCategoryMouse          = BIT(3),
		EventCategoryMouseButton    = BIT(4),
        EventCategoryWindow         = BIT(5)
	};

#define IMPLEMENT_EVENT_CLASS(eventtype, category) static EventType GetType() { return EventType::eventtype; } \
    virtual EventType GetEventType() const override { return GetType(); } \
    virtual int GetCategoryFlags() const override { return category; } \
    virtual const char *GetName() const override { return #eventtype; }

    class Event
	{
	public:
		virtual ~Event() = default;

		bool Handled = false;

		virtual EventType GetEventType() const = 0;
		virtual const char* GetName() const = 0;
		virtual int GetCategoryFlags() const = 0;
		virtual std::string ToString() const { return GetName(); }

		bool IsInCategory(EventCategory category)
		{
			return GetCategoryFlags() & category;
		}
	};

    class EventHandler
    {
    public:
        EventHandler(const std::unique_ptr<Event> &inEvent) : mEvent(inEvent) {}

        template<typename T>
        bool Handle(std::function<void(T*)> inCallback)
        {
            if (mEvent->GetEventType() == T::GetType())
            {
                inCallback(static_cast<T*>(mEvent.get()));
                return true;
            }
            return false;
        }

    private:
        const std::unique_ptr<Event> &mEvent;
    };
}