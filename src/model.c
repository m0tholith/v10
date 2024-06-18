#include "model.h"

#include <cglm/struct/mat4.h>
#include <cglm/struct/vec2.h>
#include <stdarg.h>
#include <stdio.h>

Mesh *processMesh(struct aiMesh *mesh, const struct aiScene *scene);
Node *processNode(struct aiNode *node, Node *parentNode);

Model *modelLoad(const char *modelFilename) {
    const struct aiScene *scene =
        aiImportFile(modelFilename, aiProcess_Triangulate | aiProcess_FlipUVs);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
        !scene->mRootNode) {
        printf("assimp error: %s", aiGetErrorString());
    }

    Model *model = malloc(sizeof(Model));
    model->Scene = scene;

    model->MeshCount = scene->mNumMeshes;
    model->Meshes = malloc(model->MeshCount * sizeof(Mesh));
    Mesh *meshAlloc;
    for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
        meshAlloc = processMesh(scene->mMeshes[i], scene);
        model->Meshes[i] = *meshAlloc;
        free(meshAlloc);
    }

    model->MaterialCount = scene->mNumMaterials;
    model->Materials = malloc(model->MaterialCount * sizeof(Material *));

    model->RootNode = processNode(scene->mRootNode, NULL);
    model->RootNode->Transform = GLMS_MAT4_IDENTITY;

    model->OnDelete = &modelDelete;

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
    nodeRender(model->RootNode, model->Meshes, model->Materials);
}
void modelFree(Model *model) { (model->OnDelete)(model); }
void modelDelete(void *_model) {
    Model *model = (Model *)_model;
    aiReleaseImport(model->Scene);
    nodeFree(model->RootNode);
    free(model->Materials);
    free(model->Meshes);
    free(model);
}
void modelDeleteFreeMaterials(void *_model) {
    Model *model = (Model *)_model;
    for (int i = 0; i < model->MaterialCount - 1; i++) {
        materialFree(model->Materials[i]);
    }
    modelDelete(model);
}

Mesh *processMesh(struct aiMesh *mesh, const struct aiScene *scene) {
    Vertex vertices[mesh->mNumVertices];
    unsigned int indices[mesh->mNumFaces * 3];
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex v = {0};

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
        vertices[i] = v;
    }
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        struct aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < 3; j++) {
            indices[3 * i + j] = face.mIndices[j];
        }
    }

    Mesh *newMesh =
        meshLoad(vertices, indices, mesh->mNumVertices, mesh->mNumFaces * 3);
    newMesh->MaterialIndex = mesh->mMaterialIndex;
    return newMesh;
}

mat4s aiMatrixToGLMS(struct aiMatrix4x4 aiMat) {
    aiTransposeMatrix4(&aiMat);
    return *(mat4s *)&aiMat;
}
Node *processNode(struct aiNode *node, Node *parentNode) {
    Node *newNode = nodeCreate(parentNode, node->mNumChildren);
    newNode->Transform = aiMatrixToGLMS(node->mTransformation);
    newNode->MeshCount = node->mNumMeshes;
    newNode->Meshes = node->mMeshes;
    for (int i = 0; i < node->mNumChildren; i++) {
        newNode->Children[i] = processNode(node->mChildren[i], newNode);
    }
    return newNode;
}
