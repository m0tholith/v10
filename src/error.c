#include "error.h"

#include "glad/glad.h"
#include <GL/gl.h>
#include <stdio.h>

void errorHandle() {
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        fprintf(stderr, "gl error 0x%04X\n", err);
    }
}
