#ifndef MODEL_PRESETS_H
#define MODEL_PRESETS_H

#include "model.h"

// arguments are vec3s for each material's color for the model
Model *modelPresetTintedLoad(const char *modelFilename,
                             const char *vertexShaderPath,
                             const char *fragmentShaderPath, ...);
void modelPresetTintedDelete(Model *model);

#endif // !MODEL_PRESETS_H
