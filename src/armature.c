#include "armature.h"

#include "glad/glad.h"
#include <cglm/struct/mat4.h>
#include <stdio.h>
#include <stdlib.h>

Armature *armatureCreate(Model *model) {
    Armature *armature = malloc(sizeof(Armature));
    armature->Model = model;

    if (model->NodeCount > MAX_BONES) {
        fprintf(stderr, "couldn't create armature: model has too many nodes");
        exit(1);
    }

    for (int i = 0; i < MAX_BONES; i++) {
        armature->BoneTransformations[i] = GLMS_MAT4_IDENTITY;
        if (i >= model->NodeCount) {
            armature->OffsetMatrices[i] = GLMS_MAT4_IDENTITY;
            continue;
        }
        armature->OffsetMatrices[i] =
            glms_mat4_inv(model->NodeEntries[i].WorldFromLocal);
    }

    return armature;
}
void armatureApplyTransformations(Armature *armature) {
    for (int i = 0; i < armature->Model->NodeCount; i++) {
        armature->BoneTransformations[i] =
            glms_mat4_mul(armature->Model->NodeEntries[i].WorldFromLocal,
                          armature->OffsetMatrices[i]);
    }
    for (int i = 0; i < armature->Model->MaterialCount; i++) {
        glUseProgram(armature->Model->Materials[i]->Shader);

        glUniformMatrix4fv(
            glGetUniformLocation(armature->Model->Materials[i]->Shader,
                                 "_boneTransformations"),
            MAX_BONES, GL_FALSE, (void *)&armature->BoneTransformations);
    }
}
void armatureFree(Armature *armature) { free(armature); }
