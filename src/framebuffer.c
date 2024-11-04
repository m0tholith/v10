#include "framebuffer.h"

#include "glad/glad.h"
#include "window.h"
#include <GL/gl.h>
#include <stdlib.h>

Framebuffer *framebufferCreate(int x, int y, enum FramebufferType type) {
    Framebuffer *framebuffer = malloc(sizeof(Framebuffer));
    framebuffer->x = x;
    framebuffer->y = y;

    glGenFramebuffers(1, &framebuffer->FBO);

    glGenTextures(1, &framebuffer->Texture);
    glBindTexture(GL_TEXTURE_2D, framebuffer->Texture);
    switch (type) {
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
    switch (type) {
    case FRAMEBUF_DEPTH:
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                               GL_TEXTURE_2D, framebuffer->Texture, 0);
        break;
    case FRAMEBUF_RGB:
    default:
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_2D, framebuffer->Texture, 0);
        break;
    }
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return framebuffer;
}
void framebufferBind(Framebuffer *framebuffer) {
    glViewport(0, 0, framebuffer->x, framebuffer->y);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->FBO);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
void framebufferResetBind() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void framebufferFree(Framebuffer *framebuffer) {
    glDeleteBuffers(1, &framebuffer->FBO);
    glDeleteTextures(1, &framebuffer->Texture);
    free(framebuffer);
}
