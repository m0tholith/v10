#ifndef SHADER_H
#define SHADER_H

#define SHADERS_PATH "shaders/"

#include <stdint.h>

uint32_t shaderCreate(const char *vertexShaderPath,
                      const char *fragmentShaderPath);

#endif // !SHADER_H
