#include "node.h"

#include <stdlib.h>

Node *nodeCreate(Node *parent, int childCount) {
    Node *node = malloc(sizeof(Node));
    node->Parent = parent;
    node->ChildCount = childCount;
    node->Children = malloc(childCount * sizeof(Node *));
    return node;
}
void nodeDelete(Node *node) {
    for (int i = 0; i < node->ChildCount; i++) {
        nodeDelete(node->Children[i]);
    }
    free(node->Children);
    free(node);
}
