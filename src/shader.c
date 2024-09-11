#include "shader.h"

#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *readFile(const char *fileName);
uint64_t hash(char *str);

struct shaderCache {
    size_t used;
    size_t size;
    uint64_t *array;
};
struct shaderCache *shaderCacheCreate();
uint32_t shaderCacheValue(struct shaderCache *cache, int index);
void shaderCachePush(struct shaderCache *cache, uint32_t key, uint32_t value);
int shaderCacheSearch(struct shaderCache *cache, uint32_t key);
void shaderCacheFree(struct shaderCache *cache);

struct shaderCache *_cache;

uint32_t shaderCreate(const char *_vertexShaderPath,
                      const char *_fragmentShaderPath) {
    if (_cache == NULL)
        _cache = shaderCacheCreate();

    char *hashSource =
        malloc(strlen(_vertexShaderPath) + strlen(_fragmentShaderPath) + 1);
    strcpy(hashSource, _vertexShaderPath);
    strcat(hashSource, _fragmentShaderPath);
    uint64_t hashSourceHash = hash(hashSource);
    int shaderCacheIndex = shaderCacheSearch(_cache, hashSourceHash);
    if (shaderCacheIndex != -1) {
        free(hashSource);
        return _cache->array[shaderCacheIndex] & 0xFFFF;
    }

    char *vertexShaderPath =
        malloc(strlen(_vertexShaderPath) + sizeof(SHADERS_PATH));
    strcpy(vertexShaderPath, SHADERS_PATH);
    strcat(vertexShaderPath, _vertexShaderPath);
    char *fragmentShaderPath =
        malloc(strlen(_fragmentShaderPath) + sizeof(SHADERS_PATH));
    strcpy(fragmentShaderPath, SHADERS_PATH);
    strcat(fragmentShaderPath, _fragmentShaderPath);

    char *vertexShaderContents = readFile(vertexShaderPath);
    const char *vertexShaderSource = vertexShaderContents;
    uint32_t vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // error checking
    int success = 0;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, sizeof(infoLog), NULL, infoLog);
        fprintf(stderr, "vertex shader could not compile: %s", infoLog);
        exit(EXIT_FAILURE);
    }

    char *fragmentShaderContents = readFile(fragmentShaderPath);
    const char *fragmentShaderSource = fragmentShaderContents;
    uint32_t fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, sizeof(infoLog), NULL, infoLog);
        fprintf(stderr, "fragment shader could not compile: %s", infoLog);
        exit(EXIT_FAILURE);
    }

    uint32_t shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    int shaderSuccess;
    glGetProgramiv(shaderProgram, GL_VALIDATE_STATUS, &shaderSuccess);
    if (success != GL_TRUE) {
        glGetProgramInfoLog(shaderProgram, sizeof(infoLog), NULL, infoLog);
        fprintf(stderr, "shader program could not be linked: %s", infoLog);
        exit(EXIT_FAILURE);
    }
    glUseProgram(shaderProgram);
    glDeleteShader(fragmentShader);
    glDeleteShader(vertexShader);

    free(vertexShaderContents);
    free(fragmentShaderContents);
    free(vertexShaderPath);
    free(fragmentShaderPath);

    shaderCachePush(_cache, hashSourceHash, shaderProgram);
    free(hashSource);

    return shaderProgram;
}
void shaderFree(uint32_t shader) { glDeleteProgram(shader); }
void shaderFreeCache() { shaderCacheFree(_cache); }

void shaderCacheFitSize(struct shaderCache *cache) {
    uint64_t *temp = realloc(cache->array, cache->size);
    if (temp != NULL)
        cache->array = temp;
    else {
        fprintf(stderr, "could not resize shader cache\n");
        exit(EXIT_FAILURE);
    }
}
struct shaderCache *shaderCacheCreate() {
    struct shaderCache *cache = malloc(sizeof(struct shaderCache));
    cache->array = NULL;
    cache->used = 0;
    cache->size = 4;
    shaderCacheFitSize(cache);
    return cache;
}
uint32_t shaderCacheValue(struct shaderCache *cache, int index) {
    return cache->array[index] & 0xFFFF;
}
void shaderCachePush(struct shaderCache *cache, uint32_t key, uint32_t value) {
    if (cache->used > cache->size) {
        cache->size *= 2;
        shaderCacheFitSize(cache);
    }
    cache->array[cache->used++] = ((uint64_t)key << 32) | value;
}
int shaderCacheSearch(struct shaderCache *cache, uint32_t key) {
    for (int i = 0; i < cache->used; i++) {
        if ((cache->array[i] >> 32) == key)
            return i;
    }
    return -1;
}
void shaderCacheFree(struct shaderCache *cache) {
    for (int i = 0; i < cache->used; i++) {
        shaderFree(shaderCacheValue(cache, i));
    }
    free(cache->array);
    free(cache);
}

char *readFile(const char *fileName) {
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
uint64_t hash(char *str) {
    uint64_t hash = 5381;
    int c;

    while ((c = (uint8_t)*str++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}
