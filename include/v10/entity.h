#ifndef ENTITY_H
#define ENTITY_H

#include "v10/armature.h"
#include "v10/model.h"

enum EntityRenderOptions {
    ENTITY_RENDER_NONE = 0,
    ENTITY_RENDER_NOAPPLYTRANSFORMS = (1 << 0),
    ENTITY_RENDER_DEPTH_TEX = (1 << 1),
    ENTITY_RENDER_DEPTH_CUBEMAP = (1 << 2),
};

typedef struct {
    Model *Model;
    Armature *Armature;
} Entity;

Entity *entityCreate(Model *model, Armature *armature);
void entityRender(Entity *entity, enum EntityRenderOptions options);
void entityFree(Entity *entity);

#endif // !ENTITY_H
