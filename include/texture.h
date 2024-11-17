#ifndef TEXTURE_H
#define TEXTURE_H

#include <stdbool.h>
#include <stdint.h>

#define TEXTURES_PATH "textures/"
#define TEXTURE_DEFAULT "default.jpg"

typedef struct {
    uint32_t id;
} Texture;

/// @param const char *texturePath: path to texture; prepended with
/// `TEXTURES_PATH`
Texture *textureCreate(const char *texturePath, bool optional);
void textureFree(Texture *texture);
void textureFreeCache();

#endif // !TEXTURE_H
