#ifndef MESH_H
#define MESH_H

#include "cglm/types-struct.h"

#define MAX_BONE_INFLUENCE 4

struct Vertex {
    vec3s Position;
    vec3s Normal;
    vec2s TexCoords;
    vec3s Color;
    int BoneIDs[MAX_BONE_INFLUENCE];
    float Weights[MAX_BONE_INFLUENCE];
};
struct Mesh {
    struct Vertex *Vertices;
    int VertexCount;
    unsigned int *Indices;
    int IndexCount;

    unsigned int VAO, VBO, EBO;
    mat4s Transform;
    int MaterialIndex;
};

/// creates OpenGL buffers with supplied vertices and indices. free with
/// `meshFree`
struct Mesh *meshLoad(struct Vertex *vertices, unsigned int *indices,
                      int vertexCount, int indexCount);
void meshSendData(struct Mesh *mesh);
void meshRender(struct Mesh *mesh, mat4s transformation, unsigned int shader);
void meshFree(struct Mesh *mesh);

#endif // !MESH_H
