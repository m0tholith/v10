#version 330 core
in vec3 normal;
in vec2 texCoordinates;

uniform sampler2D texSampler;

out vec4 FragColor;

void main()
{
    float p = 3;
    FragColor = texture(texSampler, texCoordinates) * vec4(pow(normal, vec3(p)) / vec3(4) + vec3(0.75), 1.0);
}
