#include "light.h"

#include "framebuffer.h"
#include "glad/glad.h"
#include "window.h"
#include <GL/gl.h>
#include <cglm/struct/affine-mat.h>
#include <cglm/struct/cam.h>
#include <cglm/util.h>
#include <math.h>
#include <stddef.h>
#include <stdlib.h>

void setPointLightMatrices(PointLight *pointLight,
                           mat4s *projectionFromWorldMatrices);

DirLight dirLightCreate(vec3s direction, vec3s ambient, vec3s diffuse,
                        vec3s specular) {
    DirLight dirLight = (DirLight){
        .ProjectionFromWorld = glms_mul(
            glms_ortho(-10, 10, -10, 10, -10, 10),
            glms_lookat(direction, GLMS_VEC3_ZERO, (vec3s){{0, 1, 0}})),
        .Ambient = ambient,
        .Diffuse = diffuse,
        .Specular = specular,
        .Direction = direction,
        ._enabled = true,
    };
    return dirLight;
}

PointLight pointLightCreate(vec3s position, vec3s diffuse, vec3s specular,
                            float intensity, float distance, float decay) {
    PointLight pointLight = (PointLight){
        .Position = position,
        .Intensity = intensity,
        .Diffuse = diffuse,
        .Decay = decay,
        .Specular = specular,
        .Distance = distance,
        ._enabled = true,
    };
    return pointLight;
}

SpotLight spotLightCreate(vec3s position, vec3s direction, vec3s diffuse,
                          vec3s specular, float intensity, float distance,
                          float decay, float innerCutoffDeg,
                          float outerCutoffDeg) {
    SpotLight spotLight = (SpotLight){
        .ProjectionFromWorld = glms_mul(
            glms_perspective(2 * glm_rad(outerCutoffDeg), 1, 0.1f, distance),
            glms_lookat(position,
                        glms_vec3_add(position, glms_vec3_negate(direction)),
                        (vec3s){{0, 1, 0}})),
        .Position = position,
        .Intensity = intensity,
        .Diffuse = diffuse,
        .Decay = decay,
        .Specular = specular,
        .InnerCutoff = cosf(glm_rad(innerCutoffDeg)),
        .Direction = direction,
        .OuterCutoff = cosf(glm_rad(outerCutoffDeg)),
        .Distance = distance,
        ._enabled = true,
    };
    return spotLight;
}
void spotLightSetCutoff(SpotLight *spotLight, float innerCutoffDeg,
                        float outerCutoffDeg) {
    spotLight->InnerCutoff = cosf(glm_rad(innerCutoffDeg));
    spotLight->OuterCutoff = cosf(glm_rad(outerCutoffDeg));
}

#define DIRLIGHT_SHADOWMAP_SIZE 1024
#define POINTLIGHT_SHADOWMAP_SIZE 256
#define SPOTLIGHT_SHADOWMAP_SIZE 256

LightScene *lightSceneCreate(DirLight *dirLights, PointLight *pointLights,
                             SpotLight *spotLights) {
    LightScene *lightScene = malloc(sizeof(LightScene));
    lightScene->DirLights = dirLights;
    lightScene->PointLights = pointLights;
    lightScene->SpotLights = spotLights;

    lightScene->_prev_data.dirlight_directions =
        malloc(DIRLIGHTS_MAX * sizeof(vec3s));
    memset(lightScene->_prev_data.dirlight_directions, 0,
           DIRLIGHTS_MAX * sizeof(vec3s));
    lightScene->_prev_data.pointlight_positions =
        malloc(POINTLIGHTS_MAX * sizeof(vec3s));
    memset(lightScene->_prev_data.pointlight_positions, 0,
           POINTLIGHTS_MAX * sizeof(vec3s));
    lightScene->_prev_data.pointlight_matrices =
        malloc(6 * POINTLIGHTS_MAX * sizeof(mat4s));
    memset(lightScene->_prev_data.pointlight_matrices, 0,
           6 * POINTLIGHTS_MAX * sizeof(mat4s));
    lightScene->_prev_data.spotlight_directions =
        malloc(SPOTLIGHTS_MAX * sizeof(vec3s));
    memset(lightScene->_prev_data.spotlight_directions, 0,
           SPOTLIGHTS_MAX * sizeof(vec3s));
    lightScene->_prev_data.spotlight_positions =
        malloc(SPOTLIGHTS_MAX * sizeof(vec3s));
    memset(lightScene->_prev_data.spotlight_positions, 0,
           SPOTLIGHTS_MAX * sizeof(vec3s));

    glGenBuffers(1, &lightScene->UBO);
    glBindBuffer(GL_UNIFORM_BUFFER, lightScene->UBO);
    glBufferData(GL_UNIFORM_BUFFER,
                 DIRLIGHTS_MAX * sizeof(DirLight) +
                     POINTLIGHTS_MAX * sizeof(PointLight) +
                     SPOTLIGHTS_MAX * sizeof(SpotLight),
                 NULL, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, lightScene->UBO);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    lightScene->DirLightShadowMaps =
        malloc(DIRLIGHTS_MAX * sizeof(Framebuffer));
    for (int i = 0; i < DIRLIGHTS_MAX; i++) {
        if (!dirLights[i]._enabled)
            break;
        lightScene->DirLightShadowMaps[i] =
            framebufferCreate(DIRLIGHT_SHADOWMAP_SIZE, DIRLIGHT_SHADOWMAP_SIZE,
                              FRAMEBUF_TEX2D | FRAMEBUF_DEPTH);
    }
    lightScene->PointLightShadowMaps =
        malloc(POINTLIGHTS_MAX * sizeof(Framebuffer));
    for (int i = 0; i < POINTLIGHTS_MAX; i++) {
        if (!pointLights[i]._enabled)
            break;
        lightScene->PointLightShadowMaps[i] = framebufferCreate(
            POINTLIGHT_SHADOWMAP_SIZE, POINTLIGHT_SHADOWMAP_SIZE,
            FRAMEBUF_CUBEMAP | FRAMEBUF_DEPTH);
    }
    lightScene->SpotLightShadowMaps =
        malloc(SPOTLIGHTS_MAX * sizeof(Framebuffer));
    for (int i = 0; i < SPOTLIGHTS_MAX; i++) {
        if (!spotLights[i]._enabled)
            break;
        lightScene->SpotLightShadowMaps[i] = framebufferCreate(
            SPOTLIGHT_SHADOWMAP_SIZE, SPOTLIGHT_SHADOWMAP_SIZE,
            FRAMEBUF_TEX2D | FRAMEBUF_DEPTH);
    }

    return lightScene;
}

void lightSceneRenderShadowMaps(LightScene *lightScene, Entity **entities,
                                int entityCount) {
    glCullFace(GL_FRONT);

#define sendMatrices(__name, __matrices, __count, __shader)                    \
    glUseProgram(__shader->ID);                                                \
    glUniformMatrix4fv(shaderGetUniformLocation(__shader, __name), __count,    \
                       GL_FALSE, (void *)__matrices);

    glViewport(0, 0, DIRLIGHT_SHADOWMAP_SIZE, DIRLIGHT_SHADOWMAP_SIZE);
    for (int dirLightIdx = 0; dirLightIdx < DIRLIGHTS_MAX; dirLightIdx++) {
        DirLight *dirLight = &lightScene->DirLights[dirLightIdx];
        if (!dirLight->_enabled)
            continue;

        // check if dirlight matrix needs to be regenerated (if direction has
        // been changed)
        if (!glms_vec3_eqv(
                dirLight->Direction,
                lightScene->_prev_data.dirlight_directions[dirLightIdx])) {
            dirLight->ProjectionFromWorld = glms_mul(
                glms_ortho(-10, 10, -10, 10, -10, 10),
                glms_lookat(dirLight->Direction, GLMS_VEC3_ZERO, GLMS_YUP));
            lightScene->_prev_data.dirlight_directions[dirLightIdx] =
                dirLight->Direction;
        }

        framebufferBind(lightScene->DirLightShadowMaps[dirLightIdx]);
        for (int entityIdx = 0; entityIdx < entityCount; entityIdx++) {
            Entity *entity = entities[entityIdx];
            glUseProgram(entity->Model->TexDepthShader->ID);
            sendMatrices("_lightSpaceProjectionFromWorld",
                         dirLight->ProjectionFromWorld.raw[0], 1,
                         entity->Model->TexDepthShader);
            entityRender(entity, ENTITY_RENDER_DEPTH_TEX);
        }
    }

    glViewport(0, 0, POINTLIGHT_SHADOWMAP_SIZE, POINTLIGHT_SHADOWMAP_SIZE);
    for (int pointLightIdx = 0; pointLightIdx < POINTLIGHTS_MAX;
         pointLightIdx++) {
        PointLight *pointLight = &lightScene->PointLights[pointLightIdx];
        if (!pointLight->_enabled)
            continue;

        mat4s *projectionFromWorldMatrices =
            &lightScene->_prev_data.pointlight_matrices[6 * pointLightIdx];

        // check if point light matrices have to be regenerated (if position or
        // direction have been changed)
        if (!glms_vec3_eqv(
                pointLight->Position,
                lightScene->_prev_data.pointlight_positions[pointLightIdx])) {
            setPointLightMatrices(pointLight, projectionFromWorldMatrices);
            lightScene->_prev_data.pointlight_positions[pointLightIdx] =
                pointLight->Position;
        }

        framebufferBind(lightScene->PointLightShadowMaps[pointLightIdx]);
        for (int entityIdx = 0; entityIdx < entityCount; entityIdx++) {
            Entity *entity = entities[entityIdx];
            Shader *shader = entity->Model->CubemapDepthShader;
            sendMatrices("_lightMatrices", projectionFromWorldMatrices, 6,
                         shader);
            glUniform3fv(shaderGetUniformLocation(shader, "_lightPos"), 1,
                         (void *)&pointLight->Position);
            glUniform1f(shaderGetUniformLocation(shader, "_farPlane"),
                        pointLight->Distance);
            entityRender(entity, ENTITY_RENDER_DEPTH_CUBEMAP |
                                     ENTITY_RENDER_NOAPPLYTRANSFORMS);
        }
    }

    glViewport(0, 0, SPOTLIGHT_SHADOWMAP_SIZE, SPOTLIGHT_SHADOWMAP_SIZE);
    for (int spotLightIdx = 0; spotLightIdx < SPOTLIGHTS_MAX; spotLightIdx++) {
        SpotLight *spotLight = &lightScene->SpotLights[spotLightIdx];
        if (!spotLight->_enabled)
            continue;

        // check if spotlight matrix has to be regenerated (if position or
        // direction have been changed)
        if (!glms_vec3_eqv(
                spotLight->Direction,
                lightScene->_prev_data.spotlight_directions[spotLightIdx]) ||

            !glms_vec3_eqv(
                spotLight->Position,
                lightScene->_prev_data.spotlight_positions[spotLightIdx])) {

            spotLight->ProjectionFromWorld = glms_mul(
                glms_perspective(2 * acosf(spotLight->OuterCutoff), 1, 0.1f,
                                 spotLight->Distance),
                glms_lookat(
                    spotLight->Position,
                    glms_vec3_add(spotLight->Position,
                                  glms_vec3_negate(spotLight->Direction)),
                    GLMS_YUP));

            lightScene->_prev_data.spotlight_positions[spotLightIdx] =
                spotLight->Position;
            lightScene->_prev_data.spotlight_directions[spotLightIdx] =
                spotLight->Direction;
        }

        framebufferBind(lightScene->SpotLightShadowMaps[spotLightIdx]);
        for (int entityIdx = 0; entityIdx < entityCount; entityIdx++) {
            Entity *entity = entities[entityIdx];
            sendMatrices("_lightSpaceProjectionFromWorld",
                         spotLight->ProjectionFromWorld.raw[0], 1,
                         entity->Model->TexDepthShader);
            entityRender(entity, ENTITY_RENDER_DEPTH_TEX |
                                     ENTITY_RENDER_NOAPPLYTRANSFORMS);
        }
    }

    glCullFace(GL_BACK);
    framebufferResetBind();
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
}
void lightScenePreRender(LightScene *lightScene) {
    glBindBuffer(GL_UNIFORM_BUFFER, lightScene->UBO);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, DIRLIGHTS_MAX * sizeof(DirLight),
                    lightScene->DirLights);
    glBufferSubData(GL_UNIFORM_BUFFER, DIRLIGHTS_MAX * sizeof(DirLight),
                    POINTLIGHTS_MAX * sizeof(PointLight),
                    lightScene->PointLights);
    glBufferSubData(GL_UNIFORM_BUFFER,
                    DIRLIGHTS_MAX * sizeof(DirLight) +
                        POINTLIGHTS_MAX * sizeof(PointLight),
                    SPOTLIGHTS_MAX * sizeof(SpotLight), lightScene->SpotLights);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
void lightSceneFree(LightScene *lightScene) {
    glDeleteBuffers(1, &lightScene->UBO);
    for (int i = 0; i < DIRLIGHTS_MAX; i++) {
        if (!lightScene->DirLights[i]._enabled)
            continue;
        framebufferFree(lightScene->DirLightShadowMaps[i]);
    }
    for (int i = 0; i < POINTLIGHTS_MAX; i++) {
        if (!lightScene->PointLights[i]._enabled)
            continue;
        framebufferFree(lightScene->PointLightShadowMaps[i]);
    }
    free(lightScene->DirLightShadowMaps);
    free(lightScene->PointLightShadowMaps);

    free(lightScene->_prev_data.dirlight_directions);
    free(lightScene->_prev_data.pointlight_positions);
    free(lightScene->_prev_data.pointlight_matrices);
    free(lightScene->_prev_data.spotlight_directions);
    free(lightScene->_prev_data.spotlight_positions);

    free(lightScene);
}

void setPointLightMatrices(PointLight *pointLight, mat4s *out) {
    mat4s projectionFromWorld =
        glms_perspective(glm_rad(90), 1, 0.1f, pointLight->Distance);
    out[0] = glms_mul(projectionFromWorld,
                      glms_lookat(pointLight->Position,
                                  glms_vec3_add(pointLight->Position,
                                                (vec3s){{+1.0, 0.0, 0.0}}),
                                  (vec3s){{0.0, -1.0, 0.0}}));
    out[1] = glms_mul(projectionFromWorld,
                      glms_lookat(pointLight->Position,
                                  glms_vec3_add(pointLight->Position,
                                                (vec3s){{-1.0, 0.0, 0.0}}),
                                  (vec3s){{0.0, -1.0, 0.0}}));
    out[2] = glms_mul(projectionFromWorld,
                      glms_lookat(pointLight->Position,
                                  glms_vec3_add(pointLight->Position,
                                                (vec3s){{0.0, +1.0, 0.0}}),
                                  (vec3s){{0.0, 0.0, +1.0}}));
    out[3] = glms_mul(projectionFromWorld,
                      glms_lookat(pointLight->Position,
                                  glms_vec3_add(pointLight->Position,
                                                (vec3s){{0.0, -1.0, 0.0}}),
                                  (vec3s){{0.0, 0.0, -1.0}}));
    out[4] = glms_mul(projectionFromWorld,
                      glms_lookat(pointLight->Position,
                                  glms_vec3_add(pointLight->Position,
                                                (vec3s){{0.0, 0.0, +1.0}}),
                                  (vec3s){{0.0, -1.0, 0.0}}));
    out[5] = glms_mul(projectionFromWorld,
                      glms_lookat(pointLight->Position,
                                  glms_vec3_add(pointLight->Position,
                                                (vec3s){{0.0, 0.0, -1.0}}),
                                  (vec3s){{0.0, -1.0, 0.0}}));
}
