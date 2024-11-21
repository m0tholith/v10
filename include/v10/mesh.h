#ifndef MESH_H
#define MESH_H

#include "v10/shader.h"
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

/**
 * Creates mesh struct to hold vertex data.
 */
struct Mesh *meshLoad(struct Vertex *vertices, uint32_t *indices,
                      int vertexCount, int indexCount);
/**
 * Creates the GPU buffers containing vertex data.
 */
void meshSendData(struct Mesh *mesh);
/**
 * Sets the shader which will override any shaders supplied in meshRender().
 *
 * Set to NULL to remove this override.
 *
 * @example:
 *  shaderOverride(NULL);
 *  meshRender(myMesh, coolTransform1, redShader); // renders red
 *  shaderOverride(blueShader);
 *  meshRender(myMesh, coolTransform2, redShader); // renders blue
 *  shaderOverride(NULL); // back to normal
 */
void meshOverrideShaders(Shader *shader);
void meshRender(struct Mesh *mesh, mat4s worldFromModel, Shader *shader);
void meshFree(struct Mesh *mesh);

#endif // !MESH_H
