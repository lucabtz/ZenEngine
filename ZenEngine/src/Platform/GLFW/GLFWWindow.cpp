#include "GLFWWindow.h"
#include "ZenEngine/Core/Log.h"
#include "ZenEngine/Event/EventBus.h"
#include "ZenEngine/Event/WindowEvents.h"
#include "ZenEngine/Event/KeyEvents.h"
#include "ZenEngine/Event/MouseEvents.h"

namespace ZenEngine
{

    static void GLFWErrorCallback(int error, const char* description)
    {
        ZE_CORE_ERROR("GLFW Error ({}): {}", error, description);
    }

    int GLFWWindow::sGLFWWindowCount = 0;

    GLFWWindow::GLFWWindow(const WindowInfo &inWindowInfo)
        : mWindowData{}
    {
        mWindowData.Title = inWindowInfo.Title;
        mWindowData.Width = inWindowInfo.Width;
        mWindowData.Height = inWindowInfo.Height;
        Init();
    }

    void GLFWWindow::OnUpdate()
    {
        glfwPollEvents();
    }

    void GLFWWindow::SetVSync(bool inEnabled)
    {
        if (inEnabled)
            glfwSwapInterval(1);
        else
            glfwSwapInterval(0);
        mWindowData.VSync = inEnabled;
    }

    GLFWWindow::~GLFWWindow()
    {
        glfwDestroyWindow(mWindowHandle);
        sGLFWWindowCount--;
        if (sGLFWWindowCount == 0)
        {
            glfwTerminate();
        }
    }

    void GLFWWindow::Init()
    {
        ZE_CORE_INFO("Creating window {} ({}, {})", mWindowData.Title, mWindowData.Width, mWindowData.Height);

        if (sGLFWWindowCount == 0)
        {
            if (glfwInit() == GLFW_TRUE) 
            {
                glfwSetErrorCallback(GLFWErrorCallback);
            }
            else
            {
                ZE_CORE_DIE("Could not initialize GLFW");
            }
        }

        mWindowHandle = glfwCreateWindow(mWindowData.Width, mWindowData.Height, mWindowData.Title.c_str(), nullptr, nullptr);
        glfwSetWindowUserPointer(mWindowHandle, &mWindowData);
        sGLFWWindowCount++;

        //SetVSync(true);

        glfwSetWindowSizeCallback(mWindowHandle, [](GLFWwindow* window, int width, int height)
        {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
            data.Width = width;
            data.Height = height;
        
            EventBus::Get().PostEvent(std::make_unique<WindowResizeEvent>(width, height));
        });

        glfwSetWindowCloseCallback(mWindowHandle, [](GLFWwindow* window)
        {			
            EventBus::Get().PostEvent(std::make_unique<WindowCloseEvent>());
        });

        glfwSetKeyCallback(mWindowHandle, [](GLFWwindow* window, int key, int scancode, int action, int mods)
        {
            switch (action)
            {
            case GLFW_PRESS:
                EventBus::Get().PostEvent(std::make_unique<KeyPressedEvent>(key));
                break;
            case GLFW_RELEASE: 
                EventBus::Get().PostEvent(std::make_unique<KeyReleasedEvent>(key));
                break;
            case GLFW_REPEAT: 
                EventBus::Get().PostEvent(std::make_unique<KeyPressedEvent>(key, true));
                break;
            }
        });

        glfwSetCharCallback(mWindowHandle, [](GLFWwindow* window, unsigned int keycode)
        {
            EventBus::Get().PostEvent(std::make_unique<KeyTypedEvent>(keycode));
        });

        glfwSetMouseButtonCallback(mWindowHandle, [](GLFWwindow* window, int button, int action, int mods)
        {
            switch (action)
            {
            case GLFW_PRESS: 
                EventBus::Get().PostEvent(std::make_unique<MouseButtonPressedEvent>(button));
                break;
            case GLFW_RELEASE: 
                EventBus::Get().PostEvent(std::make_unique<MouseButtonReleasedEvent>(button));
                break;
            }
        });

        glfwSetScrollCallback(mWindowHandle, [](GLFWwindow* window, double xOffset, double yOffset)
        {
            EventBus::Get().PostEvent(std::make_unique<MouseScrolledEvent>(xOffset, yOffset));
        });

        glfwSetCursorPosCallback(mWindowHandle, [](GLFWwindow* window, double xPos, double yPos)
        {
            EventBus::Get().PostEvent(std::make_unique<MouseMovedEvent>(xPos, yPos));
        });

    }

}