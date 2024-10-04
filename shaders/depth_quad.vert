#version 420 core

layout(location = 0) in vec2 vertPos;
layout(location = 1) in vec2 vertTexCoord;

out VS_OUT {
    vec2 UV;
} vs_out;

void main()
{
    gl_Position = vec4(vertPos.x, vertPos.y, 0.0, 1.0);
    vs_out.UV = vertTexCoord;
}
