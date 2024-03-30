#include "input.h"

#include <stdbool.h>

extern float blend;

void inputKeyCallback(GLFWwindow *window, int key, int scancode, int action,
                      int mods) {
    if (action == GLFW_PRESS) {
        switch (key) {
        case GLFW_KEY_ESCAPE:
            glfwSetWindowShouldClose(window, true);
            break;
        case GLFW_KEY_UP:
            blend += 0.1;
            break;
        case GLFW_KEY_DOWN:
            blend -= 0.1;
            break;
        default:
            break;
        }
    } else if (action == GLFW_REPEAT) {
        switch (key) {
        case GLFW_KEY_UP:
            blend += 0.1;
            break;
        case GLFW_KEY_DOWN:
            blend -= 0.1;
            break;
        }
    }
    if (blend < 0)
        blend = 0;
    else if (blend > 1)
        blend = 1;
}
