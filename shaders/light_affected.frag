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
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;

    vec3 direction;
    int enabled;
};
struct PointLight {
    vec3 position;
    float intensity;

    vec3 diffuse;
    float decay;

    vec3 specular;
    float distance;

    int enabled;
};
struct SpotLight {
    vec3 position;
    float intensity;

    vec3 diffuse;
    float decay;

    vec3 specular;
    float innerCutoff;

    vec3 direction;
    float outerCutoff;

    float distance;
    int enabled;
};
const int DirLightsMax = 1;
const int PointLightsMax = 8;
const int SpotLightsMax = 8;
layout(std140, binding = 1) uniform Lights {
    DirLight[DirLightsMax] dirLights;
    PointLight[PointLightsMax] pointLights;
    SpotLight[SpotLightsMax] spotLights;
};

layout(binding = 10) uniform sampler2D shadowMap;

vec3 _diffuse;

float attenuation(vec3 lightPos, float lightDistance, float lightIntensity,
                  float lightDecay);
vec3 diffuse(vec3 lightDir, vec3 lightDiffuse);
vec3 specular(vec3 lightDir, vec3 lightSpecular);
float shadow(vec4 lightSpacePos, float bias);
vec3 calc_spot_light(SpotLight light);
vec3 calc_point_light(PointLight light);
vec3 calc_dir_light(DirLight light);

void main() {
    _diffuse = vec3(texture(_material.diffuse_tex, fs_in.TexCoord));
    vec3 totalColor = vec3(0);
    for (int i = 0; i < DirLightsMax; i++) {
        if (dirLights[i].enabled == 0)
            break;
        totalColor += calc_dir_light(dirLights[i]);
    }
    for (int i = 0; i < PointLightsMax; i++) {
        if (pointLights[i].enabled == 0)
            break;
        totalColor += calc_point_light(pointLights[i]);
    }
    for (int i = 0; i < SpotLightsMax; i++) {
        if (spotLights[i].enabled == 0)
            break;
        totalColor += calc_spot_light(spotLights[i]);
    }
    FragColor = vec4(totalColor, 1.0);
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
float ease(float x) {
    return x < 0.5 ? 4 * x * x * x : 1 - pow(-2 * x + 2, 3) / 2;
}
float shadow(vec4 lightSpacePos, float bias) {
    vec3 projectionCoords = lightSpacePos.xyz / lightSpacePos.w;
    projectionCoords = projectionCoords * 0.5 + vec3(0.5);

    float closestDepth = texture(shadowMap, projectionCoords.xy).r;
    float currentDepth = projectionCoords.z;
    float shadow = currentDepth - bias > closestDepth ? 0.0 : 1.0;
    if (projectionCoords.z > 1.0)
        shadow = 0.0;

    return shadow;
}
vec3 calc_spot_light(SpotLight light) {
    vec3 lightDir = normalize(light.position.xyz - fs_in.Pos);

    float angle = dot(lightDir, light.direction);
    if (angle < light.outerCutoff)
        return vec3(0);

    vec3 diffuse = diffuse(lightDir, light.diffuse.xyz);
    vec3 specular = specular(lightDir, light.specular.xyz);

    vec3 resultColor = diffuse + specular;

    float t;
    if (light.innerCutoff == light.outerCutoff || angle > light.innerCutoff)
        t = 1;
    else
        t = (angle - light.outerCutoff) /
            (light.innerCutoff - light.outerCutoff);
    t = ease(t);

    return resultColor *
           attenuation(light.position.xyz, light.distance, light.intensity,
                       light.decay) *
           t;
}
vec3 calc_point_light(PointLight light) {
    vec3 lightDir = normalize(light.position.xyz - fs_in.Pos);
    vec3 diffuse = diffuse(lightDir, light.diffuse.xyz);
    vec3 specular = specular(lightDir, light.specular.xyz);

    vec3 resultColor = diffuse + specular;

    return resultColor * attenuation(light.position.xyz, light.distance,
                                     light.intensity, light.decay);
}
vec3 calc_dir_light(DirLight light) {
    vec4 lightSpacePos = light.projectionFromWorld * vec4(fs_in.Pos, 1.0);

    vec3 ambient = light.ambient.xyz * _material.ambient;
    vec3 diffuse = diffuse(light.direction.xyz, light.diffuse.xyz);
    vec3 specular = specular(light.direction.xyz, light.specular.xyz);

    vec3 resultColor = diffuse + specular;

    return ambient + resultColor * shadow(lightSpacePos, 0);
}
