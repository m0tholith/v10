#ifndef LIGHT_H
#define LIGHT_H

#include <cglm/types-struct.h>
#include <stdint.h>

typedef struct {
    mat4s ProjectionFromWorld;
    vec4s Ambient;
    vec4s Diffuse;
    vec4s Specular;

    vec3s Direction;
    int _enabled;
} DirLight; // short for directional light
DirLight dirLightCreate(vec3s direction, vec3s ambient, vec3s diffuse,
                        vec3s specular);

typedef struct {
    vec3s Position;
    float Intensity;

    vec3s Diffuse;
    float Decay;

    vec3s Specular;
    float Distance;

    int _enabled;
} PointLight;
PointLight pointLightCreate(vec3s position, vec3s diffuse, vec3s specular,
                            float intensity, float distance, float decay);

typedef struct {
    vec3s Position;
    float Intensity;

    vec3s Diffuse;
    float Decay;

    vec3s Specular;
    float InnerCutoff;

    vec3s Direction;
    float OuterCutoff;

    float Distance;
    int _enabled;
} SpotLight;
SpotLight spotLightCreate(vec3s position, vec3s direction, vec3s diffuse,
                          vec3s specular, float intensity, float distance,
                          float decay, float innerCutoffDeg,
                          float outerCutoffDeg);
void spotLightSetCutoff(SpotLight *spotLight, float innerCutoffDeg,
                        float outerCutoffDeg);

#define DIRLIGHTS_MAX 1
#define POINTLIGHTS_MAX 8
#define SPOTLIGHTS_MAX 8
typedef struct {
    DirLight *DirLights;
    PointLight *PointLights;
    SpotLight *SpotLights;

    uint32_t UBO;
} LightScene;
LightScene *lightSceneCreate(DirLight *dirLights, PointLight *pointLights,
                             SpotLight *spotLights);
void lightScenePreRender(LightScene *lightScene);
void lightSceneFree(LightScene *lightScene);

#endif // !LIGHT_H
