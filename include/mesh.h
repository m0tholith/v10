#ifndef MESH_H
#define MESH_H

#include "cglm/types-struct.h"

typedef struct {
    vec3s Position;
    vec3s Normal;
    vec2s TexCoords;
} Vertex;
typedef struct {
    Vertex *Vertices;
    int VertexCount;
    unsigned int *Indices;
    int IndexCount;

    unsigned int VAO, VBO, EBO;
    mat4s Transform;
} Mesh;

Mesh *meshCreate(Vertex *vertices, unsigned int *indices, int vertexCount,
                 int indexCount);
void meshRender(Mesh *mesh, unsigned int shader);
void meshDelete(Mesh *mesh);

#endif // !MESH_H
