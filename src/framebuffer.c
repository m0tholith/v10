#include "framebuffer.h"

#include "glad/glad.h"
#include <GL/gl.h>
#include <stdlib.h>

enum FramebufferOpts framebufferFormats = FRAMEBUF_DEPTH | FRAMEBUF_RGB;
enum FramebufferOpts framebufferTypes = FRAMEBUF_TEX2D | FRAMEBUF_CUBEMAP;

void createTex2D(Framebuffer *framebuffer, int x, int y,
                 enum FramebufferOpts format);
void createCubemap(Framebuffer *framebuffer, int x, int y,
                   enum FramebufferOpts format);

Framebuffer *framebufferCreate(int x, int y, enum FramebufferOpts options) {
    Framebuffer *framebuffer = malloc(sizeof(Framebuffer));
    framebuffer->x = x;
    framebuffer->y = y;

    glGenFramebuffers(1, &framebuffer->FBO);
    glGenTextures(1, &framebuffer->Target);

    enum FramebufferOpts format = options & framebufferFormats;
    enum FramebufferOpts type = options & framebufferTypes;

    switch (type) {
    case FRAMEBUF_CUBEMAP:
        createCubemap(framebuffer, x, y, format);
        break;
    case FRAMEBUF_TEX2D:
    default:
        createTex2D(framebuffer, x, y, format);
        break;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return framebuffer;
}
void framebufferBind(Framebuffer *framebuffer) {
    glViewport(0, 0, framebuffer->x, framebuffer->y);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->FBO);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
void framebufferResetBind() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }
void framebufferFree(Framebuffer *framebuffer) {
    glDeleteBuffers(1, &framebuffer->FBO);
    glDeleteTextures(1, &framebuffer->Target);
    free(framebuffer);
}

void createTex2D(Framebuffer *framebuffer, int x, int y,
                 enum FramebufferOpts format) {
    glBindTexture(GL_TEXTURE_2D, framebuffer->Target);
    switch (format) {
    case FRAMEBUF_DEPTH:
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, x, y, 0,
                     GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        break;
    case FRAMEBUF_RGB:
    default:
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x, y, 0, GL_RGB, GL_FLOAT, NULL);
        break;
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->FBO);
    switch (format) {
    case FRAMEBUF_DEPTH:
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                               GL_TEXTURE_2D, framebuffer->Target, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        break;
    case FRAMEBUF_RGB:
    default:
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_2D, framebuffer->Target, 0);
        break;
    }
}
void createCubemap(Framebuffer *framebuffer, int x, int y,
                   enum FramebufferOpts format) {
    glBindTexture(GL_TEXTURE_CUBE_MAP, framebuffer->Target);
    switch (format) {
    case FRAMEBUF_DEPTH:
        for (int i = 0; i < 6; i++)
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
                         GL_DEPTH_COMPONENT, x, y, 0, GL_DEPTH_COMPONENT,
                         GL_FLOAT, NULL);
        break;
    case FRAMEBUF_RGB:
    default:
        for (int i = 0; i < 6; i++)
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, x, y, 0,
                         GL_RGB, GL_FLOAT, NULL);
        break;
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);

    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->FBO);
    switch (format) {
    case FRAMEBUF_DEPTH:
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                             framebuffer->Target, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        break;
    case FRAMEBUF_RGB:
    default:
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                             framebuffer->Target, 0);
        break;
    }
}
