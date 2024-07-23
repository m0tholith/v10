#ifndef NODE_H
#define NODE_H

#include "material.h"
#include "mesh.h"

/// to be used with the model class
typedef struct {
    mat4s Transform;
    Material Material;

    int MeshCount;
    unsigned int *Meshes;

    void *Parent;

    int ChildCount;
    void **Children;

    char *Name;
} Node;

/// free with `nodeFree`
Node *nodeCreate(Node *parent, int childCount);
void nodeRender(mat4s transform, Node *node, Mesh **meshArray,
                Material **materialArray);
void nodeFree(Node *node);

#endif // !NODE_H
