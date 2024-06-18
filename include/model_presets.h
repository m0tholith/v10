#ifndef MODEL_PRESETS_H
#define MODEL_PRESETS_H

#include "model.h"

// arguments are vec3s for each material's color for the model
// creates materials that need to be freed using modelDeleteFreeMaterials
Model *modelPresetTintedLoad(const char *modelFilename,
                             const char *vertexShaderPath,
                             const char *fragmentShaderPath, ...);

#endif // !MODEL_PRESETS_H
