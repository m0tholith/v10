#ifndef CUBEMAP_H
#define CUBEMAP_H

#include <stdint.h>

typedef struct {
    uint32_t ID;
    char *Path;
} Cubemap;
Cubemap *cubemapCreate(char *folderPath, char *extension);
void cubemapFree(Cubemap *cubemap);

#endif // !CUBEMAP_H
