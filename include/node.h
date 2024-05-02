#ifndef NODE_H
#define NODE_H

#include "material.h"

typedef struct {
    mat4s Transform;
    Material Material;

    void *Parent;

    int ChildCount;
    void **Children;
} Node;

Node *nodeCreate(Node *parent, int childCount);
void nodeDelete(Node *node);

#endif // !NODE_H
