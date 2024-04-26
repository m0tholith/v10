#version 330 core
in vec3 normal;
in vec2 texCoordinates;

uniform sampler2D texSampler;

out vec4 FragColor;

void main()
{
    // FragColor = texture(texSampler, texCoordinates);
    float pow_thing = 3;
    FragColor = vec4(pow((normal + vec3(1, 1, 1)) / 2, vec3(pow_thing, pow_thing, pow_thing)), 1.0);
}
