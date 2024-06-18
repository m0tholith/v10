#ifndef MODEL_PRESETS_H
#define MODEL_PRESETS_H

#include "model.h"

// arguments are vec3s for each material's color for the model
// creates materials that need to be freed using modelDeleteFreeMaterials
Model *modelPresetTinted(const char *modelFilename,
                             const char *vertexShaderPath,
                             const char *fragmentShaderPath, ...);
// sets all materials of model to speified texture path using a single material,
// `modelFree` frees the single material (first one) and then calls
// `modelDelete`
Model *modelPresetTextured(const char *modelFilename,
                               const char *vertexShaderPath,
                               const char *fragmentShaderPath,
                               const char *texturePath);

#endif // !MODEL_PRESETS_H
