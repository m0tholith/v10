#include "animation.h"

#include "assimp/anim.h"
#include "assimp/quaternion.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

vec3s aiVecToVec3s(struct aiVector3D vec) {
    return (vec3s){vec.x, vec.y, vec.z};
}
versors aiQuatToVersors(struct aiQuaternion quat) {
    return (versors){quat.x, quat.y, quat.z, quat.w};
}
Animation *animationCreate(struct aiScene *scene, char *name) {
    int animIndex = 0;
    printf("looking for animation \"%s\"\n", name);
    for (; animIndex < scene->mNumAnimations; animIndex++) {
        printf("comparing \"%s\" to \"%s\"\n", name,
               scene->mAnimations[animIndex]->mName.data);
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

    resultAnimation->NodeCount = animation->mNumChannels;
    resultAnimation->Nodes =
        malloc(animation->mNumChannels * sizeof(AnimationNode));

    for (int i = 0; i < resultAnimation->NodeCount; i++) {
        AnimationNode *node = &resultAnimation->Nodes[i];
        struct aiNodeAnim *channel = animation->mChannels[i];
        node->Name = channel->mNodeName.data;

        node->PositionKeyCount = channel->mNumPositionKeys;
        node->RotationKeyCount = channel->mNumRotationKeys;
        node->ScalingKeyCount = channel->mNumScalingKeys;

        node->PositionKeys =
            malloc(node->PositionKeyCount * sizeof(AnimationVectorKey));
        node->RotationKeys =
            malloc(node->RotationKeyCount * sizeof(AnimationQuaternionKey));
        node->ScalingKeys =
            malloc(node->ScalingKeyCount * sizeof(AnimationVectorKey));
        for (int j = 0; j < node->PositionKeyCount; j++) {
            node->PositionKeys[j].Time = channel->mPositionKeys->mTime;
            node->PositionKeys[j].Value =
                aiVecToVec3s(channel->mPositionKeys->mValue);

            node->RotationKeys[j].Time = channel->mRotationKeys->mTime;
            node->RotationKeys[j].Value =
                aiQuatToVersors(channel->mRotationKeys->mValue);

            node->ScalingKeys[j].Time = channel->mScalingKeys->mTime;
            node->ScalingKeys[j].Value =
                aiVecToVec3s(channel->mScalingKeys->mValue);
        }
    }

    return resultAnimation;
}
void animationPlay(Animation *animation, Node *rootNode) {
    char *nextNodeName = strtok(animation->Name, "/");
    while (!strcmp(nextNodeName, rootNode->Name)) {
        nextNodeName = strtok(animation->Name, "/");
        int foundNext = 0; // just pretend it's a bool
        for (int i = 0; i < rootNode->ChildCount; i++) {
            if (!strcmp(((Node *)rootNode->Children)[i].Name, nextNodeName)) {
                rootNode = &((Node *)rootNode->Children)[i];
                foundNext = 1;
                break;
            }
        }
        if (!foundNext) {
            fprintf(stderr, "couldn't play animation \"%s\": node not found\n",
                    animation->Name);
            exit(EXIT_FAILURE);
        }
    }
    printf("successfully set up animation player \"%s\" (node \"%s\")\n",
           animation->Name, rootNode->Name);
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
