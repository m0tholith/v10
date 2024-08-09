#include "model.h"

#include "material.h"
#include "mesh.h"
#include "node.h"
#include "texture.h"

#include "glad/glad.h"
#include <GL/gl.h>
#include <assimp/postprocess.h>
#include <cglm/struct/mat4.h>
#include <cglm/struct/vec2.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Mesh *processMesh(struct aiMesh *mesh, const struct aiScene *scene);
Node *processNode(struct aiNode *node, Node *parentNode);

Model *modelLoad(const char *_modelPath) {
    char *modelFile = malloc(strlen(_modelPath) + sizeof(MODELS_PATH));
    strcpy(modelFile, MODELS_PATH);
    strcat(modelFile, _modelPath);

    const struct aiScene *scene = aiImportFile(
        modelFile, aiProcess_Triangulate | aiProcess_FlipUVs |
                       aiProcess_GenNormals | aiProcess_SplitLargeMeshes |
                       aiProcess_PopulateArmatureData);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
        !scene->mRootNode) {
        printf("assimp error: %s", aiGetErrorString());
    }

    Model *model = malloc(sizeof(Model));

    model->WorldTransform = GLMS_MAT4_IDENTITY;

    model->MeshCount = scene->mNumMeshes;
    model->Meshes = malloc(model->MeshCount * sizeof(struct Mesh *));
    for (uint32_t i = 0; i < scene->mNumMeshes; i++) {
        model->Meshes[i] = processMesh(scene->mMeshes[i], scene);
        meshSendData(model->Meshes[i]);
    }

    model->MaterialCount = scene->mNumMaterials;
    model->Materials = malloc(model->MaterialCount * sizeof(Material *));

    model->TextureCount = scene->mNumTextures;
    model->Textures = malloc(model->TextureCount * sizeof(Texture *));
    for (int i = 0; i < model->TextureCount; i++) {
        model->Textures[i] = textureCreate(scene->mTextures[i]->mFilename.data,
                                           TEXTURETYPE_RGB, true);
    }

    model->RootNode = processNode(scene->mRootNode, NULL);

    model->AnimationCount = scene->mNumAnimations;
    model->Animations = malloc(model->AnimationCount * sizeof(Animation *));
    for (int i = 0; i < model->AnimationCount; i++) {
        model->Animations[i] = animationCreate(
            scene, scene->mAnimations[i]->mName.data, model->RootNode);
    }

    model->OnDelete = &_modelDelete;

    free(modelFile);
    aiReleaseImport(scene);

    return model;
}
void modelSetMaterials(Model *model, ...) {
    va_list materials;
    va_start(materials, model->MaterialCount);
    for (int i = 0; i < model->MaterialCount; i++) {
        model->Materials[i] = va_arg(materials, Material *);
    }
    va_end(materials);
}
void modelSetDefaultMaterial(Model *model, Material *material) {
    for (int i = 0; i < model->MaterialCount; i++) {
        model->Materials[i] = material;
    }
}
void modelRender(Model *model) {
    nodeRender(model->WorldTransform, model->RootNode, model->Meshes,
               model->Materials);
}
void modelFree(Model *model) { (model->OnDelete)(model); }
void _modelDelete(void *_model) {
    Model *model = (Model *)_model;
    nodeFree(model->RootNode);
    free(model->Materials);
    for (int i = 0; i < model->MeshCount; i++) {
        meshFree(model->Meshes[i]);
    }
    free(model->Meshes);
    for (int i = 0; i < model->TextureCount; i++) {
        textureFree(model->Textures[i]);
    }
    free(model->Textures);
    for (int i = 0; i < model->AnimationCount; i++) {
        animationFree(model->Animations[i]);
    }
    free(model->Animations);
    free(model);
}
void _modelFreeMaterials(void *_model) {
    Model *model = (Model *)_model;
    for (int i = 0; i < model->MaterialCount; i++) {
        materialFree(model->Materials[i]);
    }
}

struct Mesh *processMesh(struct aiMesh *mesh, const struct aiScene *scene) {
    struct Vertex *vertices =
        malloc(mesh->mNumVertices * sizeof(struct Vertex));
    uint32_t *indices = malloc(mesh->mNumFaces * 3 * sizeof(uint32_t));
    for (uint32_t i = 0; i < mesh->mNumVertices; i++) {
        struct Vertex v = {0};

        v.Position = (vec3s){
            {mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z}};
        v.Normal = (vec3s){
            {mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z}};
        if (mesh->mTextureCoords[0])
            v.TexCoords = (vec2s){
                {mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y}};
        else
            v.TexCoords = GLMS_VEC2_ONE;
        if (mesh->mColors[0])
            v.Color = (vec3s){{
                mesh->mColors[0][i].r,
                mesh->mColors[0][i].g,
                mesh->mColors[0][i].b,
            }};
        else
            v.Color = GLMS_VEC3_ONE;
        for (int j = 0; j < MAX_BONE_INFLUENCE; j++) {
            v.BoneIDs[j] = -1;
            v.Weights[j] = 0;
        }
        vertices[i] = v;
    }
    for (uint32_t i = 0; i < mesh->mNumFaces; i++) {
        struct aiFace face = mesh->mFaces[i];
        for (uint32_t j = 0; j < 3; j++) {
            indices[3 * i + j] = face.mIndices[j];
        }
    }

    struct Mesh *newMesh =
        meshLoad(vertices, indices, mesh->mNumVertices, mesh->mNumFaces * 3);
    newMesh->MaterialIndex = mesh->mMaterialIndex;
    return newMesh;
}

mat4s aiMatrixToGLMS(struct aiMatrix4x4 aiMat) {
    aiTransposeMatrix4(&aiMat);
    return *(mat4s *)&aiMat;
}
Node *processNode(struct aiNode *node, Node *parentNode) {
    if (parentNode == NULL) {
        printf("Root node is \'%s\'\n", node->mName.data);
    }
    Node *newNode = nodeCreate(parentNode, node->mNumChildren);
    newNode->ParentFromLocal = aiMatrixToGLMS(node->mTransformation);
    newNode->MeshCount = node->mNumMeshes;
    newNode->Meshes = malloc(node->mNumMeshes * sizeof(uint32_t));
    memcpy(newNode->Meshes, node->mMeshes, node->mNumMeshes * sizeof(uint32_t));
    newNode->Name = malloc(node->mName.length * sizeof(char) + 1);
    strcpy(newNode->Name, node->mName.data);
    for (int i = 0; i < node->mNumChildren; i++) {
        newNode->Children[i] = processNode(node->mChildren[i], newNode);
    }
    return newNode;
}

Node *searchForNode(char *name, Node *rootNode) {
    if (!strcmp(name, rootNode->Name))
        return rootNode;
    for (int i = 0; i < rootNode->ChildCount; i++) {
        Node *result = searchForNode(name, rootNode->Children[i]);
        if (result != NULL)
            return result;
    }
    return NULL;
}
