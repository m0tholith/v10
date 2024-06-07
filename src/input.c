#include "input.h"

#include <cglm/struct.h>

extern vec3s movementInput;
extern vec2s mousePosition;
extern vec2s mouseDelta;
extern vec2s mouseSensitivity;

void inputInit(GLFWwindow *window) {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetKeyCallback(window, inputKeyCallback);
    mouseSensitivity = (vec2s){{0.25f, 0.25f}};
}
void inputKeyCallback(GLFWwindow *window, int key, int scancode, int action,
                      int mods) {
    if (action & (GLFW_PRESS | GLFW_REPEAT)) {
        switch (key) {
        case GLFW_KEY_ESCAPE:
            glfwSetWindowShouldClose(window, true);
            break;
        case GLFW_KEY_Q:
            movementInput.z = 1;
            break;
        case GLFW_KEY_E:
            movementInput.z = -1;
            break;
        case GLFW_KEY_W:
            movementInput.y = 1;
            break;
        case GLFW_KEY_S:
            movementInput.y = -1;
            break;
        case GLFW_KEY_A:
            movementInput.x = -1;
            break;
        case GLFW_KEY_D:
            movementInput.x = 1;
            break;
        default:
            break;
        }
    } else if (action == GLFW_RELEASE) {
        switch (key) {
        case GLFW_KEY_Q:
            movementInput.z = 0;
            break;
        case GLFW_KEY_E:
            movementInput.z = 0;
            break;
        case GLFW_KEY_W:
            movementInput.y = 0;
            break;
        case GLFW_KEY_S:
            movementInput.y = 0;
            break;
        case GLFW_KEY_A:
            movementInput.x = 0;
            break;
        case GLFW_KEY_D:
            movementInput.x = 0;
            break;
        default:
            break;
        }
    }
}
void inputMouseUpdate(GLFWwindow *window) {
    double xPosition, yPosition;
    glfwGetCursorPos(window, &xPosition, &yPosition);
    vec2s newPosition = (vec2s){{xPosition, yPosition}};
    mouseDelta = glms_vec2_sub(newPosition, mousePosition);
    mousePosition = newPosition;
}
