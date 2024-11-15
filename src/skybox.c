#include "skybox.h"

#include "glad/glad.h"
#include <stdio.h>
#include <stdlib.h>

uint32_t *SkyboxVAO = NULL, *SkyboxVBO = NULL;
Cubemap *SkyboxCubemap = NULL;
Shader *SkyboxShader = NULL;

void skyboxInit() {
    if (!(SkyboxVAO == NULL || SkyboxVBO == NULL)) {
        fprintf(stderr,
                "Error initializing skybox state: Already initialized (VAO: "
                "%p, VBO: %p)\n",
                SkyboxVAO, SkyboxVBO);
        return;
    }
    SkyboxVAO = malloc(sizeof(typeof(SkyboxVAO)));
    SkyboxVBO = malloc(sizeof(typeof(SkyboxVBO)));

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

    glGenVertexArrays(1, SkyboxVAO);
    glBindVertexArray(*SkyboxVAO);

    glGenBuffers(1, SkyboxVBO);
    glBindBuffer(GL_ARRAY_BUFFER, *SkyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                          (void *)0);
    glEnableVertexAttribArray(0);
}
void skyboxSet(Cubemap *cubemap, Shader *shader) {
    if (cubemap != NULL)
        SkyboxCubemap = cubemap;

    if (shader == NULL)
        shader = shaderCreate("skybox.vert", "", "skybox.frag");
    SkyboxShader = shader;
}
void skyboxRender() {
    if (SkyboxVAO == NULL || SkyboxVBO == NULL) {
        fprintf(stderr,
                "Error rendering skybox: Not initialized (VAO: %p, VBO: %p)\n",
                SkyboxVAO, SkyboxVBO);
        return;
    } else if (SkyboxCubemap == NULL) {
        fprintf(stderr, "Error rendering skybox: Cubemap not set\n");
        return;
    } else if (SkyboxShader == NULL) {
        fprintf(stderr, "Error rendering skybox: Shader not set\n");
        return;
    }

    glDepthFunc(GL_LEQUAL);

    glUseProgram(SkyboxShader->ID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, SkyboxCubemap->ID);

    glBindVertexArray(*SkyboxVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);

    glDepthFunc(GL_LESS);
}
void skyboxDestroy() {
    if (SkyboxVAO == NULL || SkyboxVBO == NULL) {
        fprintf(
            stderr,
            "Error deinitializing skybox: Not initialized (VAO: %p, VBO: %p)\n",
            SkyboxVAO, SkyboxVBO);
        return;
    }
    glDeleteBuffers(1, SkyboxVAO);
    glDeleteBuffers(1, SkyboxVBO);
    free(SkyboxVAO);
    free(SkyboxVBO);
    SkyboxCubemap = NULL;
    SkyboxShader = NULL;
}
