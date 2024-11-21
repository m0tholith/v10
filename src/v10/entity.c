#include "v10/entity.h"

#include "v10/lib/glad/glad.h"
#include <GL/gl.h>

Entity *entityCreate(Model *model, Armature *armature) {
    Entity *entity = malloc(sizeof(Entity));
    entity->Model = model;
    entity->Armature = armature;
    return entity;
}

#define ISDEPTH(__opt)                                                         \
    (__opt & (ENTITY_RENDER_DEPTH_TEX | ENTITY_RENDER_DEPTH_CUBEMAP))
#define DEPTHSHADER(__model, __opt)                                            \
    (__opt & ENTITY_RENDER_DEPTH_TEX ? __model->TexDepthShader                 \
                                     : __model->CubemapDepthShader)

void entityRender(Entity *entity, enum EntityRenderOptions options) {
    if (!(options & ENTITY_RENDER_NOAPPLYTRANSFORMS)) {
        modelPreRender(entity->Model);
        if (entity->Armature != NULL)
            armatureApplyTransformations(entity->Armature);
    }
    if (ISDEPTH(options)) {
        Shader *shader = DEPTHSHADER(entity->Model, options);
        if (entity->Armature != NULL) {
            glUseProgram(shader->ID);
            glUniformMatrix4fv(
                shaderGetUniformLocation(shader, "_boneTransformations"),
                MAX_BONES, GL_FALSE,
                (void *)&entity->Armature->BoneTransformations);
        }
        meshOverrideShaders(shader);
        modelRender(entity->Model);
        meshOverrideShaders(NULL);
    } else
        modelRender(entity->Model);
}
void entityFree(Entity *entity) { free(entity); }
