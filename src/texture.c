#include "texture.h"

#include "glad/glad.h"
#include "stb/stb_image.h"
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Texture *textureCreate(const char *_texturePath, enum TEXTURETYPE type,
                       bool optional) {
    char *textureFile = malloc(strlen(_texturePath) + sizeof(TEXTURES_PATH));
    strcpy(textureFile, TEXTURES_PATH);
    strcat(textureFile, _texturePath);

    unsigned int textureId;

    bool success;

    int width, height, numColorChannels;
    unsigned char *data;
    // load default texture (white)
    if (strcmp(textureFile, "") == 0)
        data = stbi_load("textures/default.jpg", &width, &height,
                         &numColorChannels, 0);
    else
        data = stbi_load(textureFile, &width, &height, &numColorChannels, 0);
    if (!data) {
        printf("failed to load texture \"%s\"\n", textureFile);
        stbi_image_free(data);
        if (optional)
            return NULL;
        else
            exit(EXIT_FAILURE);
    }
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
                 GL_RGB + (type & 1), GL_UNSIGNED_BYTE,
                 data); // HINT: type is either 0 or 1; GL_RGBA is 1+GL_RGB
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);

    free(textureFile);

    Texture *texture = malloc(sizeof(Texture));
    texture->id = textureId;
    return texture;
}
void textureFree(Texture *texture) {
    glDeleteTextures(1, &texture->id);
    free(texture);
}
