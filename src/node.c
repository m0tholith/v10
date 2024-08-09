#include "node.h"

#include "material.h"

#include <cglm/struct/mat4.h>
#include <stdio.h>
#include <stdlib.h>

void printParents(Node *node);
void printMat(mat4s *matrix);

Node *nodeCreate(Node *parent, int childCount) {
    Node *node = malloc(sizeof(Node));
    node->ParentFromLocal = GLMS_MAT4_IDENTITY;
    node->Parent = parent;
    node->ChildCount = childCount;
    node->Children = malloc(childCount * sizeof(Node *));
    return node;
}
void nodeRender(mat4s worldTransform, Node *node, struct Mesh **meshArray,
                Material **materialArray) {
    mat4s transformation =
        glms_mat4_mul(worldTransform, nodeGetParentTransform(node));
    for (int i = 0; i < node->MeshCount; i++) {
        struct Mesh *mesh = meshArray[node->Meshes[i]];
        int index = mesh->MaterialIndex;
        Material *material = materialArray[index];
        materialApplyProperties(material);
        meshRender(mesh, transformation, material->Shader);
    }
    for (int i = 0; i < node->ChildCount; i++) {
        nodeRender(worldTransform, node->Children[i], meshArray, materialArray);
    }
}
mat4s nodeGetParentTransform(Node *node) {
    if (node == NULL)
        return GLMS_MAT4_IDENTITY;
    return glms_mat4_mul(nodeGetParentTransform(node->Parent),
                         node->ParentFromLocal);
}
void nodePrintInfo(Node *node) {
    printParents(node);
    printf("Node's Name = %s\n", node->Name);
    mat4s transform = nodeGetParentTransform(node);
}
void nodeFree(Node *node) {
    for (int i = 0; i < node->ChildCount; i++) {
        nodeFree(node->Children[i]);
    }
    free(node->Children);
    free(node->Name);
    free(node->Meshes);
    free(node);
}

void printParents(Node *node) {
    if (node->Parent == NULL) {
        printf("No parents.\n");
        return;
    }
    printf("Parents: ");
    while (node->Parent != NULL) {
        printf("%s -> ", ((Node *)node->Parent)->Name);
        node = (Node *)node->Parent;
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
