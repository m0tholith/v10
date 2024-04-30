#include "mesh.h"

#include "cglm/struct/mat4.h"
#include "glad/glad.h"
#include "rendering.h"
#include <stdlib.h>

Mesh *meshCreate(Vertex *vertices, unsigned int *indices, int vertexCount,
                 int indexCount, unsigned int texture) {
    Mesh *mesh = malloc(sizeof(Mesh));
    mesh->Vertices = vertices;
    mesh->VertexCount = vertexCount;
    mesh->Indices = indices;
    mesh->IndexCount = indexCount;
    mesh->Texture = texture;
    mesh->Transform = GLMS_MAT4_IDENTITY;

    // generate vertex array object
    glGenVertexArrays(1, &mesh->VAO);
    glBindVertexArray(mesh->VAO);

    // generate vertex buffer object
    glGenBuffers(1, &mesh->VBO);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO);
    glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(Vertex), vertices,
                 GL_STATIC_DRAW); // buffer vertex data

    int attribIdx = 0;
    // position vertex attribute
    glVertexAttribPointer(attribIdx, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void *)offsetof(Vertex, Position));
    glEnableVertexAttribArray(attribIdx++);
    // normal vertex attribute
    glVertexAttribPointer(attribIdx, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void *)offsetof(Vertex, Normal));
    glEnableVertexAttribArray(attribIdx++);
    // texcoords vertex attribute
    glVertexAttribPointer(attribIdx, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void *)offsetof(Vertex, TexCoords));
    glEnableVertexAttribArray(attribIdx++);

    // generate element buffer object
    glGenBuffers(1, &mesh->EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(unsigned int),
                 indices, GL_STATIC_DRAW);

    return mesh;
}
void meshRender(Mesh *mesh, unsigned int shader) {
    glUseProgram(shader);

    // assign texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mesh->Texture);

    // assign mvp matrix
    mat4s mvpMatrix = glms_mat4_mul(glms_mat4_mul(ProjectionMatrix, ViewMatrix),
                                    mesh->Transform);
    glUniformMatrix4fv(glGetUniformLocation(shader, "mvpMatrix"), 1, GL_FALSE,
                       mvpMatrix.raw[0]);

    // render triangles
    glBindVertexArray(mesh->VAO);
    glDrawElements(GL_TRIANGLES, mesh->IndexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
void meshDelete(Mesh *mesh) { free(mesh); }
