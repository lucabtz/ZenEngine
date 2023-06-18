#pragma once

#include <glm/glm.hpp>

#include "KeyCode.h"
#include "MouseCode.h"

namespace ZenEngine
{
    namespace Input
	{
		bool IsKeyPressed(KeyCode inKey);

		bool IsMouseButtonPressed(MouseCode inButton);
		glm::vec2 GetMousePosition();
		float GetMouseX();
		float GetMouseY();

		class InputImpl
		{
		public:
			virtual bool IsKeyPressed(KeyCode inKey) = 0;

			virtual bool IsMouseButtonPressed(MouseCode inButton) = 0;
			virtual glm::vec2 GetMousePosition() = 0;
		};
	};
}