#version 460 core

in vec3 vColor;

out vec4 FragColor;

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform Light light;

void main()
{
    FragColor = vec4(light.diffuse * vColor, 1.0f);
}
