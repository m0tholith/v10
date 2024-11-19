#version 420 core

in VS_OUT {
    vec3 Color;
    vec2 TexCoord;
    vec3 Normal;
    vec3 Pos;
}
fs_in;

out vec4 FragColor;

layout(std140, binding = 0) uniform WorldData {
    mat4 _projectionFromWorld;
    vec3 _cameraWorldPosition;
};
struct material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
    float specular_strength;
    sampler2D diffuse_tex;
};
uniform material _material;

struct DirLight {
    mat4 projectionFromWorld;
    vec4 ambient;             // xyz: ambient
    vec4 diffuse;             // xyz: diffuse
    vec4 specular;            // xyz: specular
    vec4 directionAndEnabled; // xyz: direction, w: enabled
};
struct PointLight {
    vec4 positionAndIntensity; // xyz: position, w: intensity
    vec4 diffuseAndDecay;      // xyz: diffuse, w: decay
    vec4 specularAndDistance;  // xyz: specular, w: distance
    vec4 enabled;              // x: enabled
};
struct SpotLight {
    mat4 projectionFromWorld;
    vec4 positionAndIntensity;    // xyz: position, w: intensity
    vec4 diffuseAndDecay;         // xyz: diffuse, w: decay
    vec4 specularAndInnerCutoff;  // xyz: specular, w: inner cutoff
    vec4 directionAndOuterCutoff; // xyz: direction, w: outer cutoff
    vec4 distanceAndEnabled;      // x: distance, y: enabled
};
const int DirLightsMax = 1;
const int PointLightsMax = 1;
const int SpotLightsMax = 1;
layout(std140, binding = 1) uniform Lights {
    DirLight[DirLightsMax] dirLights;
    PointLight[PointLightsMax] pointLights;
    SpotLight[SpotLightsMax] spotLights;
};

layout(binding = 10) uniform sampler2D dirLightShadowMap;
layout(binding = 9) uniform samplerCube pointLightShadowMap;
layout(binding = 8) uniform sampler2D spotLightShadowMap;

vec3 _diffuse;

vec3 calc_dir_light(DirLight light);
vec3 calc_point_light(PointLight light);
vec3 calc_spot_light(SpotLight light);

float attenuation(vec3 lightPos, float lightDistance, float lightIntensity,
                  float lightDecay);
vec3 diffuse(vec3 lightDir, vec3 lightDiffuse);
vec3 specular(vec3 lightDir, vec3 lightSpecular);

float dirlight_shadow(vec4 lightSpacePos, sampler2D shadowMap, float bias);
float pointlight_shadow(PointLight light, samplerCube cubemap, float bias);
float spotlight_shadow(vec4 lightSpacePos, sampler2D shadowMap, float bias);

void main() {
    _diffuse = vec3(texture(_material.diffuse_tex, fs_in.TexCoord));
    vec3 totalColor = vec3(0);
    for (int i = 0; i < DirLightsMax; i++) {
        if (dirLights[i].directionAndEnabled.w == 0)
            break;
        totalColor += calc_dir_light(dirLights[i]);
    }
    for (int i = 0; i < PointLightsMax; i++) {
        if (pointLights[i].enabled.x == 0)
            break;
        totalColor += calc_point_light(pointLights[i]);
    }
    for (int i = 0; i < SpotLightsMax; i++) {
        if (spotLights[i].distanceAndEnabled.y == 0)
            break;
        totalColor += calc_spot_light(spotLights[i]);
    }
    FragColor = vec4(totalColor, 1.0);
}

vec3 calc_dir_light(DirLight light) {
    vec4 lightSpacePos = light.projectionFromWorld * vec4(fs_in.Pos, 1.0);

    vec3 ambient = light.ambient.xyz * _material.ambient;
    vec3 diffuse = diffuse(light.directionAndEnabled.xyz, light.diffuse.xyz);
    vec3 specular = specular(light.directionAndEnabled.xyz, light.specular.xyz);

    vec3 resultColor = diffuse + specular;

    return ambient +
           resultColor * dirlight_shadow(lightSpacePos, dirLightShadowMap, 0);
}
vec3 calc_point_light(PointLight light) {
    vec3 lightDir = normalize(light.positionAndIntensity.xyz - fs_in.Pos);
    vec3 diffuse = diffuse(lightDir, light.diffuseAndDecay.xyz);
    vec3 specular = specular(lightDir, light.specularAndDistance.xyz);

    vec3 resultColor = diffuse + specular;

    return resultColor *
           attenuation(light.positionAndIntensity.xyz,
                       light.specularAndDistance.w,
                       light.positionAndIntensity.w, light.diffuseAndDecay.w) *
           pointlight_shadow(light, pointLightShadowMap, 0.005);
}
#define SPOTLIGHT_EASE(x) (x < 0.5 ? 4 * x * x * x : 1 - pow(-2 * x + 2, 3) / 2)
vec3 calc_spot_light(SpotLight light) {
    vec4 lightSpacePos = light.projectionFromWorld * vec4(fs_in.Pos, 1.0);
    vec3 lightDir = normalize(light.positionAndIntensity.xyz - fs_in.Pos);

    float angle = dot(lightDir, light.directionAndOuterCutoff.xyz);
    if (angle < light.directionAndOuterCutoff.w)
        return vec3(0);

    vec3 diffuse = diffuse(lightDir, light.diffuseAndDecay.xyz);
    vec3 specular = specular(lightDir, light.specularAndInnerCutoff.xyz);

    vec3 resultColor = diffuse + specular;

    float t;
    if (light.specularAndInnerCutoff.w == light.directionAndOuterCutoff.w ||
        angle > light.specularAndInnerCutoff.w)
        t = 1;
    else
        t = (angle - light.directionAndOuterCutoff.w) /
            (light.specularAndInnerCutoff.w - light.directionAndOuterCutoff.w);
    t = SPOTLIGHT_EASE(t);

    return resultColor *
           attenuation(light.positionAndIntensity.xyz,
                       light.distanceAndEnabled.x, light.positionAndIntensity.w,
                       light.diffuseAndDecay.w) *
           t * spotlight_shadow(lightSpacePos, spotLightShadowMap, 0.0);
}
float attenuation(vec3 lightPos, float lightDistance, float lightIntensity,
                  float lightDecay) {
    float distanceRatioSqr =
        pow(min(distance(lightPos, fs_in.Pos) / lightDistance, 1), 2);
    float att = lightIntensity * pow(1 - distanceRatioSqr, 2) /
                (1 + lightDecay * distanceRatioSqr);
    return att;
}
vec3 diffuse(vec3 lightDir, vec3 lightDiffuse) {
    float diffuseValue = max(dot((fs_in.Normal), lightDir), 0.0);
    vec3 diffuse = lightDiffuse * diffuseValue * _material.diffuse * _diffuse *
                   fs_in.Color;
    return diffuse;
}
vec3 specular(vec3 lightDir, vec3 lightSpecular) {
    vec3 viewDir = normalize(_cameraWorldPosition - fs_in.Pos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(fs_in.Normal, halfwayDir), 0.0),
                     pow(2, _material.shininess));
    vec3 specular =
        lightSpecular * spec * _material.specular * _material.specular_strength;
    return specular;
}

#define DISTRIB(__t, __max) (-pow(__max, -2) * abs(__t) + pow(__max, -1))
const int dirPcfSize = 3;
float dirlight_shadow(vec4 lightSpacePos, sampler2D shadowMap, float bias) {
    vec3 projectionCoords = lightSpacePos.xyz / lightSpacePos.w;
    projectionCoords = projectionCoords * 0.5 + vec3(0.5);
    float currentDepth = projectionCoords.z;

    float shadow = 0;
    if (dirPcfSize > 0) {
        vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
        for (int x = -dirPcfSize; x <= dirPcfSize; ++x) {
            for (int y = -dirPcfSize; y <= dirPcfSize; ++y) {
                float pcfDepth = texture(shadowMap, projectionCoords.xy +
                                                        vec2(x, y) * texelSize)
                                     .r;
                if (currentDepth - bias > pcfDepth)
                    shadow += DISTRIB(x, dirPcfSize) * DISTRIB(y, dirPcfSize);
            }
        }
    } else {
        float closestDepth = texture(dirLightShadowMap, projectionCoords.xy).r;
        shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
    }

    return 1 - shadow;
}
const int pointPcfSize = 4;
float pointlight_shadow(PointLight light, samplerCube cubemap, float bias) {
    vec3 fragToLight = fs_in.Pos - light.positionAndIntensity.xyz;

    float currentDepth = length(fragToLight);

    float shadow = 0;
    if (pointPcfSize > 0) {
        float offset = 0.1;
        for (float x = -offset; x < offset;
             x += offset / (pointPcfSize * 0.5)) {
            for (float y = -offset; y < offset;
                 y += offset / (pointPcfSize * 0.5)) {
                for (float z = -offset; z < offset;
                     z += offset / (pointPcfSize * 0.5)) {
                    float closestDepth =
                        texture(cubemap, fragToLight + vec3(x, y, z)).r *
                        light.specularAndDistance.w;
                    if (currentDepth - bias > closestDepth)
                        shadow += 1.0;
                }
            }
        }
        shadow /= pow(pointPcfSize, 3);
    } else {
        float closestDepth = texture(cubemap, fragToLight).r;
        shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
    }

    return 1 - shadow;
}
const int spotPcfSize = 3;
float spotlight_shadow(vec4 lightSpacePos, sampler2D shadowMap, float bias) {
    vec3 projectionCoords = lightSpacePos.xyz / lightSpacePos.w;
    projectionCoords = projectionCoords * 0.5 + vec3(0.5);
    float currentDepth = projectionCoords.z;

    float shadow = 0;
    if (spotPcfSize > 0) {
        vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
        for (int x = -spotPcfSize; x <= spotPcfSize; ++x) {
            for (int y = -spotPcfSize; y <= spotPcfSize; ++y) {
                float pcfDepth = texture(shadowMap, projectionCoords.xy +
                                                        vec2(x, y) * texelSize)
                                     .r;
                if (currentDepth - bias > pcfDepth)
                    shadow += DISTRIB(x, spotPcfSize) * DISTRIB(y, spotPcfSize);
            }
        }
    } else {
        float closestDepth = texture(spotLightShadowMap, projectionCoords.xy).r;
        shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
    }

    return 1 - shadow;
}
