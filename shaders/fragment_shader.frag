#version 330 core
in vec3 vertexColor;
in vec2 texCoordinates;

uniform sampler2D texSampler;

out vec4 FragColor;

void main()
{
   FragColor = texture(texSampler, texCoordinates) * vec4(vertexColor, 1.0f);
}
