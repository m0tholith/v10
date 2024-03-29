#include "window.h"

#include "input.h"
#include "renderer.h"

#include <stdlib.h>

GLFWwindow *window;

int main(void) {
    window = windowCreate();

    // rectangle
    float vertices1[] = {
        //    positions   //     colors     //
        -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left
        -0.5f, 0.5f,  0.0f, 0.0f, 1.0f, 0.0f, // top left
        0.5f,  0.5f,  0.0f, 0.0f, 0.0f, 1.0f, // top right
        0.5f,  -0.5f, 0.0f, 1.0f, 1.0f, 0.0f, // bottom right
    };
    int indices1[] = {
        0, 1, 2, // triangle 1
        0, 2, 3, // triangle 2
    };
    BasicObject *object1 = basicObjectInit(
        vertices1, indices1, sizeof(vertices1) / sizeof(vertices1[0]),
        sizeof(indices1) / sizeof(indices1[0]));

    // joined at one vertex
    float vertices2[] = {
        //    positions   //     colors     //
        -1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left
        -1.0f, 1.0f,  0.0f, 0.0f, 1.0f, 0.0f, // top left
        0.0f,  -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // middle
        1.0f,  1.0f,  0.0f, 0.0f, 1.0f, 0.0f, // top right
        1.0f,  -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom right
    };
    int indices2[] = {
        0, 1, 2, // triangle 1
        2, 3, 4, // triangle 2
    };
    BasicObject *object2 = basicObjectInit(
        vertices2, indices2, sizeof(vertices2) / sizeof(vertices2[0]),
        sizeof(indices2) / sizeof(indices2[0]));

    while (!glfwWindowShouldClose(window)) {
        inputProcess(window);

        // HINT: drawing goes here
        // glClearColor(0.12f, 0.12f, 0.18f, 1.0f);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        basicObjectDraw(object1);
        basicObjectDraw(object2);

        windowDraw(window);
    }

    free(object1);
    free(object2);

    windowClose();
    return 0;
}
