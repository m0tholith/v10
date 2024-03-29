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

void rendererInit(float vertices[], int indices[], int vertexCount,
                  int indexCount) {
    shaderProgram = shaderCreate("shaders/vertex_shader.vert",
                                 "shaders/fragment_shader.frag");

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(float), vertices,
                 GL_STATIC_DRAW); // copy vertex data to VBO
    // vertex position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, indexCount * sizeof(float),
                          (void *)0);
    glEnableVertexAttribArray(0);
    // vertex color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, indexCount * sizeof(float),
                          (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(int), indices,
                 GL_STATIC_DRAW); // copy index data to EBO

    // glPolygonMode(GL_FRONT_AND_BACK,
    //               GL_LINE); // draw the front and back in wireframe
    glPolygonMode(GL_FRONT_AND_BACK,
                  GL_FILL); // draw the front and back of polygons (filled
                            // in)
}
void rendererDraw() {
    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
