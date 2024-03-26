#include "window.h"
#include "input.h"

GLFWwindow *window;

int main(void) {
    window = windowCreate();
    if (window == NULL) {
        return -1;
    }

    while (!glfwWindowShouldClose(window)) {
        inputProcess(window);
        windowDraw(window);
    }

    windowClose();
    return 0;
}
