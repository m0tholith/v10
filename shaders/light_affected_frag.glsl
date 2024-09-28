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
};
uniform material _material = material(vec3(0.1f), vec3(1), vec3(1), 0, 0.1f);

struct DirectionalLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
struct PointLight {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float intensity;
    float distance;
    float decay;
};
uniform PointLight pointLight;
uniform DirectionalLight directionalLight;

float attenuation(vec3 lightPos, float lightDistance, float lightIntensity, float lightDecay);
vec3 diffuse(vec3 lightDir, vec3 lightDiffuse);
vec3 specular(vec3 lightDir, vec3 lightSpecular);
vec3 calc_point_light(PointLight light);
vec3 calc_directional_light(DirectionalLight light);

void main()
{
    FragColor = vec4(calc_point_light(pointLight) + calc_directional_light(directionalLight), 1.0f);
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
    vec3 diffuse = lightDiffuse * diffuseValue * _material.diffuse * vColor;
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
vec3 calc_point_light(PointLight light)
{
    vec3 lightDir = normalize(light.position - vPos);
    vec3 ambient = light.ambient * _material.ambient;
    vec3 diffuse = diffuse(lightDir, light.diffuse);
    vec3 specular = specular(lightDir, light.specular);

    vec3 resultColor = diffuse + ambient + specular;

    return resultColor * attenuation(light.position, light.distance, light.intensity, light.decay);
}
vec3 calc_directional_light(DirectionalLight light) {
    vec3 ambient = light.ambient * _material.ambient;
    vec3 diffuse = diffuse(light.direction, light.diffuse);
    vec3 specular = specular(light.direction, light.specular);

    vec3 resultColor = diffuse + ambient + specular;

    return resultColor;
}
