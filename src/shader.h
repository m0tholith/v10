#ifndef SHADER_H
#define SHADER_H

#define objectId unsigned int

objectId shaderCreate(const char *vertexShaderPath,
                      const char *fragmentShaderPath);

#endif
