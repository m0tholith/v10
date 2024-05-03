#ifndef NODE_H
#define NODE_H

#include "material.h"
#include "mesh.h"

typedef struct {
    mat4s Transform;
    Material Material;

    int MeshCount;
    unsigned int *Meshes;

    void *Parent;

    int ChildCount;
    void **Children;
} Node;

Node *nodeCreate(Node *parent, int childCount);
void nodeRender(Node *node, Mesh *meshArray, Material **materialArray);
void nodeFree(Node *node);

#endif // !NODE_H
