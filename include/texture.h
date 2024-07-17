#ifndef TEXTURE_H
#define TEXTURE_H

#include <stdbool.h>

#define TEXTURES_PATH "textures/"

/// defines the encoding of the image (used for transparency in shaders)
enum TEXTURETYPE {
    TEXTURETYPE_RGB = 0,
    TEXTURETYPE_RGBA = 1,
};

/// @param const char *texturePath: path to texture; prepended with
/// `TEXTURES_PATH`
unsigned int textureCreate(const char *texturePath, enum TEXTURETYPE type,
                           bool optional);

#endif // !TEXTURE_H
