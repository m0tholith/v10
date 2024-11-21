#ifndef NODE_H
#define NODE_H

#include "v10/material.h"
#include "v10/mesh.h"
#include <cglm/types-struct.h>

/// Only to be used with the model class
struct Node {
    mat4s ParentFromLocal;
    Material Material;

    int MeshCount;
    int *Meshes;

    struct Node *Parent;

    int ChildCount;
    struct Node **Children;

    char *Name;
};

/// free with `nodeFree`
struct Node *nodeCreate(struct Node *parent, int childCount);
/// only renders this specific node
void nodeRender(mat4s worldFromParent, struct Node *node,
                struct Mesh **meshArray, Material **materialArray);
/// gets world transform of parent node
mat4s nodeGetWorldFromLocal(struct Node *node);
int nodeChildCount(struct Node *node);
void nodePrintInfo(struct Node *node);
/// recursively frees node and children
void nodeFree(struct Node *node);

#endif // !NODE_H
