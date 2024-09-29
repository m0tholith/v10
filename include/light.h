#ifndef LIGHT_H
#define LIGHT_H

#include <cglm/types-struct.h>
#include <stdint.h>

#define DIRLIGHT_SIZE 64
typedef struct {
    vec4s Ambient;
    vec4s Diffuse;
    vec4s Specular;

    vec3s Direction;
    float __padding; // to complete a 16 byte block in uniform buffer
} DirectionalLight;
DirectionalLight *directionalLightCreate(vec3s direction, vec3s ambient,
                                         vec3s diffuse, vec3s specular);
void directionalLightFree(DirectionalLight *directionalLight);

#define POINTLIGHT_SIZE 64
typedef struct {
    vec3s Position;
    float Intensity;

    vec3s Ambient;
    float Distance;

    vec3s Diffuse;
    float Decay;

    vec3s Specular;
    float __padding; // to complete a 16 byte block in uniform buffer
} PointLight;
PointLight *pointLightCreate(vec3s position, vec3s ambient, vec3s diffuse,
                             vec3s specular, float intensity, float distance,
                             float decay);
void pointLightFree(PointLight *pointLight);

typedef struct {
    DirectionalLight *DirectionalLight;
    PointLight *PointLight;

    uint32_t UBO;
} LightScene;
LightScene *lightSceneCreate(DirectionalLight *directionalLight,
                             PointLight *pointLight);
void lightScenePrerender(LightScene *lightScene);
void lightSceneFree(LightScene *lightScene);

#endif // !LIGHT_H
