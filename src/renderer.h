#ifndef BASIC_OBJECT_H
#define BASIC_OBJECT_H

#include "cglm/include/cglm/struct.h"

#define objectId unsigned int

typedef struct BasicObject {
    int VertexCount;
    int IndexCount;
    objectId VAO; // vertex array object, stores info about vertex attributes
    objectId VBO; // vertex buffer object, stores info about every vertex
    objectId EBO; // element buffer object; used for storing order of vertices
                  // to render
    objectId ShaderProgram;
    objectId Texture;
    mat4s Transform;
} BasicObject;

BasicObject *basicObjectInit(float vertices[], int indices[], int vertexCount,
                             int indexCount, const char *textureFile,
                             const char *vertexShaderFile,
                             const char *fragmentShaderFile);
void basicObjectDraw(BasicObject *object);

#endif
