#include "armature.h"

#include "cglm/struct/mat4.h"
#include "node.h"
#include <cglm/struct.h>
#include <stdio.h>
#include <stdlib.h>

void printMatrix(mat4s *matrix) {
    for (int j = 0; j < 4; j++) {
        for (int k = 0; k < 4; k++) {
            if (matrix->raw[k][j] == 0)
                printf(" 0.0000    ");
            else
                printf("%s%.4f    ", matrix->raw[k][j] < 0 ? "" : " ",
                       matrix->raw[k][j]);
        }
        printf("\n");
    }
    printf("\n");
}

Armature *armatureCreate() {
    Armature *armature = malloc(sizeof(Armature));
    for (int i = 0; i < MAX_BONES; i++) {
        armature->Bones[i] = NULL;
        armature->BoneMatrices[i] = GLMS_MAT4_IDENTITY;
        armature->OffsetMatrices[i] = GLMS_MAT4_IDENTITY;
    }
    return armature;
}
void armatureSetOffsetMatrices(Armature *armature) {
    for (int i = 0; i < MAX_BONES; i++) {
        if (armature->Bones[i] == NULL)
            continue;
        armature->OffsetMatrices[i] =
            glms_mat4_inv(nodeGetFinalTransform(armature->Bones[i]));
        printf("armature->Bones[%d]->Name = %s\n", i, armature->Bones[i]->Name);
        printMatrix(&armature->OffsetMatrices[i]);
    }
}
void armatureSetBoneMatrices(Armature *armature) {
    for (int i = 0; i < MAX_BONES; i++) {
        if (armature->Bones[i] == NULL)
            continue;

        armature->BoneMatrices[i] =
            glms_mat4_mul(nodeGetFinalTransform(armature->Bones[i]),
                          armature->OffsetMatrices[i]);
        printf("armature->Bones[%d]->Name = %s\n", i, armature->Bones[i]->Name);
        printMatrix(&armature->OffsetMatrices[i]);
        printMatrix(&armature->BoneMatrices[i]);
    }
}
void armatureFree(Armature *armature) { free(armature); }
