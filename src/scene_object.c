#include "scene_object.h"

#include "glad/glad.h"
#include <GL/gl.h>

SceneObject *sceneObjectCreate(Model *model, Armature *armature) {
    SceneObject *sceneObject = malloc(sizeof(SceneObject));
    sceneObject->Model = model;
    sceneObject->Armature = armature;
    return sceneObject;
}

#define ISDEPTH(__opt)                                                         \
    (__opt & (SCENEOBJ_RENDER_DEPTH_TEX | SCENEOBJ_RENDER_DEPTH_CUBEMAP))
#define DEPTHSHADER(__model, __opt)                                            \
    (__opt & SCENEOBJ_RENDER_DEPTH_TEX ? __model->TexDepthShader               \
                                       : __model->CubemapDepthShader)

void sceneObjectRender(SceneObject *sceneObject,
                       enum SceneObjectRenderOptions options) {
    if (!(options & SCENEOBJ_RENDER_NOAPPLYTRANSFORMS)) {
        modelPreRender(sceneObject->Model);
        if (sceneObject->Armature != NULL)
            armatureApplyTransformations(sceneObject->Armature);
    }
    if (ISDEPTH(options)) {
        Shader *shader = DEPTHSHADER(sceneObject->Model, options);
        if (sceneObject->Armature != NULL) {
            glUseProgram(shader->ID);
            glUniformMatrix4fv(
                glGetUniformLocation(shader->ID, "_boneTransformations"),
                MAX_BONES, GL_FALSE,
                (void *)&sceneObject->Armature->BoneTransformations);
        }
        meshOverrideShaders(shader);
        modelRender(sceneObject->Model);
        meshOverrideShaders(NULL);
    } else
        modelRender(sceneObject->Model);
}
void sceneObjectFree(SceneObject *sceneObject) { free(sceneObject); }
