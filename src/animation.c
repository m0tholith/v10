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

    resultAnimation->NodeCount = animation->mNumChannels;
    resultAnimation->Nodes =
        malloc(animation->mNumChannels * sizeof(AnimationNode));

    for (int i = 0; i < resultAnimation->NodeCount; i++) {
        AnimationNode *animNode = &resultAnimation->Nodes[i];
        struct aiNodeAnim *channel = animation->mChannels[i];
        animNode->Node = getAnimationNode(channel->mNodeName.data, rootNode);

        animNode->PositionKeyCount = channel->mNumPositionKeys;
        animNode->RotationKeyCount = channel->mNumRotationKeys;
        animNode->ScalingKeyCount = channel->mNumScalingKeys;

        animNode->LastPositionKey = 0;
        animNode->LastRotationKey = 0;
        animNode->LastScalingKey = 0;

        animNode->PositionKeys =
            malloc(animNode->PositionKeyCount * sizeof(AnimationVectorKey));
        animNode->RotationKeys =
            malloc(animNode->RotationKeyCount * sizeof(AnimationQuaternionKey));
        animNode->ScalingKeys =
            malloc(animNode->ScalingKeyCount * sizeof(AnimationVectorKey));

        for (int j = 0; j < animNode->PositionKeyCount; j++) {
            animNode->PositionKeys[j].Time = channel->mPositionKeys[j].mTime;
            animNode->PositionKeys[j].Value =
                aiVecToVec3s(channel->mPositionKeys[j].mValue);
        }
        for (int j = 0; j < animNode->RotationKeyCount; j++) {
            animNode->RotationKeys[j].Time = channel->mRotationKeys[j].mTime;
            animNode->RotationKeys[j].Value =
                aiQuatToVersors(channel->mRotationKeys[j].mValue);
        }
        for (int j = 0; j < animNode->ScalingKeyCount; j++) {
            animNode->ScalingKeys[j].Time = channel->mScalingKeys[j].mTime;
            animNode->ScalingKeys[j].Value =
                aiVecToVec3s(channel->mScalingKeys[j].mValue);
        }
    }

    return resultAnimation;
}
void animationStep(Animation *animation, float deltaTime) {
    animation->Time += deltaTime * animation->TicksPerSec;

    // now here's the licker
    for (int i = 0; i < animation->NodeCount; i++) {
        AnimationNode *animNode = &animation->Nodes[i];
        mat4s newTransform = GLMS_MAT4_IDENTITY;

        if (animNode->PositionKeyCount > 1) {
            int posKeyIndex =
                (animNode->LastPositionKey + 1) % animNode->PositionKeyCount;
            if (animNode->PositionKeys[posKeyIndex].Time <= animation->Time) {
                printf("moving to new poskey %d (time %d) at time %d\n",
                       posKeyIndex, animNode->PositionKeys[posKeyIndex].Time,
                       animation->Time);
                newTransform = glms_translate(
                    newTransform, animNode->PositionKeys[posKeyIndex].Value);
                animNode->LastPositionKey = posKeyIndex;
            } else
                newTransform = glms_translate(
                    newTransform,
                    animNode->PositionKeys[animNode->LastPositionKey].Value);
        } else
            newTransform = glms_translate(
                newTransform,
                animNode->PositionKeys[animNode->LastPositionKey].Value);

        if (animNode->ScalingKeyCount > 1) {
            int scaleKeyIndex =
                (animNode->LastScalingKey + 1) % animNode->ScalingKeyCount;
            if (animNode->ScalingKeys[scaleKeyIndex].Time <= animation->Time) {
                printf("moving to new scalekey %d (time %d) at time %d\n",
                       scaleKeyIndex, animNode->ScalingKeys[scaleKeyIndex].Time,
                       animation->Time);
                newTransform = glms_scale(
                    newTransform, animNode->ScalingKeys[scaleKeyIndex].Value);
                animNode->LastScalingKey = scaleKeyIndex;
            } else
                newTransform = glms_scale(
                    newTransform,
                    animNode->ScalingKeys[animNode->LastScalingKey].Value);
        } else
            newTransform = glms_scale(
                newTransform,
                animNode->ScalingKeys[animNode->LastScalingKey].Value);

        if (animNode->RotationKeyCount > 1) {
            int rotKeyIndex =
                (animNode->LastRotationKey + 1) % animNode->RotationKeyCount;
            if (animNode->RotationKeys[rotKeyIndex].Time <= animation->Time) {
                printf("moving to new rotkey %d (time %d) at time %d\n",
                       rotKeyIndex, animNode->RotationKeys[rotKeyIndex].Time,
                       animation->Time);
                newTransform = glms_quat_rotate(
                    newTransform, animNode->RotationKeys[rotKeyIndex].Value);
                animNode->LastRotationKey = rotKeyIndex;
            } else
                newTransform = glms_quat_rotate(
                    newTransform,
                    animNode->RotationKeys[animNode->LastRotationKey].Value);
        } else
            newTransform = glms_quat_rotate(
                newTransform,
                animNode->RotationKeys[animNode->LastRotationKey].Value);

        animNode->Node->Transform = newTransform;
    }
    animation->Time %= animation->Duration;
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
