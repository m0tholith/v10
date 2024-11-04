#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <stdint.h>

enum FramebufferOpts {
    FRAMEBUF_DEPTH = (1 << 0),
    FRAMEBUF_RGB = (1 << 1),

    FRAMEBUF_TEX2D = (1 << 2),
    FRAMEBUF_CUBEMAP = (1 << 3),
};

typedef struct {
    uint32_t FBO;
    uint32_t Target;
    int x;
    int y;
} Framebuffer;
Framebuffer *framebufferCreate(int x, int y, enum FramebufferOpts options);
void framebufferBind(Framebuffer *framebuffer);
void framebufferResetBind();
void framebufferFree(Framebuffer *framebuffer);

#endif // !FRAMEBUFFER_H
