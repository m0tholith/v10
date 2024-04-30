#ifndef MATERIAL_H
#define MATERIAL_H

#include "cglm/types-struct.h"

typedef struct {
    unsigned int Shader;
    vec3s Tint;
} Material;

Material *materialCreate(unsigned int shader, vec3s tint);
void materialApplyProperties(Material *material);

#endif // !MATERIAL_H
