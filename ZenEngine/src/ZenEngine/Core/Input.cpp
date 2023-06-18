#include "Input.h"

#include <memory>
#include "Platform.h"

#include "Platform/GLFW/GLFWInput.h"

namespace ZenEngine
{
#ifdef ZE_WINDOW_PLATFORM_GLFW
    static std::unique_ptr<Input::InputImpl> sImplementation = std::make_unique<GLFWInput>();
#else
    #error "Unknown window platform!"
#endif
    bool Input::IsKeyPressed(KeyCode inKey)
    {
        return sImplementation->IsKeyPressed(inKey);
    }

    bool Input::IsMouseButtonPressed(MouseCode inButton)
    {
        return sImplementation->IsMouseButtonPressed(inButton);
    }
    
    glm::vec2 Input::GetMousePosition()
    {
        return sImplementation->GetMousePosition();
    }

    float Input::GetMouseX() { return GetMousePosition().x; }
    float Input::GetMouseY() { return GetMousePosition().y; }
}