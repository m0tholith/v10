#ifndef ARMATURE_H
#define ARMATURE_H

#include "cglm/types-struct.h"
#include "node.h"

#define MAX_BONES 20

typedef Node *Bone;

typedef struct {
    Bone Bones[MAX_BONES];         // size is MAX_BONES
    mat4s BoneMatrices[MAX_BONES]; // movement matrix of each bone,
                                   // multiplication result of current transform
                                   // and offset matrix (inverse transform);
                                   // size is MAX_BONES
    mat4s OffsetMatrices[MAX_BONES]; // inverse model-space transform of every
                                     // bone in original pose; size is MAX_BONES
} Armature;

/// prepares bone data. free with `armatureFree`
Armature *armatureCreate();
/// fetches bone transforms, inverts them and sets them in `OffsetMatrices`
void armatureSetOffsetMatrices(Armature *armature);
/// fetches bone transforms and sets them in `BoneMatrices`
void armatureSetBoneMatrices(Armature *armature);
/// frees armature object
void armatureFree(Armature *armature);

#endif // !ARMATURE_H
