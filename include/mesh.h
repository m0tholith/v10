#ifndef MESH_H
#define MESH_H

#include <cglm/types-struct.h>
#include <stdint.h>

#define MAX_BONE_INFLUENCE 4

struct Vertex {
    vec3s Position;
    vec3s Normal;
    vec2s TexCoords;
    vec3s Color;
    int BoneIDs[MAX_BONE_INFLUENCE];
    float BoneWeights[MAX_BONE_INFLUENCE];
};
struct Mesh {
    int VertexCount;
    struct Vertex *Vertices;

    int IndexCount;
    uint32_t *Indices;

    uint32_t VAO, VBO, EBO;
    int MaterialIndex;
};

/// creates OpenGL buffers with supplied vertices and indices. free with
/// `meshFree`
struct Mesh *meshLoad(struct Vertex *vertices, uint32_t *indices,
                      int vertexCount, int indexCount);
void meshSendData(struct Mesh *mesh);
/// overrides all meshes' draw calls to use specific shader
///
/// set to -1 to remove override
void meshOverrideShaders(uint32_t shader);
void meshRender(struct Mesh *mesh, mat4s worldFromModel, uint32_t shader);
void meshFree(struct Mesh *mesh);

#endif // !MESH_H
