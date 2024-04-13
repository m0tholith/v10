#include "input.h"

#include "cglm/include/cglm/types-struct.h"
#include <stdbool.h>

extern vec3s wsadqe;

void inputKeyCallback(GLFWwindow *window, int key, int scancode, int action,
                      int mods) {
    if (action == GLFW_PRESS) {
        switch (key) {
        case GLFW_KEY_ESCAPE:
            glfwSetWindowShouldClose(window, true);
            break;
        case GLFW_KEY_Q:
            wsadqe.z = 1;
            break;
        case GLFW_KEY_E:
            wsadqe.z = -1;
            break;
        case GLFW_KEY_W:
            wsadqe.y = 1;
            break;
        case GLFW_KEY_S:
            wsadqe.y = -1;
            break;
        case GLFW_KEY_A:
            wsadqe.x = -1;
            break;
        case GLFW_KEY_D:
            wsadqe.x = 1;
            break;
        default:
            break;
        }
    } else if (action == GLFW_RELEASE) {
        switch (key) {
        case GLFW_KEY_Q:
            wsadqe.z = 0;
            break;
        case GLFW_KEY_E:
            wsadqe.z = 0;
            break;
        case GLFW_KEY_W:
            wsadqe.y = 0;
            break;
        case GLFW_KEY_S:
            wsadqe.y = 0;
            break;
        case GLFW_KEY_A:
            wsadqe.x = 0;
            break;
        case GLFW_KEY_D:
            wsadqe.x = 0;
            break;
        default:
            break;
        }
    }
}
