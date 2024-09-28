
#include "core_impl/window.h"
#include "core_impl/Setting.h"

namespace Core {
    std::unique_ptr<Window> g_window;

    Window::Window() {
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        window = glfwCreateWindow(g_setting.m_window_size[0], g_setting.m_window_size[1], g_setting.m_app_name.c_str(), nullptr, nullptr);
        glfwSetWindowPos(window, g_setting.m_window_pos[0], g_setting.m_window_pos[1]);
    }

    void Window::poll_events() const {
        glfwPollEvents();
    }

    void Window::close() const {
        glfwSetWindowShouldClose(window, true);
    }

    Window::~Window() {
        glfwDestroyWindow(window);
    }
}