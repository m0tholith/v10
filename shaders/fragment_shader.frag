#version 330 core

in vec2 uv;

out vec4 FragColor;

uniform sampler2D texSampler;
uniform vec3 tint;

void main()
{
    FragColor = vec4(tint, 1) * texture(texSampler, uv);
}
