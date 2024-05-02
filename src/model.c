#include "model.h"

#include "cglm/struct/vec2.h"
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
    for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
        model->Meshes[i] = *processMesh(scene->mMeshes[i], scene);
    }

    model->RootNode = processNode(scene->mRootNode, NULL);

    return model;
}
void modelRender(Model *model, unsigned int shader) {
    for (unsigned int i = 0; i < model->MeshCount; i++) {
        meshRender(&model->Meshes[i], shader);
    }
}
void modelDelete(Model *model) {
    aiReleaseImport(model->Scene);
    nodeDelete(model->RootNode);
    free(model->Meshes);
    free(model);
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
        vertices[i] = v;
    }
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        struct aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < 3; j++) {
            indices[3 * i + j] = face.mIndices[j];
        }
    }

    return meshCreate(vertices, indices, mesh->mNumVertices,
                      mesh->mNumFaces * 3);
}
Node *processNode(struct aiNode *node, Node *parentNode) {
    Node *newNode = nodeCreate(parentNode, node->mNumChildren);
    for (int i = 0; i < node->mNumChildren; i++) {
        newNode->Children[i] = processNode(node->mChildren[i], newNode);
    }
    return newNode;
}
