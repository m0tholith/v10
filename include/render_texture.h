#ifndef RENDER_TEXTURE_H
#define RENDER_TEXTURE_H

#include <stdint.h>

enum RenderTextureType {
    RENDERTEX_DEPTH = (1 << 0),
    RENDERTEX_RGB = (1 << 1),
};

typedef struct {
    uint32_t FBO;
    uint32_t Texture;
    int x;
    int y;
} RenderTexture;
RenderTexture *renderTextureCreate(int x, int y, enum RenderTextureType type);
void renderTextureBind(RenderTexture *renderTexture);
void renderTextureUnbind(RenderTexture *renderTexture);
void renderTextureFree(RenderTexture *renderTexture);

#endif // !RENDER_TEXTURE_H
