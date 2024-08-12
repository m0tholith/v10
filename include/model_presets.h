#ifndef MODEL_PRESETS_H
#define MODEL_PRESETS_H

#include "model.h"

/// arguments are vec3s for each material's color for the model
Model *modelPresetTinted(const char *modelFilename,
                         const char *vertexShaderPath,
                         const char *fragmentShaderPath, int materialCount,
                         ...);
/// sets all materials of model to speified texture path using a single material
Model *modelPresetTextured(const char *modelFilename,
                           const char *vertexShaderPath,
                           const char *fragmentShaderPath,
                           const char *texturePath);

/// callback for `modelFree`, calls `_modelDelete` and `_modelFreeMaterials`
void modelFreeWithMaterials(void *model);

#endif // !MODEL_PRESETS_H
