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

struct NodeEntry {
    struct Node *Node;
    int ParentIndex;
    mat4s WorldFromLocal;
};
typedef struct {
    mat4s WorldFromModel;

    int MeshCount;
    struct Mesh **Meshes;

    int MaterialCount;
    Material **Materials;

    int TextureCount;
    Texture **Textures;

    int NodeCount;
    struct NodeEntry *NodeEntries;

    int AnimationCount;
    Animation **Animations;

    Shader *TexDepthShader;
    Shader *CubemapDepthShader;

    void (*OnDelete)(void *model);
} Model;

enum ModelLoadOptions {
    MODELOPTS_DONT_IMPORT_MATERIALS = 1,
};

/// Manages rendering a 3D file. Free with `modelFree`
/// @param enum ModelLoadOptions `options` Use MODELOPTS_* options here
Model *modelLoad(const char *modelFilename, unsigned int options);
/// @param Material *... Materials to set, needs to be `model->MaterialCount`
///  number of them
void modelSetMaterials(Model *model, int materialCount, ...);
/// Sets one single material to be used by all meshes.
void modelSetDefaultMaterial(Model *model, Material *material);
void modelPreRender(void *_model);
void modelRender(Model *model);
/// calls `model->OnDelete`
void modelFree(Model *model);

/// not to be used directly, but by presets and such
void _modelDelete(void *_model);
void _modelFreeMaterials(void *_model);

#endif // !MODEL_H
