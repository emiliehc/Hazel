#include "hzpch.h"
#include "WindowsInput.h"

#include <GLFW/glfw3.h>

#include "Hazel/Core/Application.h"

namespace Hazel
{
    Input* Input::s_Instance = new WindowsInput;

    bool WindowsInput::IsKeyPressedImpl(int keycode)
    {
        const auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
        const auto state = glfwGetKey(window, keycode);
        return state == GLFW_PRESS || state == GLFW_REPEAT;
    }

    bool WindowsInput::IsMouseButtonPressedImpl(int button)
    {
        const auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
        const auto state = glfwGetMouseButton(window, button);
        return state == GLFW_PRESS;
    }

    float WindowsInput::GetMouseXImpl()
    {
        auto [x, y] = GetMousePositionImpl();
        return x;
    }

    float WindowsInput::GetMouseYImpl()
    {
        auto [x, y] = GetMousePositionImpl();
        return y;
    }

    std::pair<float, float> WindowsInput::GetMousePositionImpl()
    {
        const auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
        double x, y;
        glfwGetCursorPos(window, &x, &y);
        return {x, y};
    }
}
