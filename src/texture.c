#include "texture.h"

#include "glad/glad.h"
#include "stb/stb_image.h"
#include <GLFW/glfw3.h>
#include <string.h>

unsigned int textureCreate(const char *textureFile, enum TEXTURETYPE type) {
    unsigned int texture;

    int width, height, numColorChannels;
    unsigned char *data;
    // load default texture (white)
    if (strcmp(textureFile, "") == 0)
        data = stbi_load("textures/default.jpg", &width, &height,
                         &numColorChannels, 0);
    else
        data = stbi_load(textureFile, &width, &height, &numColorChannels, 0);
    if (!data) {
        printf("failed to load texture \"%s\"", textureFile);
        stbi_image_free(data);
        exit(EXIT_FAILURE);
    }
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
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

    return texture;
}
