#ifndef ANIMATION_H
#define ANIMATION_H

#include "assimp/scene.h"
#include "cglm/types-struct.h"
#include "node.h"

typedef struct {
    float Time;
    vec3s Value;
} AnimationVectorKey;
typedef struct {
    float Time;
    versors Value;
} AnimationQuaternionKey;

typedef struct {
    int PositionKeyCount, RotationKeyCount, ScalingKeyCount;
    AnimationVectorKey *PositionKeys;
    AnimationQuaternionKey *RotationKeys;
    AnimationVectorKey *ScalingKeys;
    char *Name;
} AnimationNode;
typedef struct {
    int NodeCount;
    AnimationNode *Nodes;
    char *Name;
} Animation;

Animation *animationCreate(struct aiScene *scene, char *name);
void animationPlay(Animation *animation, Node *rootNode);
void animationFree(Animation *animation);

#endif // !ANIMATION_H
