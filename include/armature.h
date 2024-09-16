#ifndef ARMATURE_H
#define ARMATURE_H

#include "cglm/types-struct.h"
#include "model.h"

#define MAX_BONES 100

typedef struct {
    /// inverse of world matrix of node
    mat4s OffsetMatrices[MAX_BONES];
    mat4s BoneTransformations[MAX_BONES];

    Model *Model;
} Armature;

/// requires calling `modelSetNodeWorldMatrices` beforehand
///
/// @return `Armature *` armature with offset matrices set up
Armature *armatureCreate(Model *model);
/// call before rendering; calculates bone transformations and sends them to all
/// shaders
void armatureApplyTransformations(Armature *armature);
void armatureFree(Armature *armature);

#endif // !ARMATURE_H
