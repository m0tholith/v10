#ifndef SHADER_H
#define SHADER_H

#include "lib/list.h"
#include <stdint.h>
#include <string.h>

#define SHADERS_PATH "shaders/"

struct shader_uniform {
    char *name;
    int location;
};
typedef struct {
    uint32_t ID;

    char *VertPath;
    uint32_t VertID;

    char *GeomPath;
    uint32_t GeomID;

    char *FragPath;
    uint32_t FragID;

    LIST(struct shader_uniform) Uniforms;

    uint32_t _hash;
} Shader;
struct shaderCacheEntry {
    uint32_t key;
    Shader *value;
};
typedef LIST(struct shaderCacheEntry) ShaderCache;

ShaderCache shaderCacheCreate();
Shader *shaderCacheIndex(ShaderCache cache, int idx);

void shaderSetCache(ShaderCache cache);
Shader *shaderCreate(const char *vertexShaderPath,
                     const char *geometryShaderPath,
                     const char *fragmentShaderPath);
int shaderGetUniformLocation(Shader *shader, char *uniformName);
void shaderFree(Shader *shader);

#endif // !SHADER_H
