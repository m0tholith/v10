#ifndef MODEL_H
#define MODEL_H

#define MODELS_PATH "models/"

#include "animation.h"
#include "mesh.h"
#include "node.h"
#include "texture.h"

#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <cglm/struct/mat4.h>

typedef struct {
    mat4s WorldTransform;

    uint32_t MeshCount;
    struct Mesh **Meshes;

    uint32_t MaterialCount;
    Material **Materials;

    uint32_t TextureCount;
    Texture **Textures;

    Node *RootNode;

    uint32_t AnimationCount;
    Animation **Animations;

    void (*OnDelete)(void *model);
} Model;

/// manages rendering a 3d file. free with `modelFree`
Model *modelLoad(const char *modelFilename);
/// @param Material *...: materials to set, needs to be `model->MaterialCount`
/// number of them
void modelSetMaterials(Model *model, ...);
void modelSetDefaultMaterial(Model *model, Material *material);
void modelRender(Model *model);
/// calls `model->OnDelete`
void modelFree(Model *model);

/// not to be used directly, but by presets and such
void _modelDelete(void *_model);
void _modelFreeMaterials(void *_model);

#endif // !MODEL_H
