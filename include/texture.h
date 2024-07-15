#ifndef TEXTURE_H
#define TEXTURE_H

#include <stdbool.h>

/// defines the encoding of the image (used for transparency in shaders)
enum TEXTURETYPE {
    TEXTURETYPE_RGB = 0,
    TEXTURETYPE_RGBA = 1,
};

unsigned int textureCreate(const char *textureFile, enum TEXTURETYPE type,
                           bool optional);

#endif // !TEXTURE_H
