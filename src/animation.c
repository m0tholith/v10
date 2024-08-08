#include "animation.h"

#include "assimp/anim.h"
#include "assimp/quaternion.h"
#include "cglm/struct/affine-pre.h"
#include "cglm/struct/affine.h"
#include "cglm/struct/mat4.h"
#include "cglm/struct/quat.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// interpolation functions
float StepInterp(float x) { return 0; }
float LinearInterp(float x) { return x; }
float SmoothStepInterp(float x) {
    return x < 0.5 ? 4 * x * x * x : 1 - pow(-2 * x + 2, 3) / 2;
}

vec3s aiVecToVec3s(struct aiVector3D vec) {
    return (vec3s){
        .x = vec.x,
        .y = vec.y,
        .z = vec.z,
    };
}
versors aiQuatToVersors(struct aiQuaternion quat) {
    return (versors){
        .x = quat.x,
        .y = quat.y,
        .z = quat.z,
        .w = quat.w,
    };
}
Node *getAnimationNode(char *nodeName, Node *rootNode);
Animation *animationCreate(struct aiScene *scene, char *name, Node *rootNode) {
    int animIndex = 0;
    printf("looking for animation \"%s\"\n", name);
    for (; animIndex < scene->mNumAnimations; animIndex++) {
        if (strcmp(name, scene->mAnimations[animIndex]->mName.data) == 0)
            break;
    }
    if (animIndex >= scene->mNumAnimations) {
        fprintf(stderr, "no animation found named \"%s\"\n", name);
        exit(EXIT_FAILURE);
    }

    struct aiAnimation *animation = scene->mAnimations[animIndex];
    Animation *resultAnimation = malloc(sizeof(Animation));

    resultAnimation->Name = malloc(strlen(name) + 1);
    strcpy(resultAnimation->Name, name);

    resultAnimation->Duration = animation->mDuration;
    resultAnimation->TicksPerSec = animation->mTicksPerSecond;
    resultAnimation->Time = 0;

    float (*interpFunction)(float);
    if (!strncmp(resultAnimation->Name, "Step", 4))
        interpFunction = StepInterp;
    else if (!strncmp(resultAnimation->Name, "Linear", 6))
        interpFunction = LinearInterp;
    else if (!strncmp(resultAnimation->Name, "CubicSpline", 11))
        interpFunction = SmoothStepInterp;
    else
        interpFunction = LinearInterp;

    resultAnimation->NodeCount = animation->mNumChannels;
    resultAnimation->Nodes =
        malloc(animation->mNumChannels * sizeof(AnimationNode));

    for (int i = 0; i < resultAnimation->NodeCount; i++) {
        AnimationNode *animNode = &resultAnimation->Nodes[i];
        struct aiNodeAnim *channel = animation->mChannels[i];
        animNode->Node = getAnimationNode(channel->mNodeName.data, rootNode);

        animNode->InterpFunction = interpFunction;

        animNode->PositionKeyCount = channel->mNumPositionKeys;
        animNode->RotationKeyCount = channel->mNumRotationKeys;
        animNode->ScalingKeyCount = channel->mNumScalingKeys;

        animNode->ScalingKeys =
            malloc(animNode->ScalingKeyCount * sizeof(AnimationVectorKey));
        animNode->RotationKeys =
            malloc(animNode->RotationKeyCount * sizeof(AnimationQuaternionKey));
        animNode->PositionKeys =
            malloc(animNode->PositionKeyCount * sizeof(AnimationVectorKey));

        for (int j = 0; j < animNode->ScalingKeyCount; j++) {
            animNode->ScalingKeys[j].Time = channel->mScalingKeys[j].mTime;
            animNode->ScalingKeys[j].Value =
                aiVecToVec3s(channel->mScalingKeys[j].mValue);
        }
        for (int j = 0; j < animNode->RotationKeyCount; j++) {
            animNode->RotationKeys[j].Time = channel->mRotationKeys[j].mTime;
            animNode->RotationKeys[j].Value =
                aiQuatToVersors(channel->mRotationKeys[j].mValue);
        }
        for (int j = 0; j < animNode->PositionKeyCount; j++) {
            animNode->PositionKeys[j].Time = channel->mPositionKeys[j].mTime;
            animNode->PositionKeys[j].Value =
                aiVecToVec3s(channel->mPositionKeys[j].mValue);
        }
    }

    return resultAnimation;
}
int getNextIndexVec(float targetTime, AnimationVectorKey *keyArray,
                    int keyCount);
int getNextIndexQuat(float targetTime, AnimationQuaternionKey *keyArray,
                     int keyCount);
void animationStep(Animation *animation, float deltaTime) {
    animation->Time += deltaTime * animation->TicksPerSec;
    while (animation->Time >= animation->Duration)
        animation->Time -= animation->Duration;

    // now here's the licker
    for (int i = 0; i < animation->NodeCount; i++) {
        AnimationNode *animNode = &animation->Nodes[i];
        mat4s newTransform = GLMS_MAT4_IDENTITY;

        if (animNode->ScalingKeyCount > 1) {
            int nextScaleKeyIdx =
                getNextIndexVec(animation->Time, animNode->ScalingKeys,
                                animNode->ScalingKeyCount);
            int prevScaleKeyIdx =
                (nextScaleKeyIdx + animNode->ScalingKeyCount - 1) %
                animNode->ScalingKeyCount;
            AnimationVectorKey nextScaleKey =
                animNode->ScalingKeys[nextScaleKeyIdx];
            AnimationVectorKey prevScaleKey =
                animNode->ScalingKeys[prevScaleKeyIdx];
            float t = (animation->Time - prevScaleKey.Time) /
                      (nextScaleKey.Time - prevScaleKey.Time);
            newTransform = glms_scale(
                newTransform,
                glms_vec3_lerp(prevScaleKey.Value, nextScaleKey.Value,
                               animNode->InterpFunction(t)));
        } else
            newTransform =
                glms_scale(newTransform, animNode->ScalingKeys[0].Value);

        if (animNode->RotationKeyCount > 1) {
            int nextRotKeyIdx =
                getNextIndexQuat(animation->Time, animNode->RotationKeys,
                                 animNode->RotationKeyCount);
            int prevRotKeyIdx =
                (nextRotKeyIdx + animNode->RotationKeyCount - 1) %
                animNode->RotationKeyCount;
            AnimationQuaternionKey nextRotKey =
                animNode->RotationKeys[nextRotKeyIdx];
            AnimationQuaternionKey prevRotKey =
                animNode->RotationKeys[prevRotKeyIdx];
            float t = (animation->Time - prevRotKey.Time) /
                      (nextRotKey.Time - prevRotKey.Time);
            newTransform = glms_quat_rotate(
                newTransform,
                glms_quat_slerp(prevRotKey.Value, nextRotKey.Value,
                                animNode->InterpFunction(t)));
        } else
            newTransform =
                glms_quat_rotate(newTransform, animNode->RotationKeys[0].Value);

        if (animNode->PositionKeyCount > 1) {
            int nextPosKeyIdx =
                getNextIndexVec(animation->Time, animNode->PositionKeys,
                                animNode->PositionKeyCount);
            int prevPosKeyIdx =
                (nextPosKeyIdx + animNode->PositionKeyCount - 1) %
                animNode->PositionKeyCount;
            AnimationVectorKey nextPosKey =
                animNode->PositionKeys[nextPosKeyIdx];
            AnimationVectorKey prevPosKey =
                animNode->PositionKeys[prevPosKeyIdx];
            float t = (animation->Time - prevPosKey.Time) /
                      (nextPosKey.Time - prevPosKey.Time);
            newTransform = glms_translate(
                newTransform, glms_vec3_lerp(prevPosKey.Value, nextPosKey.Value,
                                             animNode->InterpFunction(t)));
        } else
            newTransform =
                glms_translate(newTransform, animNode->PositionKeys[0].Value);

        animNode->Node->ParentFromLocal = newTransform;
    }
}
void animationFree(Animation *animation) {
    for (int i = 0; i < animation->NodeCount; i++) {
        free(animation->Nodes[i].PositionKeys);
        free(animation->Nodes[i].RotationKeys);
        free(animation->Nodes[i].ScalingKeys);
    }
    free(animation->Nodes);
    free(animation->Name);
    free(animation);
}

Node *getAnimationNode(char *nodeName, Node *rootNode) {
    if (!strcmp(rootNode->Name, nodeName))
        return rootNode;
    for (int i = 0; i < rootNode->ChildCount; i++) {
        Node *childNode = (Node *)rootNode->Children[i];
        Node *returnValue = getAnimationNode(nodeName, childNode);
        if (returnValue != NULL)
            return returnValue;
    }
    return NULL;
}
// these two are exactly the same function but because the vector key array is
// more tightly packed it would be problematic to cast the quat array
int getNextIndexVec(float targetTime, AnimationVectorKey *keyArray,
                    int keyCount) {
    int result = 0;
    while (keyArray[result].Time < targetTime) {
        result += 1;
        result %= keyCount;
    }
    return result;
}
int getNextIndexQuat(float targetTime, AnimationQuaternionKey *keyArray,
                     int keyCount) {
    int result = 0;
    while (keyArray[result].Time < targetTime) {
        result += 1;
        result %= keyCount;
    }
    return result;
}
