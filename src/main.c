#include "window.h"

#include "input.h"
#include "renderer.h"

GLFWwindow *window;

int main(void) {
    window = windowCreate();
    if (window == NULL) {
        return -1;
    }
    int rendererInitCode = rendererInit();
    if (rendererInitCode) {
        return rendererInitCode;
    }

    while (!glfwWindowShouldClose(window)) {
        inputProcess(window);

        // HINT: drawing goes here
        glClearColor(0.12f, 0.12f, 0.18f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        rendererDraw();

        windowDraw(window);
    }

    windowClose();
    return 0;
}
