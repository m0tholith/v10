#include "v10/cubemap.h"

#include "v10/lib/glad/glad.h"
#include "v10/lib/list.h"
#include "v10/lib/stb/stb_image.h"
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

uint64_t cubemapPathHash(char *str);

struct cubemapCacheEntry {
    uint32_t key;
    Cubemap *value;
};
typedef LIST(struct cubemapCacheEntry) cubemapCache;

cubemapCache cubemapCacheCreate();
void cubemapCacheAppend(cubemapCache cache, uint32_t key, Cubemap *value);
int cubemapCacheSearch(cubemapCache cache, uint32_t key);
void cubemapCacheFree(cubemapCache cache);

cubemapCache _cubemapCache;

Cubemap *cubemapCreate(char *_cubemapPath) {
    if (_cubemapCache == NULL)
        _cubemapCache = cubemapCacheCreate();

    const int cubemapPathLen = strlen(_cubemapPath);
    const int cubemapFileLen = strlen(CUBEMAPS_PATH) + cubemapPathLen;

    char *hashSource = malloc(cubemapPathLen + 1);
    strcpy(hashSource, _cubemapPath);
    char *cubemapFile = malloc(cubemapFileLen + 1);
    strcpy(cubemapFile, CUBEMAPS_PATH);
    strcat(cubemapFile, _cubemapPath);
    printf("Loading cubemap %s (%s)\n", _cubemapPath, cubemapFile);
    uint64_t hashSourceHash = cubemapPathHash(hashSource);
    int cubemapCacheIndex = cubemapCacheSearch(_cubemapCache, hashSourceHash);
    if (cubemapCacheIndex != -1) {
        free(cubemapFile);
        free(hashSource);
        return LIST_IDX(_cubemapCache, cubemapCacheIndex).value;
    }

    Cubemap *cubemap = malloc(sizeof(Cubemap));
    cubemap->Path = _cubemapPath;

    const int FilePathLen =
        cubemapFileLen +
        3; // "${cubemapFile}/nx" (what would be the result of strlen)
    char *filePath = malloc(FilePathLen + 1);
    memset(filePath, 0, FilePathLen + 1);
    strcpy(filePath, cubemapFile);

    glGenTextures(1, &cubemap->ID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap->ID);

    int width, height, nrChannels;
    bool isTransparent;
    for (int i = 0; i < 6; i++) {
        SWITCHFILE(filePath, cubemapFileLen, i);
        printf("\tLoading cubemap file '%s'\n", filePath);
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
                    _cubemapPath, filePath);
            stbi_image_free(data);
            free(hashSource);
            free(cubemapFile);
            free(filePath);
            return NULL;
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    cubemapCacheAppend(_cubemapCache, hashSourceHash, cubemap);
    free(hashSource);
    free(cubemapFile);
    free(filePath);

    printf("Loaded cubemap %s\n", _cubemapPath);

    return cubemap;
};
void cubemapFree(Cubemap *cubemap) {
    glDeleteTextures(1, &cubemap->ID);
    free(cubemap);
}
void cubemapFreeCache() { cubemapCacheFree(_cubemapCache); }

cubemapCache cubemapCacheCreate() {
    cubemapCache cache;
    LIST_INIT(cache, 4);
    return cache;
}
void cubemapCacheAppend(cubemapCache cache, uint32_t key, Cubemap *value) {
    LIST_APPEND(cache, ((struct cubemapCacheEntry){.key = key & 0xFFFFFFFF,
                                                   .value = value}));
}
int cubemapCacheSearch(cubemapCache cache, uint32_t key) {
    for (int i = 0; i < cache->size; i++) {
        if (key == LIST_IDX(cache, i).key)
            return i;
    }
    return -1;
}
void cubemapCacheFree(cubemapCache cache) {
    for (int i = 0; i < cache->size; i++) {
        cubemapFree(LIST_IDX(cache, i).value);
    }
    LIST_FREE(cache);
}

uint64_t cubemapPathHash(char *str) {
    uint64_t hash = 5381;
    int c;

    while ((c = (uint8_t)*str++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}
