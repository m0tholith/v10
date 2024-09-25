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

float attenuation(Light light) {
    float distanceRatioSqr = pow(min(distance(light.position, vPos) / light.distance, 1), 2);
    float att = light.intensity * pow(1 - distanceRatioSqr, 2) / (1 + light.decay * distanceRatioSqr);
    return att;
}
vec3 calc_point_light(Light light)
{
    vec3 ambient = light.ambient * _material.ambient;

    vec3 lightDir = normalize(light.position - vPos);
    float diffuseValue = max(dot((vNormal), lightDir), 0.0f);
    vec3 diffuse = light.diffuse * diffuseValue * _material.diffuse * vColor;

    vec3 viewDir = normalize(_cameraWorldPosition - vPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(vNormal, halfwayDir), 0.0f), pow(2, _material.shininess));
    vec3 specular = light.specular * spec * _material.specular * _material.specular_strength;

    vec3 resultColor = diffuse + ambient + specular;

    return resultColor * attenuation(light);
}
void main()
{
    FragColor = vec4(calc_point_light(light), 1.0f);
}
