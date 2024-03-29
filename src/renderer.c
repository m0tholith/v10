#include "renderer.h"

#include "glad/glad.h"
#include "shader.h"
#include <GLFW/glfw3.h>
#include <stdlib.h>

BasicObject *basicObjectInit(float vertices[], int indices[], int vertexCount,
                             int indexCount) {
    BasicObject *object = malloc(sizeof(BasicObject));
    object->VertexCount = vertexCount;
    object->IndexCount = indexCount;
    object->ShaderProgram = shaderCreate("shaders/vertex_shader.vert",
                                         "shaders/fragment_shader.frag");

    glGenVertexArrays(1, &object->VAO);
    glBindVertexArray(object->VAO);
    glGenBuffers(1, &object->VBO);
    glBindBuffer(GL_ARRAY_BUFFER, object->VBO);
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

    glGenBuffers(1, &object->EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, object->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(int), indices,
                 GL_STATIC_DRAW); // copy index data to EBO

    // glPolygonMode(GL_FRONT_AND_BACK,
    //               GL_LINE); // draw the front and back in wireframe
    glPolygonMode(GL_FRONT_AND_BACK,
                  GL_FILL); // draw the front and back of polygons (filled
                            // in)
    return object;
}
void basicObjectDraw(BasicObject *object) {
    glUseProgram(object->ShaderProgram);
    glBindVertexArray(object->VAO);
    glDrawElements(GL_TRIANGLES, object->IndexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
