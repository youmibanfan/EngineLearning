#pragma once
#include "Common.h"
#include <GLFW/glfw3.h>
#include <memory>
//#include "unique_ptr"

namespace Core {

    class Window {
        no_copy_move_construction(Window)
    public:
        Window();
        GLFWwindow *get_glfw_window() const { return window; }
        bool is_alive() const { return !glfwWindowShouldClose(window); }
        void poll_events() const;
        void close() const;
        ~Window();
    INNER_VISIBLE:
        GLFWwindow *window;
    };

    extern std::unique_ptr<Window> g_window;
}