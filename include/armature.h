#ifndef ARMATURE_H
#define ARMATURE_H

#include "cglm/types-struct.h"
#include "node.h"

#define MAX_BONES 20

typedef struct {
    Node *BoneNode;
} Bone;
typedef struct {
    Bone Bones[MAX_BONES];         // size is MAX_BONES
    mat4s BoneMatrices[MAX_BONES]; // size is MAX_BONES
} Armature;

Armature *armatureCreate();
void armatureSetBoneMatrices(Armature *armature);
void armatureFree(Armature *armature);

#endif // !ARMATURE_H
