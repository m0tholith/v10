#include "node.h"

#include "material.h"

#include <cglm/struct/mat4.h>
#include <stdio.h>
#include <stdlib.h>

void printParents(struct Node *node);
void printMat(mat4s *matrix);

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
void nodePrintInfo(struct Node *node) {
    printParents(node);
    printf("Node's Name = %s\n", node->Name);
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

void printParents(struct Node *node) {
    if (node->Parent == NULL) {
        printf("No parents.\n");
        return;
    }
    printf("Parents: ");
    while (node->Parent != NULL) {
        printf("%s -> ", node->Parent->Name);
        node = node->Parent;
    }
    printf("\b\b\b\b    \n");
}
void printMat(mat4s *matrix) {
    for (int j = 0; j < 4; j++) {
        for (int k = 0; k < 4; k++) {
            if (matrix->raw[k][j] == 0)
                printf(" 0.0000    ");
            else
                printf("%s%.4f    ", matrix->raw[k][j] < 0 ? "" : " ",
                       matrix->raw[k][j]);
        }
        printf("\n");
    }
    printf("\n");
}
