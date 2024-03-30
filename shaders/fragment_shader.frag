#version 330 core
in vec3 vertexColor;
in vec2 texCoordinates;

uniform sampler2D texSampler;
uniform sampler2D awesomefaceTexSampler;
uniform float blend;

out vec4 FragColor;

void main()
{
   FragColor = mix(texture(texSampler, texCoordinates), texture(awesomefaceTexSampler, texCoordinates), blend) * vec4(vertexColor, 1.0f);
}
