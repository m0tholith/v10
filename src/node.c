#include "node.h"

#include <cglm/struct/mat4.h>
#include <stdlib.h>

Node *nodeCreate(Node *parent, int childCount) {
    Node *node = malloc(sizeof(Node));
    node->Transform = GLMS_MAT4_IDENTITY;
    node->Parent = parent;
    node->ChildCount = childCount;
    node->Children = malloc(childCount * sizeof(Node *));
    return node;
}
mat4s getFinalTransformation(Node *node);
void nodeRender(Node *node, Mesh *meshArray, Material **materialArray) {
    mat4s transformation = getFinalTransformation(node);
    for (int i = 0; i < node->MeshCount; i++) {
        int index = (&meshArray[node->Meshes[i]])->MaterialIndex;
        Material *material = materialArray[index];
        meshRender(&meshArray[node->Meshes[i]], transformation,
                   material->Shader);
    }
    for (int i = 0; i < node->ChildCount; i++) {
        nodeRender(node->Children[i], meshArray, materialArray);
    }
}
void nodeFree(Node *node) {
    for (int i = 0; i < node->ChildCount; i++) {
        nodeFree(node->Children[i]);
    }
    free(node->Children);
    free(node);
}

mat4s getFinalTransformation(Node *node) {
    if (!node->Parent)
        return node->Transform;
    return glms_mat4_mul(getFinalTransformation(node->Parent), node->Transform);
}
