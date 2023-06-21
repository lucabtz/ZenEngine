#pragma once

#include "ZenEngine/Core/Window.h"

#include <memory>
#include <string>

#include <GLFW/glfw3.h>


namespace ZenEngine
{
    /**
     * A GLFW window
    */
    class GLFWWindow : public Window
    {
    public:
        GLFWWindow(const WindowInfo &inWindowInfo);
        virtual ~GLFWWindow();

        virtual void OnUpdate() override;

        virtual uint32_t GetWidth() const override { return mWindowData.Width; }
        virtual uint32_t GetHeight() const override { return mWindowData.Height; }

        // Window attributes
        virtual void SetVSync(bool inEnabled) override;
        virtual bool IsVSync() const override { return mWindowData.VSync; }

        // returns a pointer to the native window whatever that is
        virtual void* GetNativeWindow() const override { return mWindowHandle; }

    private:
        GLFWwindow *mWindowHandle;

        struct WindowData
        {
            std::string Title;
            uint32_t Width;
            uint32_t Height;

            bool VSync;
        };

        WindowData mWindowData;

        void Init();

        static int sGLFWWindowCount;
    };

}