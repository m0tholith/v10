#include "cubemap.h"

#include "glad/glad.h"
#include "stb/stb_image.h"
#include <GL/gl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SWITCHFILE(__filepath, __ilen, __idx)                                  \
    __filepath[__ilen + 0] = '/';                                              \
    __filepath[__ilen + 1] = __idx % 2 == 0 ? 'p' : 'n';                       \
    __filepath[__ilen + 2] =                                                   \
        (int)(__idx / 2) == 0 ? 'x' : ((int)(__idx / 2) == 1 ? 'y' : 'z');     \
    __filepath[__ilen + 3] = '.';
#define ISTRANSPARENT(__ext) (strncmp(__ext, "png", 3) == 0 ? 1 : 0)

Cubemap *cubemapCreate(char *folderPath, char *extension) {
    Cubemap *cubemap = malloc(sizeof(Cubemap));
    cubemap->Path = folderPath;

    int folderPathLen = strlen(folderPath);
    int extensionLen = strlen(extension);

    char *filePath = malloc(folderPathLen + 4 + extensionLen + 1);
    filePath[folderPathLen + 4 + extensionLen + 1] = '\0';
    strcpy(filePath, folderPath);
    strcpy(filePath + folderPathLen + 4, extension);

    glGenTextures(1, &cubemap->ID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap->ID);

    int width, height, nrChannels;
    int isTransparent = ISTRANSPARENT(extension);
    for (int i = 0; i < 6; i++) {
        SWITCHFILE(filePath, folderPathLen, i);
        for (int j = 0; j < folderPathLen + 4 + extensionLen; j++) {
            printf("%c", filePath[j]);
        }
        printf("\n");
        unsigned char *data =
            stbi_load(filePath, &width, &height, &nrChannels, 0);
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
