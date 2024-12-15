#include "v10/window.h"

#include <stdlib.h>

int WINDOW_WIDTH = 800;
int WINDOW_HEIGHT = 800;

void framebufferSizeCallback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}
void windowSizeCallback(GLFWwindow *window, int width, int height) {
    WINDOW_WIDTH = width;
    WINDOW_HEIGHT = height;
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
}
void glfwErrorCallback(int errorCode, const char *description) {
    printf("GLFW error code 0x%04X:\n\t%s\n", errorCode, description);
}

GLFWwindow *windowCreate() {
    glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_X11);
    glfwInit();
    glfwSetErrorCallback(glfwErrorCallback);
#ifdef ENABLE_ERRORCHECKING
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
#else
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
#endif
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    GLFWwindow *window =
        glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "template", NULL, NULL);
    if (window == NULL) {
        fprintf(stderr, "Failed to create GLFW window\n");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        fprintf(stderr, "Failed to initialize GLAD\n");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glEnable(GL_DEPTH_TEST);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetWindowSizeCallback(window, windowSizeCallback);
    glfwFocusWindow(window);

    return window;
}
void windowSetSkybox(float r, float g, float b) { glClearColor(r, g, b, 1.0f); }
void windowDraw(GLFWwindow *window) {
    glfwSwapBuffers(window);
    glfwPollEvents();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
void windowClose(GLFWwindow *window) {
    glfwDestroyWindow(window);
    glfwTerminate();
}
