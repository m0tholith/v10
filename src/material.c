#include "material.h"

#include "glad/glad.h"

Material *materialCreate(unsigned int shader, vec3s tint) {
    Material *material = &(Material){
        .Shader = shader,
        .Tint = tint,
    };
    materialApplyProperties(material);
    return material;
}
void materialApplyProperties(Material *material) {
    glUniform3fv(glGetUniformLocation(material->Shader, "tint"), 1, material->Tint.raw);
}
