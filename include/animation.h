#ifndef ANIMATION_H
#define ANIMATION_H

#include "assimp/scene.h"
#include "cglm/types-struct.h"
#include "node.h"

typedef struct {
    int Time;
    vec3s Value;
} AnimationVectorKey;
typedef struct {
    int Time;
    versors Value;
} AnimationQuaternionKey;

typedef struct {
    int PositionKeyCount, RotationKeyCount, ScalingKeyCount;
    AnimationVectorKey *PositionKeys;
    AnimationQuaternionKey *RotationKeys;
    AnimationVectorKey *ScalingKeys;
    Node *Node;
} AnimationNode;
typedef struct {
    int Duration; // in ticks
    float Time;
    int TicksPerSec;
    int NodeCount;
    AnimationNode *Nodes;
    char *Name;
} Animation;

Animation *animationCreate(struct aiScene *scene, char *name, Node *rootNode);
void animationStep(Animation *animation, float deltaTime);
void animationStepLinear(Animation *animation, float deltaTime);
void animationFree(Animation *animation);

#endif // !ANIMATION_H
