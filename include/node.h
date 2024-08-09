#ifndef NODE_H
#define NODE_H

#include "material.h"
#include "mesh.h"
#include <cglm/types-struct.h>

/// to be used with the model class
struct Node {
    mat4s ParentFromLocal;
    Material Material;

    int MeshCount;
    uint32_t *Meshes;

    struct Node *Parent;

    int ChildCount;
    struct Node **Children;

    char *Name;
};

/// free with `nodeFree`
struct Node *nodeCreate(struct Node *parent, int childCount);
void nodeRender(mat4s worldTransform, struct Node *node,
                struct Mesh **meshArray, Material **materialArray);
mat4s nodeGetParentTransform(struct Node *node);
void nodePrintInfo(struct Node *node);
void nodeFree(struct Node *node);

#endif // !NODE_H
