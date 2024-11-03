#ifndef SCENE_OBJECT_H
#define SCENE_OBJECT_H

#include "armature.h"
#include "model.h"

typedef struct {
    Model *Model;
    Armature *Armature;
} SceneObject;

SceneObject *sceneObjectCreate(Model *model, Armature *armature);
void sceneObjectRender(SceneObject *sceneObject, bool renderDepth);
void sceneObjectFree(SceneObject *sceneObject);

#endif // !SCENE_OBJECT_H
