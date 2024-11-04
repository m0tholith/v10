#include "light.h"

#include "cglm/struct/affine-mat.h"
#include "cglm/struct/cam.h"
#include "glad/glad.h"
#include "framebuffer.h"
#include "window.h"
#include <GL/gl.h>
#include <cglm/util.h>
#include <math.h>
#include <stddef.h>
#include <stdlib.h>

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

#define DIRLIGHT_SHADOWMAP_SIZE 2048

LightScene *lightSceneCreate(DirLight *dirLights, PointLight *pointLights,
                             SpotLight *spotLights) {
    LightScene *lightScene = malloc(sizeof(LightScene));
    lightScene->DirLights = dirLights;
    lightScene->PointLights = pointLights;
    lightScene->SpotLights = spotLights;

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
        lightScene->DirLightShadowMaps[i] = framebufferCreate(
            DIRLIGHT_SHADOWMAP_SIZE, DIRLIGHT_SHADOWMAP_SIZE, FRAMEBUF_DEPTH);
    }

    return lightScene;
}
void sendLightMatrix(mat4s *matrix, Model *model) {
    glUseProgram(model->DepthShader->ID);
    glUniformMatrix4fv(glGetUniformLocation(model->DepthShader->ID,
                                            "_lightSpaceProjectionFromWorld"),
                       1, GL_FALSE, (void *)matrix);
}
void lightSceneRenderShadowMaps(LightScene *lightScene,
                                SceneObject **sceneObjects,
                                int sceneObjectCount) {
    glCullFace(GL_FRONT);

    for (int dirLightIdx = 0; dirLightIdx < DIRLIGHTS_MAX; dirLightIdx++) {
        framebufferBind(lightScene->DirLightShadowMaps[dirLightIdx]);
        for (int objIdx = 0; objIdx < sceneObjectCount; objIdx++) {
            sendLightMatrix(
                &lightScene->DirLights[dirLightIdx].ProjectionFromWorld,
                sceneObjects[objIdx]->Model);
            sceneObjectRender(sceneObjects[objIdx], SCENEOBJ_RENDER_DEPTH);
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
        framebufferFree(lightScene->DirLightShadowMaps[i]);
    }
    free(lightScene->DirLightShadowMaps);
    free(lightScene);
}
