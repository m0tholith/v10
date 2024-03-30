#ifndef TEXTURE_H
#define TEXTURE_H

#include <stdbool.h>

#define objectId unsigned int

objectId textureCreate(const char *textureFile, bool alpha);

#endif
