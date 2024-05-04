#version 330 core

in vec2 uv;

out vec4 FragColor;

uniform sampler2D texSampler;
uniform float tint = 1;

void main()
{
    FragColor = vec4(vec3(tint), 1) * texture(texSampler, uv);
}
