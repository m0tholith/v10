#include "shader.h"

#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>

char *readFile(const char *fileName) {
    FILE *file = fopen(fileName, "rb");
    if (file == NULL) {
        printf("file %s could not be read", fileName);
        exit(EXIT_FAILURE);
    }
    fseek(file, 0, SEEK_END);
    long fsize = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *string = malloc(fsize + 1);
    fread(string, fsize, 1, file);
    fclose(file);

    string[fsize] = 0;
    return string;
}

objectId shaderCreate(const char *vertexShaderPath,
                      const char *fragmentShaderPath) {
    char *vertexShaderContents = readFile(vertexShaderPath);
    const char *vertexShaderSource = vertexShaderContents;
    objectId vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // error checking
    int success = 0;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, sizeof(infoLog), NULL, infoLog);
        printf("vertex shader could not compile: %s", infoLog);
        exit(EXIT_FAILURE);
    }

    char *fragmentShaderContents = readFile(fragmentShaderPath);
    const char *fragmentShaderSource = fragmentShaderContents;
    objectId fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, sizeof(infoLog), NULL, infoLog);
        printf("fragment shader could not compile: %s", infoLog);
        exit(EXIT_FAILURE);
    }

    objectId shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, sizeof(infoLog), NULL, infoLog);
        printf("shader program could not be linked: %s", infoLog);
        exit(EXIT_FAILURE);
    }
    glUseProgram(shaderProgram);
    glDeleteShader(fragmentShader);
    glDeleteShader(vertexShader);

    free(vertexShaderContents);
    free(fragmentShaderContents);
    return shaderProgram;
}
