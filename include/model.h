#ifndef MODEL_H
#define MODEL_H

#include "mesh.h"
#include "node.h"

#include "assimp/cimport.h"
#include "assimp/postprocess.h"
#include "assimp/scene.h"

typedef struct {
    unsigned int MeshCount;
    Mesh *Meshes;

    Node *RootNode;

    const struct aiScene *Scene;
} Model;

Model *modelLoad(const char *modelFilename);
void modelRender(Model *model, unsigned int shader);
void modelDelete(Model *model);

#endif // !MODEL_H
