#pragma once

#include <GLFW/glfw3.h>

typedef void (*render_callback)();

class imgui_wrapper {

private:
    GLFWwindow* window;

    int init_glfw();

public:

    int run(const render_callback& callback);

};
