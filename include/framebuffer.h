#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <stdint.h>

enum FramebufferType {
    FRAMEBUF_DEPTH = (1 << 0),
    FRAMEBUF_RGB = (1 << 1),
};

typedef struct {
    uint32_t FBO;
    uint32_t Texture;
    int x;
    int y;
} Framebuffer;
Framebuffer *framebufferCreate(int x, int y, enum FramebufferType type);
void framebufferBind(Framebuffer *framebuffer);
void framebufferResetBind();
void framebufferFree(Framebuffer *framebuffer);

#endif // !FRAMEBUFFER_H
