#pragma once

#include <memory>
#include <string>

namespace ZenEngine
{

    enum class WindowPlatform
    {
        None = 0,
        GLFW
    };

    struct WindowInfo
    {
        std::string Title;
        uint32_t Width;
        uint32_t Height;
    };

    /**
     * Interface that represents a window
    */
    class Window
    {
    public:
		virtual ~Window() = default;

		virtual void OnUpdate() = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		// Window attributes
		virtual void SetVSync(bool enabled) = 0;
		virtual bool IsVSync() const = 0;

        // returns a pointer to the native window whatever that is
		virtual void* GetNativeWindow() const = 0;

		static std::unique_ptr<Window> Create(const WindowInfo &inWindowInfo);

        static WindowPlatform GetWindowPlatform();
    };

}