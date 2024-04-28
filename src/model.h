#ifndef MODEL_H
#define MODEL_H

#include "mesh.h"

#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

typedef struct Model {
    Mesh *Meshes;
    unsigned int MeshCount;
    const struct aiScene *Scene;
} Model;

Model *modelLoad(const char *modelFilename);
void modelRender(Model *model, unsigned int shader);
// Apply texture to specified meshes.
// To apply to all meshes, set meshCount to -1 or meshIndices to NULL.
void modelTexture(Model *model, unsigned int texture, int *meshIndices, int meshCount);
void modelDelete(Model *model);

#endif // !MODEL_H
