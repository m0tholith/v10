#include "v10/window.h"

#include <stdio.h>
#include <stdlib.h>

void framebufferSizeCallback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}
void glfwErrorCallback(int errorCode, const char *description) {
    printf("GLFW error code 0x%04X:\n\t%s\n", errorCode, description);
}

void windowManagerInit() {
    glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_X11);
    glfwInit();
    glfwSetErrorCallback(glfwErrorCallback);
}
Window *windowCreate(int width, int height, char *title) {
    Window *window = malloc(sizeof(Window));
    window->Width = width;
    window->Height = height;

#ifdef ENABLE_ERRORCHECKING
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
#else
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
#endif
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    window->glWin = glfwCreateWindow(width, height, title, NULL, NULL);
    if (window->glWin == NULL) {
        fprintf(stderr, "Failed to create GLFW window\n");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(window->glWin);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        fprintf(stderr, "Failed to initialize GLAD\n");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glViewport(0, 0, window->Width, window->Height);
    glEnable(GL_DEPTH_TEST);
    glfwSetFramebufferSizeCallback(window->glWin, framebufferSizeCallback);
    glfwFocusWindow(window->glWin);

    return window;
}
void windowSetSkybox(float r, float g, float b) { glClearColor(r, g, b, 1.0f); }
void windowPreDraw(Window *window) {
    glViewport(0, 0, window->Width, window->Height);
}
void windowDraw(Window *window) {
    glfwSwapBuffers(window->glWin);
    glfwPollEvents();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
void windowClose(Window *window) {
    glfwDestroyWindow(window->glWin);
    free(window);
}
void windowManagerDeInit() { glfwTerminate(); }
