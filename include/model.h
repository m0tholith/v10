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

    unsigned int MaterialCount;
    Material **Materials;

    Node *RootNode;

    const struct aiScene *Scene;
} Model;

Model *modelLoad(const char *modelFilename);
// arguments are the materials to be assigned to every slot in `model`, must be
// of count `model->MaterialCount`
void modelSetMaterials(Model *model, ...);
void modelSetDefaultMaterial(Model *model, Material *material);
void modelRender(Model *model);
void modelDelete(Model *model);
void modelDeleteFreeMaterials(Model *model);

#endif // !MODEL_H
