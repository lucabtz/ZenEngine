#pragma once

#include "Event.h"

#include "ZenEngine/Core/KeyCode.h"

namespace ZenEngine
{
    class KeyEvent : public Event
	{
	public:
		KeyCode GetKeyCode() const { return mKeyCode; }
	protected:
		KeyEvent(KeyCode inKeycode)
			: mKeyCode(inKeycode) {}

		KeyCode mKeyCode;
	};

    class KeyPressedEvent : public KeyEvent
	{
	public:
        IMPLEMENT_EVENT_CLASS(KeyPressed, EventCategoryInput | EventCategoryKeyboard)

		KeyPressedEvent(KeyCode inKeycode, bool inIsRepeat = false)
			: KeyEvent(inKeycode), mIsRepeat(inIsRepeat) {}

		bool IsRepeat() const { return mIsRepeat; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyPressedEvent: (" << mKeyCode << " repeat = " << (mIsRepeat? "true" : "false") << ")";
			return ss.str();
		}

	private:
		bool mIsRepeat;
	};

    class KeyReleasedEvent : public KeyEvent
	{
	public:
        IMPLEMENT_EVENT_CLASS(KeyReleased, EventCategoryInput | EventCategoryKeyboard)

		KeyReleasedEvent(const KeyCode inKeycode)
			: KeyEvent(inKeycode) {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyReleasedEvent: (" << mKeyCode << ")";
			return ss.str();
		}
	};

	class KeyTypedEvent : public KeyEvent
	{
	public:
        IMPLEMENT_EVENT_CLASS(KeyTyped, EventCategoryInput | EventCategoryKeyboard)

		KeyTypedEvent(const KeyCode inKeycode)
			: KeyEvent(inKeycode) {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyTypedEvent: (" << mKeyCode << ")";
			return ss.str();
		}
	};
}