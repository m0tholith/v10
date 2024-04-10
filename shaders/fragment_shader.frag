#version 330 core
in vec3 vertexColor;
in vec2 texCoordinates;

uniform sampler2D texSampler;
#ifdef AWESOMEFACE
uniform sampler2D awesomefaceTexSampler;
#endif

out vec4 FragColor;

vec4 layer(vec4 foreground, vec4 background) {
    return foreground * foreground.a + background * (1.0f - foreground.a);
}

void main()
{
    vec4 tex = texture(texSampler, texCoordinates);
#ifdef AWESOMEFACE
    vec4 awesomeface = texture(awesomefaceTexSampler, texCoordinates);
    FragColor = layer(awesomeface, tex) * vec4(vertexColor, 1.0f);
#else
    FragColor = tex * vec4(vertexColor, 1.0f);
#endif
}
