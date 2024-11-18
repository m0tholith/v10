#version 420 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 18) out;

out vec4 FragPos; // FragPos from GS (output per emitvertex)

uniform mat4 _lightMatrices[6];

void main() {
    for (int face = 0; face < 6; ++face) {
        gl_Layer =
            face; // built-in variable that specifies to which face we render.
        for (int i = 0; i < 3; ++i) // for each triangle vertex
        {
            FragPos = gl_in[i].gl_Position;
            gl_Position = _lightMatrices[face] * FragPos;
            EmitVertex();
        }
        EndPrimitive();
    }
}
