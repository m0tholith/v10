#include "renderer.h"

#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <stdio.h>

#define objectId unsigned int

const char *vertexShaderSource =
    "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";
const char *fragmentShaderSource =
    "#version 330 core\n"
    "out vec4 FragColor;\n"
    "uniform vec4 vertexColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 0.063f, 0.941f, 1.0f);\n"
    "}\0";

objectId VAO; // vertex array object, stores info about vertex attributes
objectId VBO; // vertex buffer object, stores info about every vertex
objectId EBO; // element buffer object; used for storing order of vertices
              // to render
objectId shaderProgram;

int rendererInit() {
    // rectangle
    // float vertices[] = {
    //     -0.5f, -0.5f, 0.0f, // bottom left
    //     -0.5f, 0.5f,  0.0f, // top left
    //     0.5f,  0.5f,  0.0f, // top right
    //     0.5f,  -0.5f, 0.0f, // bottom right
    // };
    // objectId indices[] = {
    //     0, 1, 2, // triangle 1
    //     0, 2, 3, // triangle 2
    // };
    // joined at one vertex
    float vertices1[] = {
        -1.0f, -1.0f, 0.0f, // bottom left
        -1.0f, 1.0f,  0.0f, // top left
        0.0f,  -1.0f, 0.0f, // middle
        1.0f,  1.0f,  0.0f, // top right
        1.0f,  -1.0f, 0.0f, // bottom right
    };
    objectId indices[] = {
        0, 1, 2, // triangle 1
        2, 3, 4, // triangle 2
    };

    objectId vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    objectId fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glUseProgram(shaderProgram);
    glDeleteShader(fragmentShader);
    glDeleteShader(vertexShader);

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1), vertices1,
                 GL_STATIC_DRAW); // copy vertex data to VBO1
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                          (void *)0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
                 GL_STATIC_DRAW); // copy index data to EBO

    // glPolygonMode(GL_FRONT_AND_BACK,
    //               GL_LINE); // draw the front and back in wireframe
    glPolygonMode(GL_FRONT_AND_BACK,
                  GL_FILL); // draw the front and back of polygons (filled
                            // in)

    return 0;
}
void rendererDraw() {
    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
