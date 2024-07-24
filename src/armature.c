#include "armature.h"
#include <stdlib.h>

#include <cglm/struct.h>

Armature *armatureCreate() {
    Armature *armature = malloc(sizeof(Armature));
    for (int i = 0; i < MAX_BONES; i++) {
        armature->Bones[i].BoneNode = NULL;
        armature->BoneMatrices[i] = GLMS_MAT4_IDENTITY;
    }
    return armature;
}
void armatureSetBoneMatrices(Armature *armature) {
    for (int i = 0; i < MAX_BONES; i++) {
        armature->BoneMatrices[i] = nodeGetFinalTransform(armature->Bones[i]);
    }
}
void armatureFree(Armature *armature) { free(armature); }
