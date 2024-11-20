#ifndef ANIMATION_H
#define ANIMATION_H

#include "node.h"
#include <assimp/scene.h>
#include <cglm/types-struct.h>

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
    struct Node *Node;
    /// interpolation function to use for this animation node; set with
    /// `animationCreate`
    float (*InterpFunction)(float);
} AnimationNode;
typedef struct {
    int Duration; // in ticks
    float Time;
    int TicksPerSec;
    int NodeCount;
    AnimationNode *Nodes;
    char *Name;
} Animation;

// interpolation functions
float StepInterp(float x);
float LinearInterp(float x);
float SmoothStepInterp(float x);

/**
 * This function is only to be used by whatever imports models.
 */
Animation *animationCreate(const struct aiScene *scene, char *name,
                           struct Node *rootNode);
/**
 * Call this function every frame.
 */
void animationStep(Animation *animation, float deltaTime);
/**
 * This function is only to be used by whatever imports models.
 */
void animationFree(Animation *animation);

#endif // !ANIMATION_H
