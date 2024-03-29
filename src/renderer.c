#include "renderer.h"

#include "glad/glad.h"
#include "shader.h"
#include "stb_image.h"
#include <GLFW/glfw3.h>
#include <string.h>
#include <stdlib.h>

BasicObject *basicObjectInit(float vertices[], int indices[], int vertexCount,
                             int indexCount, const char *textureFile) {
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
    glVertexAttribPointer(
        0, 3, GL_FLOAT, GL_FALSE,
        8 * sizeof(float), // 8 because we have 8 values per vertex:
                           // position, tint, texture sampling coordinate
        (void *)0);
    glEnableVertexAttribArray(0);
    // vertex color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glGenBuffers(1, &object->EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, object->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(int), indices,
                 GL_STATIC_DRAW); // copy index data to EBO

    // glPolygonMode(GL_FRONT_AND_BACK,
    //               GL_LINE); // draw the front and back in wireframe
    glPolygonMode(GL_FRONT_AND_BACK,
                  GL_FILL); // draw the front and back of polygons (filled
                            // in)

    if (strcmp(textureFile, "") != 0) {
        glGenTextures(1, &object->Texture);
        glBindTexture(GL_TEXTURE_2D, object->Texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                        GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        int width, height, numColorChannels;
        unsigned char *data =
            stbi_load(textureFile, &width, &height, &numColorChannels, 0);
        if (!data) {
            printf("failed to load texture \"%s\"", textureFile);
            exit(EXIT_FAILURE);
            stbi_image_free(data);
        }
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
                     GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(data);
    } else
        object->Texture = 0;

    return object;
}
void basicObjectDraw(BasicObject *object) {
    glUseProgram(object->ShaderProgram);
    if (object->Texture != 0)
        glBindTexture(GL_TEXTURE_2D, object->Texture);
    glBindVertexArray(object->VAO);
    glDrawElements(GL_TRIANGLES, object->IndexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
