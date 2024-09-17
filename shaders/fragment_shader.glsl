#version 460 core

in vec3 vColor;
in vec2 vTexCoord;
in vec3 vNormal;
in vec3 vPos;

out vec4 FragColor;

uniform vec3 light_position;
uniform vec3 light_color;

const vec3 ambient_color = vec3(0.1f);

void main()
{
    vec3 lightDir = normalize(light_position - vPos);
    float diffuseValue = max(dot((vNormal), lightDir), 0.0f);
    vec3 diffuse = diffuseValue * light_color;

    FragColor = vec4((diffuse + ambient_color) * vColor, 1.0f);
}
