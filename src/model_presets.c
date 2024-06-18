#include "model_presets.h"

#include "material.h"
#include "shader.h"
#include <stdarg.h>
#include <stdio.h>

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
    return model;
}
