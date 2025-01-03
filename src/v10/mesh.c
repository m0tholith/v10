#include "v10/mesh.h"

#include "v10/lib/glad/glad.h"
#include <cglm/struct/mat3.h>
#include <cglm/struct/mat4.h>
#include <stdlib.h>

Shader *shaderOverride = NULL;

struct Mesh *meshLoad(struct Vertex *vertices, uint32_t *indices,
                      int vertexCount, int indexCount) {
    struct Mesh *mesh = malloc(sizeof(struct Mesh));
    mesh->Vertices = vertices;
    mesh->VertexCount = vertexCount;
    mesh->Indices = indices;
    mesh->IndexCount = indexCount;

    return mesh;
}
void meshSendData(struct Mesh *mesh) {
    // generate vertex array object which contains information about all the
    // vertices
    glGenVertexArrays(1, &mesh->VAO);
    glBindVertexArray(mesh->VAO);

    // generate vertex buffer object which is the actual vertex data
    glGenBuffers(1, &mesh->VBO);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO);
    glBufferData(GL_ARRAY_BUFFER, mesh->VertexCount * sizeof(struct Vertex),
                 mesh->Vertices, GL_STATIC_DRAW);

    int attribIdx = 0;
    // position vertex attribute
    glVertexAttribPointer(attribIdx, 3, GL_FLOAT, GL_FALSE,
                          sizeof(struct Vertex),
                          (void *)offsetof(struct Vertex, Position));
    glEnableVertexAttribArray(attribIdx++);
    // normals
    glVertexAttribPointer(attribIdx, 3, GL_FLOAT, GL_FALSE,
                          sizeof(struct Vertex),
                          (void *)offsetof(struct Vertex, Normal));
    glEnableVertexAttribArray(attribIdx++);
    // texcoords
    glVertexAttribPointer(attribIdx, 2, GL_FLOAT, GL_FALSE,
                          sizeof(struct Vertex),
                          (void *)offsetof(struct Vertex, TexCoords));
    glEnableVertexAttribArray(attribIdx++);
    // vertex colors
    glVertexAttribPointer(attribIdx, 3, GL_FLOAT, GL_FALSE,
                          sizeof(struct Vertex),
                          (void *)offsetof(struct Vertex, Color));
    glEnableVertexAttribArray(attribIdx++);
    // bone IDs
    glVertexAttribIPointer(attribIdx, MAX_BONE_INFLUENCE, GL_INT,
                           sizeof(struct Vertex),
                           (void *)offsetof(struct Vertex, BoneIDs));
    glEnableVertexAttribArray(attribIdx++);
    // bone weights
    glVertexAttribPointer(attribIdx, MAX_BONE_INFLUENCE, GL_FLOAT, GL_FALSE,
                          sizeof(struct Vertex),
                          (void *)offsetof(struct Vertex, BoneWeights));
    glEnableVertexAttribArray(attribIdx++);

    // generate element buffer object which contains information about the order
    // of vertices to draw
    glGenBuffers(1, &mesh->EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->IndexCount * sizeof(uint32_t),
                 mesh->Indices, GL_STATIC_DRAW);
}
void meshOverrideShaders(Shader *shader) { shaderOverride = shader; }
void meshRender(struct Mesh *mesh, mat4s worldFromModel, Shader *shader) {
    if (shaderOverride != NULL)
        shader = shaderOverride;
    glUseProgram(shader->ID);

    glUniformMatrix4fv(shaderGetUniformLocation(shader, "_worldFromModel"), 1,
                       GL_FALSE, worldFromModel.raw[0]);

    // render triangles
    glBindVertexArray(mesh->VAO);
    glDrawElements(GL_TRIANGLES, mesh->IndexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
void meshFree(struct Mesh *mesh) {
    glDeleteBuffers(3, &mesh->VAO);
    free(mesh->Vertices);
    free(mesh->Indices);
    free(mesh);
}
