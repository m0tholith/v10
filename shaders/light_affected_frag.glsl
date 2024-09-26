#version 330 core

in vec3 vColor;
in vec2 vTexCoord;
in vec3 vNormal;
in vec3 vPos;

out vec4 FragColor;

uniform vec3 _cameraWorldPosition;
struct material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
    float specular_strength;
};
uniform material _material = material(vec3(0.1f), vec3(1), vec3(1), 0, 0.1f);

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float intensity;
    float distance;
    float decay;
};
uniform Light light;

float attenuation(vec3 lightPos, float lightDistance, float lightIntensity, float lightDecay);
vec3 diffuse(vec3 lightPos, vec3 lightDiffuse);
vec3 specular(vec3 lightPos, vec3 lightSpecular);
vec3 calc_point_light(Light light);

void main()
{
    FragColor = vec4(calc_point_light(light), 1.0f);
}

float attenuation(vec3 lightPos, float lightDistance, float lightIntensity, float lightDecay)
{
    float distanceRatioSqr = pow(min(distance(lightPos, vPos) / lightDistance, 1), 2);
    float att = lightIntensity * pow(1 - distanceRatioSqr, 2) / (1 + lightDecay * distanceRatioSqr);
    return att;
}
vec3 diffuse(vec3 lightPos, vec3 lightDiffuse)
{
    vec3 lightDir = normalize(lightPos - vPos);
    float diffuseValue = max(dot((vNormal), lightDir), 0.0f);
    vec3 diffuse = lightDiffuse * diffuseValue * _material.diffuse * vColor;
    return diffuse;
}
vec3 specular(vec3 lightPos, vec3 lightSpecular)
{
    vec3 lightDir = normalize(lightPos - vPos);
    vec3 viewDir = normalize(_cameraWorldPosition - vPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(vNormal, halfwayDir), 0.0f), pow(2, _material.shininess));
    vec3 specular = lightSpecular * spec * _material.specular * _material.specular_strength;
    return specular;
}
vec3 calc_point_light(Light light)
{
    vec3 ambient = light.ambient * _material.ambient;
    vec3 diffuse = diffuse(light.position, light.diffuse);
    vec3 specular = specular(light.position, light.specular);

    vec3 resultColor = diffuse + ambient + specular;

    return resultColor * attenuation(light.position, light.distance, light.intensity, light.decay);
}
