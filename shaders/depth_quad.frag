#version 420 core

out vec4 FragColor;

in VS_OUT {
    vec2 UV;
} fs_in;

uniform sampler2D depthMap;

void main()
{
    float depthValue = texture(depthMap, fs_in.UV).r;
    FragColor = vec4(vec3(depthValue), 1.0);
}
