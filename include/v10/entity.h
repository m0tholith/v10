#ifndef ENTITY_H
#define ENTITY_H

#include "v10/armature.h"
#include "v10/model.h"

enum EntityRenderOptions {
    ENTITYRENDER_NONE = 0,
    ENTITYRENDER_DEPTH_TEX = (1 << 0),
    ENTITYRENDER_DEPTH_CUBEMAP = (1 << 1),
    ENTITYRENDER_NOAPPLYTRANSFORMS = (1 << 2),
    ENTITYRENDER_WIREFRAME = (1 << 3),
};

typedef struct {
    Model *Model;
    Armature *Armature;
} Entity;

Entity *entityCreate(Model *model, Armature *armature);
void entityRender(Entity *entity, enum EntityRenderOptions options);
void entityFree(Entity *entity);

#endif // !ENTITY_H
