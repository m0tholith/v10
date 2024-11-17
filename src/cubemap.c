#include "cubemap.h"

#include "glad/glad.h"
#include "stb/stb_image.h"
#include <GL/gl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SWITCHFILE(__filepath, __ilen, __idx)                                  \
    __filepath[__ilen + 0] = '/';                                              \
    __filepath[__ilen + 1] = __idx % 2 == 0 ? 'p' : 'n';                       \
    __filepath[__ilen + 2] =                                                   \
        (int)(__idx / 2) == 0 ? 'x' : ((int)(__idx / 2) == 1 ? 'y' : 'z');

Cubemap *cubemapCreate(char *folderPath) {
    Cubemap *cubemap = malloc(sizeof(Cubemap));
    cubemap->Path = folderPath;

    int folderPathLen = strlen(folderPath);

    const int FilePathLen =
        folderPathLen +
        3; // "${folderPathLen}/nx" (what would be the result of strlen)
    char *filePath = malloc(FilePathLen + 1);
    filePath[FilePathLen] = '\0'; // (FilePathLen - 1) + 1
    strcpy(filePath, folderPath);

    glGenTextures(1, &cubemap->ID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap->ID);

    int width, height, nrChannels;
    bool isTransparent;
    for (int i = 0; i < 6; i++) {
        SWITCHFILE(filePath, folderPathLen, i);
        unsigned char *data =
            stbi_load(filePath, &width, &height, &nrChannels, 0);
        isTransparent =
            nrChannels == 2 ||
            nrChannels == 4; // stb sets these values if an image has either:
                             // grey + alpha channel (hence 2)
                             // red + green + blue + alpha channel (hence 4)

        if (data) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
                         GL_RGB + isTransparent, width, height, 0,
                         GL_RGB + isTransparent, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        } else {
            fprintf(stderr, "Could not load cubemap \"%s\" (file \"%s\")\n",
                    folderPath, filePath);
            stbi_image_free(data);
        }
    }
    free(filePath);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return cubemap;
};
void cubemapFree(Cubemap *cubemap) {
    glDeleteTextures(1, &cubemap->ID);
    free(cubemap);
}
