#include "v10/node.h"

#include "v10/material.h"

#include <cglm/struct/mat4.h>
#include <stdio.h>
#include <stdlib.h>

struct Node *nodeCreate(struct Node *parent, int childCount) {
    struct Node *node = malloc(sizeof(struct Node));
    node->ParentFromLocal = GLMS_MAT4_IDENTITY;
    node->Parent = parent;
    node->ChildCount = childCount;
    node->Children = malloc(childCount * sizeof(struct Node *));
    return node;
}
void nodeRender(mat4s worldFromParent, struct Node *node,
                struct Mesh **meshArray, Material **materialArray) {
    mat4s worldFromLocal =
        glms_mat4_mul(worldFromParent, node->ParentFromLocal);
    for (int i = 0; i < node->MeshCount; i++) {
        struct Mesh *mesh = meshArray[node->Meshes[i]];
        int index = mesh->MaterialIndex;
        Material *material = materialArray[index];
        materialApplyProperties(material);
        meshRender(mesh, worldFromLocal, material->Shader);
    }
}
mat4s nodeGetWorldFromLocal(struct Node *node) {
    if (node == NULL)
        return GLMS_MAT4_IDENTITY;
    return glms_mat4_mul(nodeGetWorldFromLocal(node->Parent),
                         node->ParentFromLocal);
}
int nodeChildCount(struct Node *node) {
    int result = node->ChildCount;
    for (int i = 0; i < node->ChildCount; i++) {
        result += nodeChildCount(node->Children[i]);
    }
    return result;
}
void nodeFree(struct Node *node) {
    for (int i = 0; i < node->ChildCount; i++) {
        nodeFree(node->Children[i]);
    }
    free(node->Children);
    free(node->Name);
    free(node->Meshes);
    free(node);
}
