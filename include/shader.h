#ifndef SHADER_H
#define SHADER_H

#include <stdint.h>

#define SHADERS_PATH "shaders/"

typedef struct {
    uint32_t ID;
    char *VertPath;
    char *FragPath;

    uint32_t _hash;
} Shader;

Shader *shaderCreate(const char *vertexShaderPath,
                     const char *fragmentShaderPath);
void shaderFree(Shader *shader);
void shaderFreeCache();

#endif // !SHADER_H
