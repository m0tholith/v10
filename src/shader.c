#include "shader.h"

#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *readShaderFile(const char *fileName);
uint64_t shaderPathHash(char *str);

void shaderCacheFitSize(ShaderCache *cache);
void shaderCacheAppend(ShaderCache *cache, uint32_t key, Shader *value);
void shaderCacheRemove(ShaderCache *cache, int index);
int shaderCacheSearch(ShaderCache *cache, uint32_t key);

ShaderCache *_shaderCache;

ShaderCache *shaderCacheCreate() {
    ShaderCache *cache = malloc(sizeof(ShaderCache));
    cache->Array = NULL;
    cache->Used = 0;
    cache->Size = 4;
    shaderCacheFitSize(cache);
    return cache;
}
void shaderSetCache(ShaderCache *cache) { _shaderCache = cache; }
Shader *shaderCreate(const char *_vertexShaderPath,
                     const char *_geometryShaderPath,
                     const char *_fragmentShaderPath) {
    bool useGeomShader = strlen(_geometryShaderPath) != 0;

    char *hashSource = "";
    uint64_t hashSourceHash = 0;
    if (_shaderCache != NULL) {
        hashSource =
            malloc(strlen(_vertexShaderPath) + strlen(_geometryShaderPath) +
                   strlen(_fragmentShaderPath) + 1);
        strcpy(hashSource, _vertexShaderPath);
        strcat(hashSource, _geometryShaderPath);
        strcat(hashSource, _fragmentShaderPath);
        hashSourceHash = shaderPathHash(hashSource);
        int shaderCacheIndex = shaderCacheSearch(_shaderCache, hashSourceHash);
        if (shaderCacheIndex != -1) {
            free(hashSource);
            return _shaderCache->Array[shaderCacheIndex].value;
        }
    }

    char *vertexShaderPath =
        malloc(strlen(_vertexShaderPath) + sizeof(SHADERS_PATH));
    strcpy(vertexShaderPath, SHADERS_PATH);
    strcat(vertexShaderPath, _vertexShaderPath);
    char *geometryShaderPath;
    if (useGeomShader) {
        geometryShaderPath =
            malloc(strlen(_geometryShaderPath) + sizeof(SHADERS_PATH));
        strcpy(geometryShaderPath, SHADERS_PATH);
        strcat(geometryShaderPath, _geometryShaderPath);
    } else
        geometryShaderPath = "";
    char *fragmentShaderPath =
        malloc(strlen(_fragmentShaderPath) + sizeof(SHADERS_PATH));
    strcpy(fragmentShaderPath, SHADERS_PATH);
    strcat(fragmentShaderPath, _fragmentShaderPath);

    char *vertexShaderContents = readShaderFile(vertexShaderPath);
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

    char *geometryShaderContents = malloc(sizeof(char));
    uint32_t geometryShader = 0;
    if (useGeomShader) {
        free(geometryShaderContents);
        char *geometryShaderContents = readShaderFile(geometryShaderPath);
        const char *geometryShaderSource = geometryShaderContents;
        geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(geometryShader, 1, &geometryShaderSource, NULL);
        glCompileShader(geometryShader);
        glGetShaderiv(geometryShader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(geometryShader, sizeof(infoLog), NULL, infoLog);
            fprintf(stderr, "geometry shader could not compile: %s", infoLog);
            exit(EXIT_FAILURE);
        }
    }

    char *fragmentShaderContents = readShaderFile(fragmentShaderPath);
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
    if (useGeomShader)
        glAttachShader(shaderProgram, geometryShader);
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

    free(vertexShaderContents);
    free(geometryShaderContents);
    free(fragmentShaderContents);
    free(vertexShaderPath);
    if (useGeomShader)
        free(geometryShaderPath);
    free(fragmentShaderPath);

    Shader *shader = malloc(sizeof(Shader));

    shader->ID = shaderProgram;

    shader->VertPath = malloc(strlen(_vertexShaderPath) + 1);
    strcpy(shader->VertPath, _vertexShaderPath);
    shader->VertID = vertexShader;

    if (useGeomShader) {
        shader->GeomPath = malloc(strlen(_geometryShaderPath) + 1);
        strcpy(shader->GeomPath, _geometryShaderPath);
        shader->GeomID = geometryShader;
    } else {
        shader->GeomPath = "";
        shader->GeomID = 0;
    }

    shader->FragPath = malloc(strlen(_fragmentShaderPath) + 1);
    strcpy(shader->FragPath, _fragmentShaderPath);
    shader->FragID = fragmentShader;

    if (_shaderCache != NULL) {
        shader->_hash = hashSourceHash;
        shaderCacheAppend(_shaderCache, hashSourceHash, shader);
        free(hashSource);
    }

    return shader;
}
void shaderFree(Shader *shader) {
    bool useGeomShader = shader->GeomID != 0;

    glDeleteProgram(shader->ID);
    glDeleteShader(shader->VertID);
    if (useGeomShader)
        glDeleteShader(shader->GeomID);
    glDeleteShader(shader->FragID);
    free(shader->VertPath);
    if (useGeomShader)
        free(shader->GeomPath);
    free(shader->FragPath);
    free(shader);
}

void shaderCacheFitSize(ShaderCache *cache) {
    struct shaderCacheEntry *temp =
        realloc(cache->Array, cache->Size * sizeof(struct shaderCacheEntry));
    if (temp != NULL)
        cache->Array = temp;
    else {
        fprintf(stderr, "could not resize shader cache\n");
        exit(EXIT_FAILURE);
    }
}
void shaderCacheAppend(ShaderCache *cache, uint32_t key, Shader *value) {
    if (cache->Used >= cache->Size) {
        cache->Size *= 2;
        shaderCacheFitSize(cache);
    }
    cache->Array[cache->Used++] =
        (struct shaderCacheEntry){.key = key & 0xFFFFFFFF, .value = value};
}
void shaderCacheRemove(ShaderCache *cache, int index) {
    shaderFree(cache->Array[index].value);
    for (int i = index + 1; i < cache->Used; i++) {
        cache[i - 1] = cache[i];
    }
    cache->Used--;
}
int shaderCacheSearch(ShaderCache *cache, uint32_t key) {
    for (int i = 0; i < cache->Used; i++) {
        if (key == cache->Array[i].key)
            return i;
    }
    return -1;
}
void shaderCacheFree(ShaderCache *cache) {
    while (cache->Used > 0) {
        shaderCacheRemove(cache, cache->Used - 1);
    }
    free(cache->Array);
    free(cache);
}

char *readShaderFile(const char *fileName) {
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
uint64_t shaderPathHash(char *str) {
    uint64_t hash = 5381;
    int c;

    while ((c = (uint8_t)*str++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}
