#version 330 core

in vec3 vColor;
in vec2 vTexCoord;
out vec4 FragColor;

uniform sampler2D _texture;

void main()
{
    FragColor = texture(_texture, vTexCoord);
}
