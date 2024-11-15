#ifndef SKYBOX_H
#define SKYBOX_H

#include "cubemap.h"
#include "shader.h"

typedef struct {
    Cubemap *Cubemap;
    Shader *Shader;
    uint32_t VAO;
    uint32_t VBO;
} Skybox;
Skybox *skyboxCreate(Cubemap *cubemap);
void skyboxRender(Skybox *skybox);
void skyboxFree(Skybox *skybox);

#endif // !SKYBOX_H
