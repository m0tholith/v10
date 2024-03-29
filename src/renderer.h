#ifndef BASIC_OBJECT_H
#define BASIC_OBJECT_H

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
} BasicObject;

BasicObject *basicObjectInit(float vertices[], int indices[], int vertexCount,
                             int indexCount, const char *textureFile);
void basicObjectDraw(BasicObject *object);

#endif
