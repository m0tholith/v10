#include "renderer.h"

#include "glad/glad.h"
#include "shader.h"
#include <GLFW/glfw3.h>
#include <stdio.h>

#define objectId unsigned int

objectId VAO; // vertex array object, stores info about vertex attributes
objectId VBO; // vertex buffer object, stores info about every vertex
objectId EBO; // element buffer object; used for storing order of vertices
              // to render
objectId shaderProgram;

int rendererInit() {
    // rectangle
    // float vertices[] = {
    //     -0.5f, -0.5f, 0.0f, // bottom left
    //     -0.5f, 0.5f,  0.0f, // top left
    //     0.5f,  0.5f,  0.0f, // top right
    //     0.5f,  -0.5f, 0.0f, // bottom right
    // };
    // objectId indices[] = {
    //     0, 1, 2, // triangle 1
    //     0, 2, 3, // triangle 2
    // };
    // joined at one vertex
    float vertices[] = {
        //    positions   //     colors     //
        -1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left
        -1.0f, 1.0f,  0.0f, 0.0f, 1.0f, 0.0f, // top left
        0.0f,  -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // middle
        1.0f,  1.0f,  0.0f, 0.0f, 1.0f, 0.0f, // top right
        1.0f,  -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom right
    };
    objectId indices[] = {
        0, 1, 2, // triangle 1
        2, 3, 4, // triangle 2
    };

    shaderProgram = shaderCreate("shaders/vertex_shader.vert",
                                 "shaders/fragment_shader.frag");

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices,
                 GL_STATIC_DRAW); // copy vertex data to VBO
    // vertex position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                          (void *)0);
    glEnableVertexAttribArray(0);
    // vertex color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                          (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
                 GL_STATIC_DRAW); // copy index data to EBO

    // glPolygonMode(GL_FRONT_AND_BACK,
    //               GL_LINE); // draw the front and back in wireframe
    glPolygonMode(GL_FRONT_AND_BACK,
                  GL_FILL); // draw the front and back of polygons (filled
                            // in)

    return 0;
}
void rendererDraw() {
    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
