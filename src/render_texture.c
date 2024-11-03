#include "render_texture.h"

#include "glad/glad.h"
#include "window.h"
#include <GL/gl.h>
#include <stdlib.h>

RenderTexture *renderTextureCreate(int x, int y, enum RenderTextureType type) {
    RenderTexture *renderTexture = malloc(sizeof(RenderTexture));
    renderTexture->x = x;
    renderTexture->y = y;

    glGenFramebuffers(1, &renderTexture->FBO);

    glGenTextures(1, &renderTexture->Texture);
    glBindTexture(GL_TEXTURE_2D, renderTexture->Texture);
    switch (type) {
    case RENDERTEX_DEPTH:
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, x, y, 0,
                     GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        break;
    case RENDERTEX_RGB:
    default:
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x, y, 0, GL_RGB, GL_FLOAT, NULL);
        break;
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    glBindFramebuffer(GL_FRAMEBUFFER, renderTexture->FBO);
    switch (type) {
    case RENDERTEX_DEPTH:
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                               GL_TEXTURE_2D, renderTexture->Texture, 0);
        break;
    case RENDERTEX_RGB:
    default:
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_2D, renderTexture->Texture, 0);
        break;
    }
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return renderTexture;
}
void renderTextureBind(RenderTexture *renderTexture) {
    glViewport(0, 0, renderTexture->x, renderTexture->y);
    glBindFramebuffer(GL_FRAMEBUFFER, renderTexture->FBO);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
void renderTextureResetBind() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void renderTextureFree(RenderTexture *renderTexture) {
    glDeleteBuffers(1, &renderTexture->FBO);
    glDeleteTextures(1, &renderTexture->Texture);
    free(renderTexture);
}
