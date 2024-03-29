#include "window.h"

#include "input.h"
#include "renderer.h"

GLFWwindow *window;

int main(void) {
    // rectangle
    float vertices[] = {
        //    positions   //     colors     //
        -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left
        -0.5f, 0.5f,  0.0f, 0.0f, 1.0f, 0.0f, // top left
        0.5f,  0.5f,  0.0f, 0.0f, 0.0f, 1.0f, // top right
        0.5f,  -0.5f, 0.0f, 1.0f, 1.0f, 0.0f, // bottom right
    };
    int indices[] = {
        0, 1, 2, // triangle 1
        0, 2, 3, // triangle 2
    };
    // joined at one vertex
    // float vertices[] = {
    //     //    positions   //     colors     //
    //     -1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left
    //     -1.0f, 1.0f,  0.0f, 0.0f, 1.0f, 0.0f, // top left
    //     0.0f,  -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // middle
    //     1.0f,  1.0f,  0.0f, 0.0f, 1.0f, 0.0f, // top right
    //     1.0f,  -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom right
    // };
    // objectId indices[] = {
    //     0, 1, 2, // triangle 1
    //     2, 3, 4, // triangle 2
    // };

    window = windowCreate();
    rendererInit(vertices, indices, sizeof(vertices) / sizeof(vertices[0]),
                 sizeof(indices) / sizeof(indices[0]));

    while (!glfwWindowShouldClose(window)) {
        inputProcess(window);

        // HINT: drawing goes here
        // glClearColor(0.12f, 0.12f, 0.18f, 1.0f);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        rendererDraw();

        windowDraw(window);
    }

    windowClose();
    return 0;
}
