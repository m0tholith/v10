#include "input.h"

#include <stdbool.h>

void inputKeyCallback(GLFWwindow *window, int key, int scancode, int action,
                      int mods) {
    if (action == GLFW_PRESS) {
        switch (key) {
        case GLFW_KEY_ESCAPE:
            glfwSetWindowShouldClose(window, true);
            break;
        default:
            break;
        }
    } else if (action == GLFW_REPEAT) {
        switch (key) {
        }
    }
}
