#version 330 core

in vec3 vColor;

out vec4 FragColor;

struct Light {
    vec3 diffuse;
};
uniform Light light;

void main()
{
    FragColor = vec4(light.diffuse, 1.0f);
}
