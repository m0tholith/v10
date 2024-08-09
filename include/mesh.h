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
};
struct Mesh {
    struct Vertex *Vertices;
    int VertexCount;
    uint32_t *Indices;
    int IndexCount;

    uint32_t VAO, VBO, EBO;
    int MaterialIndex;
};

/// creates OpenGL buffers with supplied vertices and indices. free with
/// `meshFree`
struct Mesh *meshLoad(struct Vertex *vertices, uint32_t *indices,
                      int vertexCount, int indexCount);
void meshSendData(struct Mesh *mesh);
void meshRender(struct Mesh *mesh, mat4s worldTransform, uint32_t shader);
void meshFree(struct Mesh *mesh);

#endif // !MESH_H
