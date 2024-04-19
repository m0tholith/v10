#ifndef TEXTURE_H
#define TEXTURE_H

#include <stdbool.h>

#define objectId unsigned int

typedef enum TEXTURETYPE {
    RGB = 0,
    RGBA = 1,
} TEXTURETYPE;
typedef struct Texture {
    objectId Id;
    TEXTURETYPE Type;
} Texture;

Texture textureCreate(const char *textureFile, TEXTURETYPE type);

#endif
