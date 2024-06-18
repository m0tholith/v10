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

    void (*OnDelete)(void *model);

    const struct aiScene *Scene;
} Model;

Model *modelLoad(const char *modelFilename);
// arguments are the materials to be assigned to every slot in `model`, must be
// of count `model->MaterialCount`
void modelSetMaterials(Model *model, ...);
void modelSetDefaultMaterial(Model *model, Material *material);
void modelRender(Model *model);
// complementary to `modelLoad`, calls `model->OnDelete`
void modelFree(Model *model);

// not to be used directly, but by presets and such
void modelDelete(void *_model);
void modelDeleteFreeMaterials(void *_model);

#endif // !MODEL_H
