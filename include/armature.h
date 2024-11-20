#ifndef ARMATURE_H
#define ARMATURE_H

#include "model.h"
#include <cglm/types-struct.h>

#define MAX_BONES 100

/**
 * Struct that supplies transformations for skinned meshes' nodes.
 */
typedef struct {
    /// inverse of world matrix of node
    mat4s OffsetMatrices[MAX_BONES];
    mat4s BoneTransformations[MAX_BONES];

    Model *Model;
} Armature;

Armature *armatureCreate(Model *model);
void armatureApplyTransformations(Armature *armature);
void armatureFree(Armature *armature);

#endif // !ARMATURE_H
