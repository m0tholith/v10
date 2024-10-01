#version 420 core

in vec3 vColor;
in vec2 vTexCoord;
in vec3 vNormal;
in vec3 vPos;

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

struct DirectionalLight {
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;

    vec3 direction;
    float _padding;
};
struct PointLight {
    vec3 position;
    float intensity;

    vec3 ambient;
    float distance;

    vec3 diffuse;
    float decay;

    vec3 specular;
    float _padding;
};
struct SpotLight {
    vec3 position;
    float intensity;

    vec3 ambient;
    float distance;

    vec3 diffuse;
    float decay;

    vec3 specular;
    float innerCutoff;

    vec3 direction;
    float outerCutoff;
};
const int DirLightsMax = 1;
const int PointLightsMax = 8;
const int SpotLightsMax = 8;
layout(std140, binding = 1) uniform Lights {
    DirectionalLight[DirLightsMax] directionalLights;
    PointLight[PointLightsMax] pointLights;
    SpotLight[SpotLightsMax] spotLights;
};

vec3 _diffuse;

float attenuation(vec3 lightPos, float lightDistance, float lightIntensity, float lightDecay);
vec3 diffuse(vec3 lightDir, vec3 lightDiffuse);
vec3 specular(vec3 lightDir, vec3 lightSpecular);
vec3 calc_spot_light(SpotLight light);
vec3 calc_point_light(PointLight light);
vec3 calc_directional_light(DirectionalLight light);

void main()
{
    _diffuse = vec3(texture(_material.diffuse_tex, vTexCoord));
    vec3 totalColor = vec3(0);
    for (int i = 0; i < DirLightsMax; i++) {
        totalColor += calc_directional_light(directionalLights[i]);
    }
    for (int i = 0; i < PointLightsMax; i++) {
        totalColor += calc_point_light(pointLights[i]);
    }
    for (int i = 0; i < SpotLightsMax; i++) {
        totalColor += calc_spot_light(spotLights[i]);
    }
    FragColor = vec4(totalColor, 1.0f);
}

float attenuation(vec3 lightPos, float lightDistance, float lightIntensity, float lightDecay)
{
    float distanceRatioSqr = pow(min(distance(lightPos, vPos) / lightDistance, 1), 2);
    float att = lightIntensity * pow(1 - distanceRatioSqr, 2) / (1 + lightDecay * distanceRatioSqr);
    return att;
}
vec3 diffuse(vec3 lightDir, vec3 lightDiffuse)
{
    float diffuseValue = max(dot((vNormal), lightDir), 0.0f);
    vec3 diffuse = lightDiffuse * diffuseValue * _diffuse * vColor;
    return diffuse;
}
vec3 specular(vec3 lightDir, vec3 lightSpecular)
{
    vec3 viewDir = normalize(_cameraWorldPosition - vPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(vNormal, halfwayDir), 0.0f), pow(2, _material.shininess));
    vec3 specular = lightSpecular * spec * _material.specular * _material.specular_strength;
    return specular;
}
float ease(float x) {
    return x < 0.5 ? 4 * x * x * x : 1 - pow(-2 * x + 2, 3) / 2;
}
vec3 calc_spot_light(SpotLight light) {
    vec3 lightDir = normalize(light.position.xyz - vPos);
    vec3 ambient = light.ambient.xyz * _material.ambient;
    float att = attenuation(light.position.xyz, light.distance, light.intensity, light.decay);

    float angle = dot(lightDir, light.direction);
    if (angle < light.outerCutoff)
        return ambient * att;

    vec3 diffuse = diffuse(lightDir, light.diffuse.xyz);
    vec3 specular = specular(lightDir, light.specular.xyz);

    vec3 resultColor = diffuse + specular;

    float t;
    if (light.innerCutoff == light.outerCutoff || angle > light.innerCutoff)
        t = 1;
    else
        t = (angle - light.outerCutoff) / (light.innerCutoff - light.outerCutoff);
    t = ease(t);

    return (ambient + resultColor * t) * att;
}
vec3 calc_point_light(PointLight light)
{
    vec3 lightDir = normalize(light.position.xyz - vPos);
    vec3 ambient = light.ambient.xyz * _material.ambient;
    vec3 diffuse = diffuse(lightDir, light.diffuse.xyz);
    vec3 specular = specular(lightDir, light.specular.xyz);

    vec3 resultColor = diffuse + ambient + specular;

    return resultColor * attenuation(light.position.xyz, light.distance, light.intensity, light.decay);
}
vec3 calc_directional_light(DirectionalLight light) {
    vec3 ambient = light.ambient.xyz * _material.ambient;
    vec3 diffuse = diffuse(light.direction.xyz, light.diffuse.xyz);
    vec3 specular = specular(light.direction.xyz, light.specular.xyz);

    vec3 resultColor = diffuse + ambient + specular;

    return resultColor;
}
