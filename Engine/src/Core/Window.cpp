#include "Forge/Core/Window.h"
#include "Forge/Core/Log.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Forge {

    static bool s_GLFWInitialized = false;

    Window::Window(const WindowProps& props) {
        Init(props);
    }

    Window::~Window() {
        Shutdown();
    }

    void Window::Init(const WindowProps& props) {
        m_Data = props;

        if (!s_GLFWInitialized) {
            if (!glfwInit()) {
                FE_ERROR("GLFW ishga tushmadi!");
                return;
            }
            s_GLFWInitialized = true;
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        m_Window = glfwCreateWindow(
            static_cast<int>(m_Data.Width),
            static_cast<int>(m_Data.Height),
            m_Data.Title.c_str(),
            nullptr,
            nullptr
        );

        if (!m_Window) {
            FE_ERROR("Oyna yaratib bo'lmadi!");
            glfwTerminate();
            s_GLFWInitialized = false;
            return;
        }

        glfwMakeContextCurrent(m_Window);

        if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
            FE_ERROR("GLAD yuklanmadi!");
            glfwDestroyWindow(m_Window);
            m_Window = nullptr;
            return;
        }

        glfwSwapInterval(1); // VSync yoqilgan

        glViewport(0, 0, static_cast<int>(m_Data.Width), static_cast<int>(m_Data.Height));
        glfwSetFramebufferSizeCallback(m_Window, [](GLFWwindow*, int width, int height) {
            glViewport(0, 0, width, height);
        });

        FE_INFO("Oyna yaratildi: " + m_Data.Title);
        FE_INFO(std::string("OpenGL GPU: ") + reinterpret_cast<const char*>(glGetString(GL_RENDERER)));
        FE_INFO(std::string("OpenGL Versiya: ") + reinterpret_cast<const char*>(glGetString(GL_VERSION)));
    }

    void Window::Shutdown() {
        if (m_Window) {
            glfwDestroyWindow(m_Window);
            m_Window = nullptr;
        }
        if (s_GLFWInitialized) {
            glfwTerminate();
            s_GLFWInitialized = false;
        }
    }

    void Window::OnUpdate() {
        glfwPollEvents();
        glfwSwapBuffers(m_Window);
    }

    bool Window::ShouldClose() const {
        return !m_Window || glfwWindowShouldClose(m_Window);
    }

}
