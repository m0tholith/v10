#include "texture.h"

#include "glad/glad.h"
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
struct textureCache {
    size_t used;
    size_t size;
    struct textureCacheEntry *array;
};
struct textureCache *textureCacheCreate();
void textureCacheAppend(struct textureCache *cache, uint32_t key,
                        Texture *value);
void textureCacheRemove(struct textureCache *cache, int index);
int textureCacheSearch(struct textureCache *cache, uint32_t key);
void textureCacheFree(struct textureCache *cache);

struct textureCache *_textureCache;

Texture *textureCreate(const char *_texturePath, enum TEXTURETYPE type,
                       bool optional) {
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
        free(hashSource);
        return _textureCache->array[textureCacheIndex].value;
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

    textureCacheAppend(_textureCache, hashSourceHash, texture);
    free(hashSource);

    return texture;
}
void textureFree(Texture *texture) {
    glDeleteTextures(1, &texture->id);
    free(texture);
}
void textureFreeCache() { textureCacheFree(_textureCache); }

void textureCacheFitSize(struct textureCache *cache) {
    struct textureCacheEntry *temp =
        realloc(cache->array, cache->size * sizeof(struct textureCacheEntry));
    if (temp != NULL)
        cache->array = temp;
    else {
        fprintf(stderr, "could not resize texture cache\n");
        exit(EXIT_FAILURE);
    }
}
struct textureCache *textureCacheCreate() {
    struct textureCache *cache = malloc(sizeof(struct textureCache));
    cache->array = NULL;
    cache->used = 0;
    cache->size = 4;
    textureCacheFitSize(cache);
    return cache;
}
void textureCacheAppend(struct textureCache *cache, uint32_t key,
                        Texture *value) {
    if (cache->used > cache->size) {
        cache->size *= 2;
        textureCacheFitSize(cache);
    }
    cache->array[cache->used++] =
        (struct textureCacheEntry){.key = key & 0xFFFFFFFF, .value = value};
}
void textureCacheRemove(struct textureCache *cache, int index) {
    textureFree(cache->array[index].value);
    for (int i = index + 1; i < cache->used; i++) {
        cache[i - 1] = cache[i];
    }
    cache->size--;
}
int textureCacheSearch(struct textureCache *cache, uint32_t key) {
    for (int i = 0; i < cache->used; i++) {
        if (key == cache->array[i].key)
            return i;
    }
    return -1;
}
void textureCacheFree(struct textureCache *cache) {
    for (int i = 0; i < cache->used; i++) {
        textureFree(cache->array[i].value);
    }
    free(cache->array);
    free(cache);
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
