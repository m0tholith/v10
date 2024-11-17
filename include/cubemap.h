#ifndef CUBEMAP_H
#define CUBEMAP_H

#include <stdint.h>

typedef struct {
    uint32_t ID;
    char *Path;
} Cubemap;
/**
 * Creates cubemap.
 *
 * Loads cubemap files from specified path (directory) and loads them. Files
 * must be named nx, px, ny, py, nz, pz
 *
 * @param: cubemapPath Directory to cubemap files.
 * @return: Resulting cubemap.
 *
 * @example: Cubemap *cubemap = cubemapCreate("textures/skybox_cubemap");
 */
Cubemap *cubemapCreate(char *cubemapPath);
void cubemapFree(Cubemap *cubemap);

#endif // !CUBEMAP_H
