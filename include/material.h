#ifndef MATERIAL_H
#define MATERIAL_H

#include "cglm/types-struct.h"

typedef struct {
    unsigned int Shader;
    unsigned int TextureId;
    vec3s Tint;
} Material;

Material *materialCreate(unsigned int shader, unsigned int textureId,
                         vec3s tint);
void materialPreRender(Material *material);
void materialApplyProperties(Material *material);

#endif // !MATERIAL_H
