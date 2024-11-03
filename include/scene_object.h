#ifndef SCENE_OBJECT_H
#define SCENE_OBJECT_H

#include "armature.h"
#include "model.h"

enum SceneObjectRenderOptions {
    SCENEOBJ_RENDER_NONE = 0,
    SCENEOBJ_RENDER_DEPTH = (1 << 0),
    SCENEOBJ_RENDER_NOAPPLYTRANSFORMS = (1 << 1),
};

typedef struct {
    Model *Model;
    Armature *Armature;
} SceneObject;

SceneObject *sceneObjectCreate(Model *model, Armature *armature);
void sceneObjectRender(SceneObject *sceneObject,
                       enum SceneObjectRenderOptions options);
void sceneObjectFree(SceneObject *sceneObject);

#endif // !SCENE_OBJECT_H
