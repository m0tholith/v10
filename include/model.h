#ifndef MODEL_H
#define MODEL_H

#include "mesh.h"

#include "assimp/cimport.h"
#include "assimp/postprocess.h"
#include "assimp/scene.h"

typedef struct {
    Mesh *Meshes;
    unsigned int MeshCount;
    const struct aiScene *Scene;
} Model;

Model *modelLoad(const char *modelFilename);
void modelRender(Model *model, unsigned int shader);
void modelDelete(Model *model);

#endif // !MODEL_H
