#ifndef ARMATURE_H
#define ARMATURE_H

#include "cglm/types-struct.h"
#include "node.h"

#define MAX_BONES 20

typedef Node *Bone;
;
typedef struct {
    Bone Bones[MAX_BONES];         // size is MAX_BONES
    mat4s BoneMatrices[MAX_BONES]; // size is MAX_BONES
} Armature;

/// prepares bone data. free with `armatureFree`
Armature *armatureCreate();
/// fetches bone transforms and sets them in `BoneMatrices`
void armatureSetBoneMatrices(Armature *armature);
/// frees armature object
void armatureFree(Armature *armature);

#endif // !ARMATURE_H
