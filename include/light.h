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
    float __padding; // to complete a 16 byte block in uniform buffer
} DirectionalLight;
DirectionalLight directionalLightCreate(vec3s direction, vec3s ambient,
                                        vec3s diffuse, vec3s specular);

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
PointLight pointLightCreate(vec3s position, vec3s ambient, vec3s diffuse,
                            vec3s specular, float intensity, float distance,
                            float decay);

typedef struct {
    vec3s Position;
    float Intensity;

    vec3s Ambient;
    float Distance;

    vec3s Diffuse;
    float Decay;

    vec3s Specular;
    float InnerCutoff;

    vec3s Direction;
    float OuterCutoff;
} SpotLight;
SpotLight spotLightCreate(vec3s position, vec3s direction, vec3s ambient,
                          vec3s diffuse, vec3s specular, float intensity,
                          float distance, float decay, float innerCutoffDeg,
                          float outerCutoffDeg);
void spotLightSetCutoff(SpotLight *spotLight, float innerCutoffDeg,
                        float outerCutoffDeg);

#define DIRLIGHTS_MAX 1
#define POINTLIGHTS_MAX 8
#define SPOTLIGHTS_MAX 8
typedef struct {
    DirectionalLight *DirectionalLights;
    PointLight *PointLights;
    SpotLight *SpotLights;

    uint32_t UBO;
} LightScene;
LightScene *lightSceneCreate(DirectionalLight *directionalLights,
                             PointLight *pointLights, SpotLight *spotLights);
void lightScenePreRender(LightScene *lightScene);
void lightSceneFree(LightScene *lightScene);

#endif // !LIGHT_H
