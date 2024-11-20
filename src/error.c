#include "error.h"

#include "glad/glad.h"
#include <GL/gl.h>
#include <stdio.h>

char *types[] = {"ERROR",       "DEPRECATED_BEHAVIOR", "UNDEFINED_BEHAVIOR",
                 "PORTABILITY", "PERFORMANCE",         "MARKER",
                 "PUSH_GROUP",  "POP_GROUP",           "OTHER"};

void errorHandle(GLenum source, GLenum type, GLuint id, GLenum severity,
                 GLsizei length, const GLchar *message, const void *userParam) {
    char *sourceString;
    switch (source) {
    case GL_DEBUG_SOURCE_API:
        sourceString = "API";
        break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
        sourceString = "WINDOW_SYSTEM";
        break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER:
        sourceString = "SHADER_COMPILER";
        break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:
        sourceString = "THIRD_PARTY";
        break;
    case GL_DEBUG_SOURCE_APPLICATION:
        sourceString = "APPLICATION";
        break;
    case GL_DEBUG_SOURCE_OTHER:
        sourceString = "OTHER";
        break;
    default:
        sourceString = "????";
        break;
    }
    char *typeString;
    switch (type) {
    case GL_DEBUG_TYPE_ERROR:
        typeString = "ERROR";
        break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        typeString = "DEPRECATED_BEHAVIOR";
        break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        typeString = "UNDEFINED_BEHAVIOR";
        break;
    case GL_DEBUG_TYPE_PORTABILITY:
        typeString = "PORTABILITY";
        break;
    case GL_DEBUG_TYPE_PERFORMANCE:
        typeString = "PERFORMANCE";
        break;
    case GL_DEBUG_TYPE_MARKER:
        typeString = "MARKER";
        break;
    case GL_DEBUG_TYPE_PUSH_GROUP:
        typeString = "PUSH_GROUP";
        break;
    case GL_DEBUG_TYPE_POP_GROUP:
        typeString = "POP_GROUP";
        break;
    case GL_DEBUG_TYPE_OTHER:
        typeString = "OTHER";
        break;
    default:
        typeString = "????";
        break;
    }
    switch (severity) {
    case GL_DEBUG_SEVERITY_NOTIFICATION:
        fprintf(stderr, "GL notification from %s (type %s):\n\t%s\n",
                sourceString, typeString, message);
        break;
    case GL_DEBUG_SEVERITY_LOW:
        fprintf(stderr, "GL low severity from %s (type %s):\n\t%s\n",
                sourceString, typeString, message);
        break;
    case GL_DEBUG_SEVERITY_MEDIUM:
        fprintf(stderr, "GL medium severity from %s (type %s):\n\t%s\n",
                sourceString, typeString, message);
        break;
    case GL_DEBUG_SEVERITY_HIGH:
        fprintf(stderr, "GL high severity from %s (type %s):\n\t%s\n",
                sourceString, typeString, message);
        break;
    default:
        break;
    }
}

void errorInit() {
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(errorHandle, NULL);

    printf("Enabled error/debug output.\n");
}
