#include "material.h"

#include "glad/glad.h"
#include <GL/gl.h>

Material *materialCreate(unsigned int shader, unsigned int textureId,
                         vec3s tint) {
    Material *material = &(Material){
        .Shader = shader,
        .TextureId = textureId,
        .Tint = tint,
    };
    materialApplyProperties(material);
    return material;
}
void materialPreRender(Material *material) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, material->TextureId);
}
void materialApplyProperties(Material *material) {
    glUniform3fv(glGetUniformLocation(material->Shader, "tint"), 1,
                 material->Tint.raw);
}
