#include "Window.h"
#include "Macros.h"
#include "Platform.h"
#include "Platform/GLFW/GLFWWindow.h"

namespace ZenEngine
{
    std::unique_ptr<Window> Window::Create(const WindowInfo &inWindowInfo)
    {
#ifdef ZE_WINDOW_PLATFORM_GLFW
        return std::make_unique<GLFWWindow>(inWindowInfo);
#else
#error "Window platform not supported!"
#endif
        ZE_ASSERT_CORE_MSG(false, "Window platform not supported!");
    }
    WindowPlatform Window::GetWindowPlatform()
    {
#ifdef ZE_WINDOW_PLATFORM_GLFW
        return WindowPlatform::GLFW;
#endif
        return WindowPlatform::None;
    }
}