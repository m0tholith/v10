#include "v10/model.h"

#include "v10/material.h"
#include "v10/mesh.h"
#include "v10/node.h"
#include "v10/texture.h"
#include <assimp/material.h>
#include <assimp/matrix4x4.h>
#include <assimp/mesh.h>

#include "v10/lib/glad/glad.h"
#include <GL/gl.h>
#include <assimp/postprocess.h>
#include <cglm/struct/mat4.h>
#include <cglm/struct/vec2.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Mesh *processMesh(struct aiMesh *mesh);
struct Node *processNode(struct aiNode *node, struct Node *parentNode);
void processNodeArray(struct NodeEntry *nodeArray, struct Node *rootNode,
                      int *index, int parentIndex);
Material *importAssimpMaterials(Model *model, struct aiMaterial *aiMat);

Model *modelLoad(const char *_modelPath, unsigned int options) {
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
        exit(1);
    }
    if (scene->mNumSkeletons > 1) {
        printf("model import error: more than 1 skeleton");
        exit(1);
    }

    Model *model = malloc(sizeof(Model));

    model->WorldFromModel = GLMS_MAT4_IDENTITY;

    model->MeshCount = scene->mNumMeshes;
    model->Meshes = malloc(model->MeshCount * sizeof(struct Mesh *));
    for (int i = 0; i < scene->mNumMeshes; i++) {
        model->Meshes[i] = processMesh(scene->mMeshes[i]);
    }

    model->MaterialCount = scene->mNumMaterials;
    model->Materials = malloc(model->MaterialCount * sizeof(Material *));

    model->TextureCount = scene->mNumTextures;
    model->Textures = malloc(model->TextureCount * sizeof(Texture *));
    for (int i = 0; i < model->TextureCount; i++) {
        model->Textures[i] =
            textureCreate(scene->mTextures[i]->mFilename.data, true);
    }

    if (!(options & MODELOPTS_DONT_IMPORT_MATERIALS)) {
        for (int i = 0; i < model->MaterialCount; i++)
            model->Materials[i] =
                importAssimpMaterials(model, scene->mMaterials[i]);
    }

    struct Node *rootNode = processNode(scene->mRootNode, NULL);
    model->NodeCount = nodeChildCount(rootNode) + 1; // +1 for root node
    model->NodeEntries = malloc(model->NodeCount * sizeof(struct NodeEntry));
    int index = 0;
    processNodeArray(model->NodeEntries, rootNode, &index, -1);

    for (int meshId = 0; meshId < model->MeshCount; meshId++) {
        struct Mesh *mesh = model->Meshes[meshId];
        struct Vertex *vertices = mesh->Vertices;
        struct aiMesh *assimpMesh = scene->mMeshes[meshId];

        for (int aiBoneId = 0; aiBoneId < assimpMesh->mNumBones; aiBoneId++) {
            struct aiBone *assimpBone = assimpMesh->mBones[aiBoneId];

            for (int nodeId = 0; nodeId < model->NodeCount; nodeId++) {
                struct Node *node = model->NodeEntries[nodeId].Node;

                if (strncmp(node->Name, assimpBone->mNode->mName.data,
                            assimpBone->mNode->mName.length) == 0) {
                    struct aiVertexWeight *assimpWeights = assimpBone->mWeights;

                    for (int weightId = 0; weightId < assimpBone->mNumWeights;
                         weightId++) {
                        struct aiVertexWeight weight = assimpWeights[weightId];

                        for (int i = 0; i < MAX_BONE_INFLUENCE; i++) {
                            struct Vertex *vertex = &vertices[weight.mVertexId];
                            if (vertex->BoneIDs[i] == -1) {
                                vertex->BoneIDs[i] = nodeId;
                                vertex->BoneWeights[i] = weight.mWeight;
                                break;
                            }
                        }
                    }
                    break;
                }
            }
        }

        meshSendData(mesh);
    }

    model->AnimationCount = scene->mNumAnimations;
    model->Animations = malloc(model->AnimationCount * sizeof(Animation *));
    for (int i = 0; i < model->AnimationCount; i++) {
        model->Animations[i] =
            animationCreate(scene, scene->mAnimations[i]->mName.data,
                            model->NodeEntries[0].Node);
    }

    model->TexDepthShader =
        shaderCreate("depth_tex2d.vert", "", "depth_tex2d.frag");
    model->CubemapDepthShader = shaderCreate(
        "depth_cubemap.vert", "depth_cubemap.geom", "depth_cubemap.frag");

    model->OnDelete = &_modelDelete;

    free(modelFile);
    aiReleaseImport(scene);

    return model;
}
void modelSetMaterials(Model *model, int materialCount, ...) {
    va_list materials;
    va_start(materials, materialCount);
    for (int i = 0; i < materialCount; i++) {
        model->Materials[i] = va_arg(materials, Material *);
    }
    va_end(materials);
}
void modelSetDefaultMaterial(Model *model, Material *material) {
    for (int i = 0; i < model->MaterialCount; i++) {
        model->Materials[i] = material;
    }
}
void modelPreRender(void *_model) {
    Model *model = (Model *)_model;
    for (int i = 0; i < model->NodeCount; i++) {
        struct NodeEntry *nodeEntry = &model->NodeEntries[i];
        mat4s worldFromParent;
        if (i == 0)
            worldFromParent = model->WorldFromModel;
        else
            worldFromParent =
                model->NodeEntries[nodeEntry->ParentIndex].WorldFromLocal;

        nodeEntry->WorldFromLocal =
            glms_mat4_mul(worldFromParent, nodeEntry->Node->ParentFromLocal);
    }
}
void modelRender(Model *model) {
    for (int i = 0; i < model->NodeCount; i++) {
        struct NodeEntry *nodeEntry = &model->NodeEntries[i];
        mat4s worldFromParent;
        if (i == 0)
            worldFromParent = model->WorldFromModel;
        else
            worldFromParent =
                model->NodeEntries[nodeEntry->ParentIndex].WorldFromLocal;

        nodeRender(worldFromParent, nodeEntry->Node, model->Meshes,
                   model->Materials);
    }
}
void modelFree(Model *model) { model->OnDelete(model); }

void _modelDelete(void *_model) {
    Model *model = (Model *)_model;
    nodeFree(model->NodeEntries[0].Node);
    free(model->NodeEntries);
    free(model->Materials);
    for (int i = 0; i < model->MeshCount; i++) {
        meshFree(model->Meshes[i]);
    }
    free(model->Meshes);
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

struct Mesh *processMesh(struct aiMesh *mesh) {
    struct Vertex *vertices =
        malloc(mesh->mNumVertices * sizeof(struct Vertex));
    uint32_t *indices = malloc(mesh->mNumFaces * 3 * sizeof(uint32_t));
    for (int i = 0; i < mesh->mNumVertices; i++) {
        struct Vertex v = {0};

        v.Position = (vec3s){
            {mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z}};
        v.Normal = (vec3s){
            {mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z}};
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
        for (int i = 0; i < MAX_BONE_INFLUENCE; i++) {
            v.BoneIDs[i] = -1;
            v.BoneWeights[i] = 0;
        }
        vertices[i] = v;
    }
    for (int i = 0; i < mesh->mNumFaces; i++) {
        struct aiFace face = mesh->mFaces[i];
        for (int j = 0; j < 3; j++) {
            indices[3 * i + j] = face.mIndices[j];
        }
    }

    struct Mesh *newMesh =
        meshLoad(vertices, indices, mesh->mNumVertices, mesh->mNumFaces * 3);
    newMesh->MaterialIndex = mesh->mMaterialIndex;
    return newMesh;
}

mat4s aiMatrixToGLMS(struct aiMatrix4x4 aiMat) {
    mat4s mat;
    mat.raw[0][0] = aiMat.a1;
    mat.raw[0][2] = aiMat.c1;
    mat.raw[0][1] = aiMat.b1;
    mat.raw[0][3] = aiMat.d1;
    mat.raw[1][0] = aiMat.a2;
    mat.raw[1][1] = aiMat.b2;
    mat.raw[1][2] = aiMat.c2;
    mat.raw[1][3] = aiMat.d2;
    mat.raw[2][0] = aiMat.a3;
    mat.raw[2][1] = aiMat.b3;
    mat.raw[2][2] = aiMat.c3;
    mat.raw[2][3] = aiMat.d3;
    mat.raw[3][0] = aiMat.a4;
    mat.raw[3][1] = aiMat.b4;
    mat.raw[3][2] = aiMat.c4;
    mat.raw[3][3] = aiMat.d4;
    return mat;
}
struct Node *processNode(struct aiNode *node, struct Node *parentNode) {
    if (parentNode == NULL) {
        printf("Root node is \'%s\'\n", node->mName.data);
    }
    struct Node *newNode = nodeCreate(parentNode, node->mNumChildren);
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
void processNodeArray(struct NodeEntry *nodeArray, struct Node *rootNode,
                      int *indexPtr, int parentIndex) {
    int index = (*indexPtr)++;
    nodeArray[index].Node = rootNode;
    nodeArray[index].ParentIndex = parentIndex;

    if (parentIndex == -1)
        nodeArray[index].WorldFromLocal = GLMS_MAT4_IDENTITY;
    else
        nodeArray[index].WorldFromLocal = glms_mat4_mul(
            nodeArray[parentIndex].WorldFromLocal, rootNode->ParentFromLocal);

    for (int i = 0; i < rootNode->ChildCount; i++) {
        processNodeArray(nodeArray, rootNode->Children[i], indexPtr, index);
    }
}

struct Node *searchForNode(char *name, struct Node *rootNode) {
    if (!strcmp(name, rootNode->Name))
        return rootNode;
    for (int i = 0; i < rootNode->ChildCount; i++) {
        struct Node *result = searchForNode(name, rootNode->Children[i]);
        if (result != NULL)
            return result;
    }
    return NULL;
}
Material *importAssimpMaterials(Model *model, struct aiMaterial *aiMat) {
    Material *material = materialCreate(0, 0);

    struct aiColor4D color;
    float num;
    struct aiString texPath;
    enum aiTextureMapping texMapping;
    unsigned int texUvIndex;
    ai_real texBlend;
    enum aiTextureOp texOp;
    enum aiTextureMapMode texMapMode;
    unsigned int texFlags;
    char *texPathStr;
    Texture *texture;

    if (aiGetMaterialColor(aiMat, AI_MATKEY_COLOR_AMBIENT, &color) ==
        AI_SUCCESS) {
        vec3s *ambient = malloc(sizeof(vec3s));
        ambient->r = color.r;
        ambient->g = color.g;
        ambient->b = color.b;
        materialAddProperty(
            material, materialPropertyCreate("_material.ambient", MATTYPE_VEC3,
                                             (void *)ambient));
    }
    if (aiGetMaterialColor(aiMat, AI_MATKEY_COLOR_DIFFUSE, &color) ==
        AI_SUCCESS) {
        vec3s *diffuse = malloc(sizeof(vec3s));
        diffuse->r = color.r;
        diffuse->g = color.g;
        diffuse->b = color.b;
        materialAddProperty(
            material, materialPropertyCreate("_material.diffuse", MATTYPE_VEC3,
                                             (void *)diffuse));
    }
    if (aiGetMaterialColor(aiMat, AI_MATKEY_COLOR_SPECULAR, &color) ==
        AI_SUCCESS) {
        vec3s *specular = malloc(sizeof(vec3s));
        specular->r = color.r;
        specular->g = color.g;
        specular->b = color.b;
        materialAddProperty(
            material, materialPropertyCreate("_material.specular", MATTYPE_VEC3,
                                             (void *)specular));
    }
    if (aiGetMaterialFloat(aiMat, AI_MATKEY_SHININESS, &num) == AI_SUCCESS) {
        float *shininess = malloc(sizeof(float));
        *shininess = num;
        materialAddProperty(
            material, materialPropertyCreate("_material.shininess",
                                             MATTYPE_FLOAT, (void *)shininess));
    }
    if (aiGetMaterialFloat(aiMat, AI_MATKEY_SHININESS_STRENGTH, &num) ==
        AI_SUCCESS) {
        float *specular_strength = malloc(sizeof(float));
        *specular_strength = num;
        materialAddProperty(
            material,
            materialPropertyCreate("_material.specular_strength", MATTYPE_FLOAT,
                                   (void *)specular_strength));
    }

    if (aiGetMaterialTexture(aiMat, aiTextureType_DIFFUSE, 0, &texPath,
                             &texMapping, &texUvIndex, &texBlend, &texOp,
                             &texMapMode, &texFlags) == AI_SUCCESS &&
        texPath.length > 0) {
        texPathStr = texPath.data;
        if (texPathStr[0] == '*') {
            int idx = atoi(&texPathStr[1]);
            printf("using texture %d\n", idx);
            texture = model->Textures[idx];
        } else
            texture = textureCreate(texPathStr, 0);
    } else
        texture = textureCreate(TEXTURE_DEFAULT, 0);
    materialAddProperty(
        material,
        materialPropertyCreate("_material.diffuse_tex", MATTYPE_TEXTURE2D,
                               (void *)materialTextureDataCreate(texture, 0)));

    return material;
}
