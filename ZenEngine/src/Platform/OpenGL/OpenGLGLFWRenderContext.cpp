#include "OpenGLGLFWRenderContext.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "ZenEngine/Core/Macros.h"

namespace ZenEngine
{
    void OpenGLGLFWRenderContext::Init()
    {
        glfwMakeContextCurrent(mWindowHandle);
        int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
        ZE_ASSERT_CORE_MSG(status, "Failed to initialize Glad!");

        ZE_ASSERT_CORE_MSG(GLVersion.major > 4 || (GLVersion.major == 4 && GLVersion.minor >= 5), "ZenEngine requires at least OpenGL version 4.5!");
    }

    void OpenGLGLFWRenderContext::SwapBuffers()
    {
		    glfwSwapBuffers(mWindowHandle);
    }
}
