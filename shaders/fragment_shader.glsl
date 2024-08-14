#version 460 core

in vec3 vColor;
in vec2 vTexCoord;
out vec4 FragColor;

uniform sampler2D _texture;

void main()
{
    FragColor = vec4(vColor, 1) * texture(_texture, vTexCoord);
}
