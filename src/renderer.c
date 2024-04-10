#include "renderer.h"

#include "glad/glad.h"
#include "shader.h"
#include "stb_image.h"
#include "texture.h"
#include <GLFW/glfw3.h>
#include <stdlib.h>

#ifdef AWESOMEFACE
objectId awesomeface = 0;
#endif

BasicObject *basicObjectInit(float vertices[], int indices[], int vertexCount,
                             int indexCount, const char *textureFile,
                             const char *vertexShaderFile,
                             const char *fragmentShaderFile) {
    BasicObject *object = malloc(sizeof(BasicObject));
    object->VertexCount = vertexCount;
    object->IndexCount = indexCount;
    object->ShaderProgram = shaderCreate(vertexShaderFile, fragmentShaderFile);

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

    // texture loading
    stbi_set_flip_vertically_on_load(true);

    glActiveTexture(GL_TEXTURE0);
    object->Texture = textureCreate(textureFile, false);

#ifdef AWESOMEFACE
    if (awesomeface == 0) {
        glActiveTexture(GL_TEXTURE1);
        awesomeface = textureCreate("textures/awesomeface.png", true);

        glUseProgram(object->ShaderProgram);
        glUniform1i(glGetUniformLocation(object->ShaderProgram, "texSampler"),
                    0);
        glUniform1i(glGetUniformLocation(object->ShaderProgram,
                                         "awesomefaceTexSampler"),
                    1);
    }
#endif

    object->Transform = glms_mat4_identity();

    return object;
}
void basicObjectDraw(BasicObject *object) {
    glUseProgram(object->ShaderProgram);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, object->Texture);
#ifdef AWESOMEFACE
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, awesomeface);
#endif
    glUniformMatrix4fv(glGetUniformLocation(object->ShaderProgram, "transform"),
                       1, GL_FALSE, (float *)object->Transform.raw);
    glBindVertexArray(object->VAO);
    glDrawElements(GL_TRIANGLES, object->IndexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
