#version 460 core

in vec3 vColor;
in vec2 vTexCoord;
in vec3 vNormal;
in vec3 vPos;

out vec4 FragColor;

uniform vec3 _cameraWorldPosition;
uniform vec3 _diffuse;

uniform vec3 light_position;
uniform vec3 light_color;

const vec3 ambient_color = vec3(0.1f);
const float specular_strength = 0.15f;
const int shininess = 6;

void main()
{
    vec3 lightDir = normalize(light_position - vPos);
    float diffuseValue = max(dot((vNormal), lightDir), 0.0f);
    vec3 diffuse = diffuseValue * light_color;

    vec3 viewDir = normalize(_cameraWorldPosition - vPos);
    vec3 reflectDir = reflect(-lightDir, vNormal);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(vNormal, halfwayDir), 0.0f), pow(2, shininess));
    vec3 specular = specular_strength * spec * light_color;

    if (_diffuse != vec3(0))
        FragColor = vec4((diffuse + ambient_color + specular) * vColor * _diffuse, 1.0f);
    else
        FragColor = vec4((diffuse + ambient_color + specular) * vColor, 1.0f);
}
