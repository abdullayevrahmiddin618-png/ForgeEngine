#include "Forge/Input/Input.h"
#include "Forge/Core/Application.h"

#include <GLFW/glfw3.h>

namespace Forge {

    bool Input::IsKeyPressed(int keycode) {
        auto* window = Application::Get().GetWindow().GetNativeWindow();
        int state = glfwGetKey(window, keycode);
        return state == GLFW_PRESS || state == GLFW_REPEAT;
    }

    bool Input::IsMouseButtonPressed(int button) {
        auto* window = Application::Get().GetWindow().GetNativeWindow();
        int state = glfwGetMouseButton(window, button);
        return state == GLFW_PRESS;
    }

    std::pair<float, float> Input::GetMousePosition() {
        auto* window = Application::Get().GetWindow().GetNativeWindow();
        double xpos = 0.0, ypos = 0.0;
        glfwGetCursorPos(window, &xpos, &ypos);
        return { static_cast<float>(xpos), static_cast<float>(ypos) };
    }

    float Input::GetMouseX() {
        return GetMousePosition().first;
    }

    float Input::GetMouseY() {
        return GetMousePosition().second;
    }

}
