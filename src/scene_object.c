#include "scene_object.h"

#include "glad/glad.h"
#include <GL/gl.h>

SceneObject *sceneObjectCreate(Model *model, Armature *armature) {
    SceneObject *sceneObject = malloc(sizeof(SceneObject));
    sceneObject->Model = model;
    sceneObject->Armature = armature;
    return sceneObject;
}
void sceneObjectRender(SceneObject *sceneObject,
                       enum SceneObjectRenderOptions options) {
    if (options & ~SCENEOBJ_RENDER_NOAPPLYTRANSFORMS)
        modelPreRender(sceneObject->Model);
    if (options & SCENEOBJ_RENDER_DEPTH)
        meshOverrideShaders(sceneObject->Model->DepthShader);
    if (options & ~SCENEOBJ_RENDER_NOAPPLYTRANSFORMS &&
        sceneObject->Armature != NULL) {
        armatureApplyTransformations(sceneObject->Armature);
        if (options & SCENEOBJ_RENDER_DEPTH) {
            glUseProgram(sceneObject->Model->DepthShader->ID);
            glUniformMatrix4fv(
                glGetUniformLocation(sceneObject->Model->DepthShader->ID,
                                     "_boneTransformations"),
                MAX_BONES, GL_FALSE,
                (void *)&sceneObject->Armature->BoneTransformations);
        }
    }
    modelRender(sceneObject->Model);
    if (options & SCENEOBJ_RENDER_DEPTH)
        meshOverrideShaders(NULL);
}
void sceneObjectFree(SceneObject *sceneObject) { free(sceneObject); }
