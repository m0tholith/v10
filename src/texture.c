#include "texture.h"

#include "glad/glad.h"
#include "lib/list.h"
#include "stb/stb_image.h"
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *readTextureFile(const char *fileName);
uint64_t texturePathHash(char *str);

struct textureCacheEntry {
    uint32_t key;
    Texture *value;
};
typedef LIST(struct textureCacheEntry) textureCache;

textureCache textureCacheCreate();
void textureCacheAppend(textureCache cache, uint32_t key, Texture *value);
int textureCacheSearch(textureCache cache, uint32_t key);
void textureCacheFree(textureCache cache);

textureCache _textureCache;

Texture *textureCreate(const char *_texturePath, bool optional) {
    if (_textureCache == NULL)
        _textureCache = textureCacheCreate();

    char *hashSource = malloc(strlen(_texturePath) + 1);
    strcpy(hashSource, _texturePath);
    char *textureFile = malloc(strlen(_texturePath) + sizeof(TEXTURES_PATH));
    strcpy(textureFile, TEXTURES_PATH);
    strcat(textureFile, _texturePath);
    uint64_t hashSourceHash = texturePathHash(hashSource);
    int textureCacheIndex = textureCacheSearch(_textureCache, hashSourceHash);
    if (textureCacheIndex != -1) {
        free(textureFile);
        free(hashSource);
        return LIST_IDX(_textureCache, textureCacheIndex).value;
    }

    uint32_t textureId;

    int width, height, numColorChannels;
    unsigned char *data;
    // load default texture (white)
    if (strcmp(textureFile, "") == 0)
        data = stbi_load("textures/default.jpg", &width, &height,
                         &numColorChannels, 0);
    else
        data = stbi_load(textureFile, &width, &height, &numColorChannels, 0);
    bool isTransparent =
        numColorChannels == 2 ||
        numColorChannels == 4; // stb sets these values if an image has either:
                               // grey + alpha channel (hence 2)
                               // red + green + blue + alpha channel (hence 4)
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
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB + isTransparent, width, height, 0,
                 GL_RGB + isTransparent, GL_UNSIGNED_BYTE,
                 data); // HINT: type is either 0 or 1; GL_RGBA is 1+GL_RGB
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);

    free(textureFile);

    Texture *texture = malloc(sizeof(Texture));
    texture->id = textureId;

    textureCacheAppend(_textureCache, hashSourceHash, texture);
    free(hashSource);

    return texture;
}
void textureFree(Texture *texture) {
    for (int i = 0; i < _textureCache->size; i++) {
        if (texture == LIST_IDX(_textureCache, i).value) {
            LIST_POP(_textureCache, i);
        }
    }
    glDeleteTextures(1, &texture->id);
    free(texture);
}
void textureFreeCache() { textureCacheFree(_textureCache); }

textureCache textureCacheCreate() {
    textureCache cache;
    LIST_INIT(cache, 4);
    return cache;
}
void textureCacheAppend(textureCache cache, uint32_t key, Texture *value) {
    LIST_APPEND(cache, ((struct textureCacheEntry){.key = key & 0xFFFFFFFF,
                                                   .value = value}));
}
int textureCacheSearch(textureCache cache, uint32_t key) {
    for (int i = 0; i < cache->size; i++) {
        if (key == LIST_IDX(cache, i).key)
            return i;
    }
    return -1;
}
void textureCacheFree(textureCache cache) {
    for (int i = 0; i < cache->size; i++) {
        textureFree(LIST_IDX(cache, i).value);
    }
    LIST_FREE(cache);
}

char *readTextureFile(const char *fileName) {
    FILE *file = fopen(fileName, "rb");
    if (file == NULL) {
        printf("file %s could not be read", fileName);
        exit(EXIT_FAILURE);
    }
    fseek(file, 0, SEEK_END);
    long fsize = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *string = malloc(fsize + 1);
    if (fread(string, fsize, 1, file) < 1) {
        fprintf(stderr, "couldn't read file \"%s\"\n", fileName);
        exit(1);
    }
    fclose(file);

    string[fsize] = 0;
    return string;
}
uint64_t texturePathHash(char *str) {
    uint64_t hash = 5381;
    int c;

    while ((c = (uint8_t)*str++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}
