#include "shader.h"

#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    if (fread(string, fsize, 1, file) < 1) {
        fprintf(stderr, "couldn't read file \"%s\"\n", fileName);
        exit(1);
    }
    fclose(file);

    string[fsize] = 0;
    return string;
}

uint32_t shaderCreate(const char *_vertexShaderPath,
                      const char *_fragmentShaderPath) {
    char *vertexShaderPath =
        malloc(strlen(_vertexShaderPath) + sizeof(SHADERS_PATH));
    strcpy(vertexShaderPath, SHADERS_PATH);
    strcat(vertexShaderPath, _vertexShaderPath);
    char *fragmentShaderPath =
        malloc(strlen(_fragmentShaderPath) + sizeof(SHADERS_PATH));
    strcpy(fragmentShaderPath, SHADERS_PATH);
    strcat(fragmentShaderPath, _fragmentShaderPath);

    char *vertexShaderContents = readFile(vertexShaderPath);
    const char *vertexShaderSource = vertexShaderContents;
    uint32_t vertexShader = glCreateShader(GL_VERTEX_SHADER);
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
    uint32_t fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, sizeof(infoLog), NULL, infoLog);
        printf("fragment shader could not compile: %s", infoLog);
        exit(EXIT_FAILURE);
    }

    uint32_t shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    int shaderSuccess;
    glGetProgramiv(shaderProgram, GL_VALIDATE_STATUS, &shaderSuccess);
    if (success != GL_TRUE) {
        glGetProgramInfoLog(shaderProgram, sizeof(infoLog), NULL, infoLog);
        printf("shader program could not be linked: %s", infoLog);
        exit(EXIT_FAILURE);
    }
    glUseProgram(shaderProgram);
    glDeleteShader(fragmentShader);
    glDeleteShader(vertexShader);

    free(vertexShaderContents);
    free(fragmentShaderContents);
    free(vertexShaderPath);
    free(fragmentShaderPath);
    return shaderProgram;
}
void shaderFree(uint32_t shader) { glDeleteProgram(shader); }
