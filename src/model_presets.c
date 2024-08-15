#include "model_presets.h"

#include "material.h"
#include "model.h"
#include "shader.h"
#include "texture.h"

#include <stdarg.h>
#include <stdio.h>

void modelPresetTintedFree(void *_model);
Model *modelPresetTinted(const char *modelFilename,
                         const char *vertexShaderPath,
                         const char *fragmentShaderPath, int materialCount,
                         ...) {
    uint32_t shader = shaderCreate(vertexShaderPath, fragmentShaderPath);
    Model *model = modelLoad(modelFilename);
    va_list colors;
    va_start(colors, materialCount);
    for (int i = 0; i < materialCount; i++) {
        vec3s color = va_arg(colors, vec3s);
        model->Materials[i] = materialCreate(
            shader, 1,
            materialPropertyCreate("color", MATTYPE_VEC3, (void *)color.raw));
        materialApplyProperties(model->Materials[i]);
        printf("%d, (%.2f,%.2f,%.2f)\n", i, color.r, color.g, color.b);
    }
    va_end(colors);
    model->OnDelete = &modelFreeWithMaterials;
    return model;
}
void modelPresetTintedFree(void *_model) {
    Model *model = (Model *)_model;
    if (model->MaterialCount > 0)
        shaderFree(model->Materials[0]->Shader);
    _modelDelete(_model);
}

void modelPresetTexturedFree(void *_model);
Model *modelPresetTextured(const char *modelFilename,
                           const char *vertexShaderPath,
                           const char *fragmentShaderPath,
                           const char *texturePath) {
    uint32_t shader = shaderCreate(vertexShaderPath, fragmentShaderPath);
    Model *model = modelLoad(modelFilename);
    Material *material = materialCreate(
        shader, 1,
        materialPropertyCreate(
            "_texture", MATTYPE_TEXTURE2D,
            (void *)textureCreate(texturePath, TEXTURETYPE_RGB, false)));
    modelSetDefaultMaterial(model, material);
    model->OnDelete = &modelPresetTexturedFree;

    return model;
}
void modelPresetTexturedFree(void *_model) {
    Model *model = (Model *)_model;
    // delete only one material out of the bunch, assuming that the same
    // material takes all slots
    if (model->MaterialCount > 0) {
        shaderFree(model->Materials[0]->Shader);
        materialFree(model->Materials[0]);
    }
    _modelDelete(_model);
}

void modelPresetTexturedAllFree(void *_model);
Model *modelPresetTexturedAll(const char *modelFilename,
                              const char *vertexShaderPath,
                              const char *fragmentShaderPath) {
    uint32_t shader = shaderCreate(vertexShaderPath, fragmentShaderPath);
    Model *model = modelLoad(modelFilename);
    for (int i = 0; i < model->MaterialCount; i++) {
        model->Materials[i] = materialCreate(
            shader, 1,
            materialPropertyCreate(
                "_texture", MATTYPE_TEXTURE2D,
                (void *)materialTextureDataCreate(model->Textures[i], 0)));
    }
    model->OnDelete = &modelFreeWithMaterials;
    return model;
}
void modelPresetTexturedAllFree(void *_model) {
    Model *model = (Model *)_model;
    // delete only one material out of the bunch, assuming that the same
    // material takes all slots
    if (model->MaterialCount > 0) {
        shaderFree(model->Materials[0]->Shader);
        materialFree(model->Materials[0]);
    }
    _modelFreeMaterials(_model);
    _modelDelete(_model);
}

void modelFreeWithMaterials(void *_model) {
    _modelFreeMaterials(_model);
    _modelDelete(_model);
}
