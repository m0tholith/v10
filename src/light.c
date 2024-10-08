#include "light.h"

#include "glad/glad.h"
#include <GL/gl.h>
#include <cglm/util.h>
#include <math.h>
#include <stddef.h>
#include <stdlib.h>

DirectionalLight directionalLightCreate(vec3s direction, vec3s ambient,
                                        vec3s diffuse, vec3s specular) {
    DirectionalLight directionalLight = (DirectionalLight){
        .Ambient = (vec4s){{ambient.x, ambient.y, ambient.z, 0}},
        .Diffuse = (vec4s){{diffuse.x, diffuse.y, diffuse.z, 0}},
        .Specular = (vec4s){{specular.x, specular.y, specular.z, 0}},
        .Direction = direction,
    };
    return directionalLight;
}
void directionalLightFree(DirectionalLight *directionalLight) {
    free(directionalLight);
}

PointLight pointLightCreate(vec3s position, vec3s ambient, vec3s diffuse,
                            vec3s specular, float intensity, float distance,
                            float decay) {
    PointLight pointLight = (PointLight){
        .Position = position,
        .Intensity = intensity,
        .Ambient = ambient,
        .Distance = distance,
        .Diffuse = diffuse,
        .Decay = decay,
        .Specular = specular,
    };
    return pointLight;
}
void pointLightFree(PointLight *pointLight) { free(pointLight); }

SpotLight spotLightCreate(vec3s position, vec3s direction, vec3s ambient,
                          vec3s diffuse, vec3s specular, float intensity,
                          float distance, float decay, float innerCutoffDeg,
                          float outerCutoffDeg) {
    SpotLight spotLight = (SpotLight){
        .Position = position,
        .Intensity = intensity,
        .Ambient = ambient,
        .Distance = distance,
        .Diffuse = diffuse,
        .Decay = decay,
        .Specular = specular,
        .InnerCutoff = cosf(glm_rad(innerCutoffDeg)),
        .Direction = direction,
        .OuterCutoff = cosf(glm_rad(outerCutoffDeg)),
    };
    return spotLight;
}
void spotLightSetCutoff(SpotLight *spotLight, float innerCutoffDeg,
                        float outerCutoffDeg) {
    spotLight->InnerCutoff = cosf(glm_rad(innerCutoffDeg));
    spotLight->OuterCutoff = cosf(glm_rad(outerCutoffDeg));
}

LightScene *lightSceneCreate(DirectionalLight *directionalLights,
                             PointLight *pointLights, SpotLight *spotLights) {
    LightScene *lightScene = malloc(sizeof(LightScene));
    lightScene->DirectionalLights = directionalLights;
    lightScene->PointLights = pointLights;
    lightScene->SpotLights = spotLights;

    glGenBuffers(1, &lightScene->UBO);
    glBindBuffer(GL_UNIFORM_BUFFER, lightScene->UBO);
    glBufferData(GL_UNIFORM_BUFFER,
                 DIRLIGHTS_MAX * sizeof(DirectionalLight) +
                     POINTLIGHTS_MAX * sizeof(PointLight) +
                     SPOTLIGHTS_MAX * sizeof(SpotLight),
                 NULL, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, lightScene->UBO);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    return lightScene;
}
void lightScenePreRender(LightScene *lightScene) {
    glBindBuffer(GL_UNIFORM_BUFFER, lightScene->UBO);
    glBufferSubData(GL_UNIFORM_BUFFER, 0,
                    DIRLIGHTS_MAX * sizeof(DirectionalLight),
                    lightScene->DirectionalLights);
    glBufferSubData(GL_UNIFORM_BUFFER, DIRLIGHTS_MAX * sizeof(DirectionalLight),
                    POINTLIGHTS_MAX * sizeof(PointLight),
                    lightScene->PointLights);
    glBufferSubData(GL_UNIFORM_BUFFER,
                    DIRLIGHTS_MAX * sizeof(DirectionalLight) +
                        POINTLIGHTS_MAX * sizeof(PointLight),
                    SPOTLIGHTS_MAX * sizeof(SpotLight), lightScene->SpotLights);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
void lightSceneFree(LightScene *lightScene) {
    glDeleteBuffers(1, &lightScene->UBO);
    free(lightScene);
}
