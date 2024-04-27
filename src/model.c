#include "model.h"

#include "cglm/include/cglm/struct/vec2.h"
#include <stdio.h>

Mesh *processMesh(struct aiMesh *mesh, const struct aiScene *scene);

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

    return model;
}
void modelRender(Model *model, unsigned int shader) {
    for (unsigned int i = 0; i < model->MeshCount; i++) {
        meshRender(&model->Meshes[i], shader);
    }
}
void modelTexture(Model *model, unsigned int texture, int *meshIndices,
                  int meshCount) {
    if (meshIndices == NULL || meshCount == -1) {
        for (int i = 0; i < model->MeshCount; i++) {
            model->Meshes[i].Texture = texture;
        }
    } else {
        for (int i = 0; i < meshCount; i++) {
            model->Meshes[meshIndices[i]].Texture = texture;
        }
    }
}
void modelDelete(Model *model) {
    for (int i = 0; i < model->MeshCount; i++) {
        meshDelete(&model->Meshes[i]);
    }
    aiReleaseImport(model->Scene);
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
                      mesh->mNumFaces * 3, 0);
}
