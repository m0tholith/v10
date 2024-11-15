#ifndef SKYBOX_H
#define SKYBOX_H

#include "cubemap.h"
#include "shader.h"

/// Sets up skybox state (VAO, VBO)
void skyboxInit();
/// Sets skybox cubemap and shader.
/// @param cubemap Setting this to NULL will only set the shader.
/// @param shader Shader pointer. Leave as NULL for default.
void skyboxSet(Cubemap *cubemap, Shader *shader);
/// Call this after all other entities have been rendered.
void skyboxRender();
void skyboxDestroy();

#endif // !SKYBOX_H
