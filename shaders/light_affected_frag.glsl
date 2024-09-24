#version 460 core

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
};
uniform Light light;

void main()
{
    vec3 ambient = light.ambient * _material.ambient;

    vec3 lightDir = normalize(light.position - vPos);
    float diffuseValue = max(dot((vNormal), lightDir), 0.0f);
    vec3 diffuse = light.diffuse * diffuseValue * _material.diffuse;

    vec3 viewDir = normalize(_cameraWorldPosition - vPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(vNormal, halfwayDir), 0.0f), pow(2, _material.shininess));
    vec3 specular = light.specular * spec * _material.specular * _material.specular_strength;

    FragColor = vec4((diffuse + ambient + specular) * vColor, 1.0f);
}
