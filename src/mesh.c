#include "mesh.h"

#include "glad/glad.h"
#include "rendering.h"
#include <cglm/struct/mat4.h>
#include <stdlib.h>

struct Mesh *meshLoad(struct Vertex *vertices, unsigned int *indices,
                      int vertexCount, int indexCount) {
    struct Mesh *mesh = malloc(sizeof(struct Mesh));
    mesh->Vertices = vertices;
    mesh->VertexCount = vertexCount;
    mesh->Indices = indices;
    mesh->IndexCount = indexCount;
    mesh->Transform = GLMS_MAT4_IDENTITY;

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

    // generate element buffer object which contains information about the order
    // of vertices to draw
    glGenBuffers(1, &mesh->EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 mesh->IndexCount * sizeof(unsigned int), mesh->Indices,
                 GL_STATIC_DRAW);
}
void meshRender(struct Mesh *mesh, mat4s transformation, unsigned int shader) {
    glUseProgram(shader);

    // assign projectionFromModel matrix
    mat4s projectionFromModel = glms_mat4_mul(
        ProjectionMatrix,
        glms_mat4_mul(ViewMatrix,
                      glms_mat4_mul(transformation, mesh->Transform)));
    // `projectionFromModel` should probably always be defined and used in the
    // vertex shader
    glUniformMatrix4fv(glGetUniformLocation(shader, "projectionFromModel"), 1,
                       GL_FALSE, projectionFromModel.raw[0]);

    // render triangles
    glBindVertexArray(mesh->VAO);
    glDrawElements(GL_TRIANGLES, mesh->IndexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
void meshFree(struct Mesh *mesh) {
    free(mesh->Vertices);
    free(mesh->Indices);
    free(mesh);
}
