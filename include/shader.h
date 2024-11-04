#ifndef SHADER_H
#define SHADER_H

#include <stdint.h>
#include <string.h>

#define SHADERS_PATH "shaders/"

typedef struct {
    uint32_t ID;

    char *VertPath;
    uint32_t VertID;

    char *GeomPath;
    uint32_t GeomID;

    char *FragPath;
    uint32_t FragID;

    uint32_t _hash;
} Shader;
struct shaderCacheEntry {
    uint32_t key;
    Shader *value;
};
typedef struct {
    size_t Used;
    size_t Size;
    struct shaderCacheEntry *Array;
} ShaderCache;

ShaderCache *shaderCacheCreate();
void shaderSetCache(ShaderCache *cache);
Shader *shaderCreate(const char *vertexShaderPath,
                     const char *geometryShaderPath,
                     const char *fragmentShaderPath);
void shaderFree(Shader *shader);
void shaderCacheFree(ShaderCache *cache);

#endif // !SHADER_H
