#ifndef NODE_H
#define NODE_H

#include "cglm/types-struct.h"
#include "material.h"
#include "mesh.h"

/// to be used with the model class
typedef struct {
    mat4s ParentFromLocal;
    Material Material;

    int MeshCount;
    uint32_t *Meshes;

    void *Parent;

    int ChildCount;
    void **Children;

    char *Name;
} Node;

/// free with `nodeFree`
Node *nodeCreate(Node *parent, int childCount);
void nodeRender(mat4s worldTransform, Node *node, struct Mesh **meshArray,
                Material **materialArray);
mat4s nodeGetParentTransform(Node *node);
void nodePrintInfo(Node *node);
void nodeFree(Node *node);

#endif // !NODE_H
