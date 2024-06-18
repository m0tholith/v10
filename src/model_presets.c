#include "model_presets.h"

#include "material.h"
#include "model.h"
#include "shader.h"
#include "texture.h"
#include <stdarg.h>
#include <stdio.h>

void modelPresetTexturedDelete(void *_model);

Model *modelPresetTintedLoad(const char *modelFilename,
                             const char *vertexShaderPath,
                             const char *fragmentShaderPath, ...) {
    Model *model = modelLoad(modelFilename);
    va_list colors;
    va_start(colors, model->MaterialCount);
    for (int i = 0; i < model->MaterialCount - 1; i++) {
        vec3s color = va_arg(colors, vec3s);
        model->Materials[i] = materialCreate(
            shaderCreate(vertexShaderPath, fragmentShaderPath), 1,
            materialPropertyCreate("color", MATTYPE_VEC3, (void *)color.raw));
        materialApplyProperties(model->Materials[i]);
        printf("%d, (%.2f,%.2f,%.2f)\n", i, color.r, color.g, color.b);
    }
    va_end(colors);
    model->OnDelete = &modelDeleteFreeMaterials;
    return model;
}
Model *modelPresetTexturedLoad(const char *modelFilename,
                               const char *vertexShaderPath,
                               const char *fragmentShaderPath,
                               const char *texturePath) {
    Model *model = modelLoad(modelFilename);
    Material *material = materialCreate(
        shaderCreate(vertexShaderPath, fragmentShaderPath), 1,
        materialPropertyCreate(
            "_texture", MATTYPE_TEXTURE2D,
            (void *)textureCreate(texturePath, TEXTURETYPE_RGB).Id));
    for (int i = 0; i < model->MaterialCount; i++) {
        model->Materials[i] = material;
    }
    model->OnDelete = &modelPresetTexturedDelete;

    return model;
}
void modelPresetTexturedDelete(void *_model) {
    Model *model = (Model *)_model;
    if (model->MaterialCount > 0)
        materialFree(model->Materials[0]);
    modelDelete(model);
}
