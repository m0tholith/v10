#ifndef LIGHT_H
#define LIGHT_H

#include "framebuffer.h"
#include "scene_object.h"
#include <cglm/types-struct.h>
#include <stdint.h>

typedef struct {
    mat4s ProjectionFromWorld;

    vec3s Ambient;
    float _padding1;

    vec3s Diffuse;
    float _padding2;

    vec3s Specular;
    float _padding3;

    vec3s Direction;
    float _enabled;
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

    float _enabled;
    vec3s _padding; // to complete 16 byte offset
} PointLight;
PointLight pointLightCreate(vec3s position, vec3s diffuse, vec3s specular,
                            float intensity, float distance, float decay);

typedef struct {
    mat4s ProjectionFromWorld;

    vec3s Position;
    float Intensity;

    vec3s Diffuse;
    float Decay;

    vec3s Specular;
    float InnerCutoff;

    vec3s Direction;
    float OuterCutoff;

    float Distance;
    float _enabled;
    vec2s _padding; // to complete 16 byte offset
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

    Framebuffer **DirLightShadowMaps;
    Framebuffer **PointLightShadowMaps;
    Framebuffer **SpotLightShadowMaps;

    /**
     * Stores some data for optimizing light passes.
     *
     * Not to be used by user. Only by light.c.
     */
    struct {
        vec3s *dirlight_directions;

        vec3s *pointlight_positions;
        mat4s *pointlight_matrices; // six for each light

        vec3s *spotlight_directions;
        vec3s *spotlight_positions;
    } _prev_data;
} LightScene;
LightScene *lightSceneCreate(DirLight *dirLights, PointLight *pointLights,
                             SpotLight *spotLights);
void lightSceneRenderShadowMaps(LightScene *lightScene,
                                SceneObject **sceneObjects,
                                int sceneObjectCount);
void lightScenePreRender(LightScene *lightScene);
void lightSceneFree(LightScene *lightScene);

#endif // !LIGHT_H
