#include "texture.h"

#include "glad/glad.h"
#include "stb/stb_image.h"
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <string.h>

#define HASHMAP_SIZE 0xFFFF
unsigned int texHashmap[HASHMAP_SIZE] = {0};

unsigned int crc32b(unsigned char *message);
void texHashmapInsert(unsigned char *textureFile, unsigned int texture);
unsigned int texHashmapGet(unsigned char *textureFile, bool *success);

unsigned int textureCreate(const char *textureFile, enum TEXTURETYPE type) {
    unsigned int texture;

    bool success;
    texture = texHashmapGet(textureFile, &success);
    if (success) {
        return texture;
    }

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

    texHashmapInsert(textureFile, texture);
    return texture;
}

void texHashmapInsert(unsigned char *textureFile, unsigned int texture) {
    unsigned int hash = crc32b(textureFile);
    texHashmap[hash] = texture;
}
unsigned int texHashmapGet(unsigned char *textureFile, bool *success) {
    unsigned int hash = crc32b(textureFile);
    unsigned int entry = texHashmap[hash];
    // if the entry in hashmap is already set
    // opengl texture ids (afaik) start at 1
    if (texHashmap[hash] == 0) {
#define REDCODE "\033[0;31m"
#define ENDCODE "\033[0m"
        fprintf(stderr, REDCODE "no entry found for texture \"%s\"\n" ENDCODE,
                textureFile);
        *success = false;
        return 0;
    }
    *success = true;
    printf("entry found for texture \"%s\"\n", textureFile);
    return entry;
}

// https://stackoverflow.com/a/21001712/24750185
unsigned int crc32b(unsigned char *message) {
    int i, j;
    unsigned int byte, crc, mask;

    i = 0;
    crc = 0xFFFFFFFF;
    while (message[i] != 0) {
        byte = message[i]; // Get next byte.
        crc = crc ^ byte;
        for (j = 7; j >= 0; j--) { // Do eight times.
            mask = -(crc & 1);
            crc = (crc >> 1) ^ (0xEDB88320 & mask);
        }
        i = i + 1;
    }
    return (~crc) % 0xFFFF;
}
