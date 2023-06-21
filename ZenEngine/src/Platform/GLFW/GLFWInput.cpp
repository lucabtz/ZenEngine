#include "GLFWInput.h"

#include <glfw/glfw3.h>

#include "ZenEngine/Core/Game.h"

namespace ZenEngine
{
    bool GLFWInput::IsKeyPressed(KeyCode inKey)
    {
        auto *window = reinterpret_cast<GLFWwindow*>(Game::Get().GetWindow()->GetNativeWindow());
        auto state = glfwGetKey(window, static_cast<int32_t>(inKey));
        return state == GLFW_PRESS;
    }
    
    bool GLFWInput::IsMouseButtonPressed(MouseCode inButton)
    {
        auto *window = reinterpret_cast<GLFWwindow*>(Game::Get().GetWindow()->GetNativeWindow());
        auto state = glfwGetMouseButton(window, static_cast<int32_t>(inButton));
        return state == GLFW_PRESS;
    }

    glm::vec2 GLFWInput::GetMousePosition()
    {
        auto *window = reinterpret_cast<GLFWwindow*>(Game::Get().GetWindow()->GetNativeWindow());
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        return { (float)xpos, (float)ypos };
    }
}
