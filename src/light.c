#include "light.h"

#include "glad/glad.h"
#include <GL/gl.h>
#include <stddef.h>
#include <stdlib.h>

DirectionalLight *directionalLightCreate(vec3s direction, vec3s ambient,
                                         vec3s diffuse, vec3s specular) {
    DirectionalLight *directionalLight = malloc(sizeof(DirectionalLight));
    *directionalLight = (DirectionalLight){
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

PointLight *pointLightCreate(vec3s position, vec3s ambient, vec3s diffuse,
                             vec3s specular, float intensity, float distance,
                             float decay) {
    PointLight *pointLight = malloc(sizeof(PointLight));
    *pointLight = (PointLight){
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

LightScene *lightSceneCreate(DirectionalLight *directionalLight,
                             PointLight *pointLight) {
    LightScene *lightScene = malloc(sizeof(LightScene));
    lightScene->DirectionalLight = directionalLight;
    lightScene->PointLight = pointLight;

    glGenBuffers(1, &lightScene->UBO);
    glBindBuffer(GL_UNIFORM_BUFFER, lightScene->UBO);
    glBufferData(GL_UNIFORM_BUFFER, DIRLIGHT_SIZE + POINTLIGHT_SIZE, NULL,
                 GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, lightScene->UBO);

    glBufferSubData(GL_UNIFORM_BUFFER, 0, DIRLIGHT_SIZE, directionalLight);
    glBufferSubData(GL_UNIFORM_BUFFER, DIRLIGHT_SIZE, POINTLIGHT_SIZE,
                    pointLight);

    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    return lightScene;
}
void lightScenePrerender(LightScene *lightScene) {
    glBindBuffer(GL_UNIFORM_BUFFER, lightScene->UBO);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, DIRLIGHT_SIZE,
                    lightScene->DirectionalLight);
    glBufferSubData(GL_UNIFORM_BUFFER, DIRLIGHT_SIZE, POINTLIGHT_SIZE,
                    lightScene->PointLight);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
void lightSceneFree(LightScene *lightScene) {
    glDeleteBuffers(1, &lightScene->UBO);
    free(lightScene);
}
