#include "node.h"
#include "material.h"

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
void nodeRender(mat4s transform, Node *node, Mesh **meshArray,
                Material **materialArray) {
    mat4s transformation =
        glms_mat4_mul(transform, nodeGetFinalTransform(node));
    for (int i = 0; i < node->MeshCount; i++) {
        Mesh *mesh = meshArray[node->Meshes[i]];
        int index = mesh->MaterialIndex;
        Material *material = materialArray[index];
        materialApplyProperties(material);
        meshRender(mesh, transformation, material->Shader);
    }
    for (int i = 0; i < node->ChildCount; i++) {
        nodeRender(transform, node->Children[i], meshArray, materialArray);
    }
}
mat4s nodeGetFinalTransform(Node *node) {
    if (node == NULL)
        return GLMS_MAT4_IDENTITY;
    if (node->Parent == NULL)
        return node->Transform;
    return glms_mat4_mul(nodeGetFinalTransform(node->Parent), node->Transform);
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
