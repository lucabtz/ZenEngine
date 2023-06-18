#include "ZenEngine/Core/Input.h"

namespace ZenEngine
{
    class GLFWInput : public Input::InputImpl
    {
    public:
        virtual bool IsKeyPressed(KeyCode inKey) override;

        virtual bool IsMouseButtonPressed(MouseCode inButton) override;
        virtual glm::vec2 GetMousePosition() override;
    };
}