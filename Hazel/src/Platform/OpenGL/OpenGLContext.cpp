#include "hzpch.h"
#include "OpenGLContext.h"


#include "glad/glad.h"
#include "GLFW/glfw3.h"

namespace Hazel
{
    OpenGLContext::OpenGLContext(GLFWwindow* windowHandle) : m_WindowHandle(windowHandle)
    {
        HZ_CORE_ASSERT(windowHandle, "Window Handle is null!");
    }

    void OpenGLContext::Init()
    {
        glfwMakeContextCurrent(m_WindowHandle);
        const int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
        HZ_CORE_ASSERT(status, "Failed to initialize Glad!");
        HZ_CORE_TRACE("OpenGL Info:");
        HZ_CORE_INFO("  Vendor: {0}", glGetString(GL_VENDOR));
        HZ_CORE_INFO("  Renderer: {0}", glGetString(GL_RENDERER));
        HZ_CORE_INFO("  Version: {0}", glGetString(GL_VERSION));
    }

    void OpenGLContext::SwapBuffers()
    {
        glfwSwapBuffers(m_WindowHandle);
    }
}