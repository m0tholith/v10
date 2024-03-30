#include "window.h"

#include "input.h"
#include "renderer.h"

#include <stdlib.h>

GLFWwindow *window;

int main(void) {
    window = windowCreate();

    glfwSetKeyCallback(window, inputKeyCallback);

    // rectangle
    float vertices[] = {
        //    positions   //     colors     //tex coords//
        -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // bottom left
        -0.5f, 0.5f,  0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top left
        0.5f,  0.5f,  0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // top right
        0.5f,  -0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom right
    };
    int indices[] = {
        0, 1, 2, // triangle 1
        0, 2, 3, // triangle 2
    };
    BasicObject *object = basicObjectInit(
        vertices, indices, sizeof(vertices) / sizeof(vertices[0]),
        sizeof(indices) / sizeof(indices[0]), "textures/crate.jpg",
        "shaders/vertex_shader.vert", "shaders/fragment_shader.frag");

    while (!glfwWindowShouldClose(window)) {
        object->Transform = glms_rotate_z(
            glms_translate(glms_mat4_identity(),
                           (vec3s){cos(glfwGetTime()), 0.0f, 0.0f}),
            glfwGetTime());

        // HINT: drawing goes here
        // glClearColor(0.12f, 0.12f, 0.18f, 1.0f);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        basicObjectDraw(object);

        windowDraw(window);
    }

    free(object);

    windowClose();
    return 0;
}
