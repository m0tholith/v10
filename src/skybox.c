#include "skybox.h"

#include "glad/glad.h"
#include "rendering.h"
#include "shader.h"
#include <stdlib.h>

Skybox *skyboxCreate(Cubemap *cubemap) {
    float vertices[] = {
        // positions
        -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f,
        1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f,

        -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f,
        -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,

        1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f,

        -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,

        -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f,

        -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f,
        1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f};

    Skybox *skybox = malloc(sizeof(Skybox));
    skybox->Cubemap = cubemap;

    glGenVertexArrays(1, &skybox->VAO);
    glBindVertexArray(skybox->VAO);

    glGenBuffers(1, &skybox->VBO);
    glBindBuffer(GL_ARRAY_BUFFER, skybox->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                          (void *)0);
    glEnableVertexAttribArray(0);

    skybox->Shader = shaderCreate("skybox.vert", "", "skybox.frag");

    return skybox;
}
void skyboxRender(Skybox *skybox) {
    glDepthFunc(GL_LEQUAL);

    glUseProgram(skybox->Shader->ID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->Cubemap->ID);

    glBindVertexArray(skybox->VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);

    glDepthFunc(GL_LESS);
}
void skyboxFree(Skybox *skybox) {
    glDeleteBuffers(2, &skybox->VAO);
    free(skybox);
}
