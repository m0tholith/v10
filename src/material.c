#include "material.h"

#include "cglm/types-struct.h"
#include "glad/glad.h"
#include <GL/gl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef void (*PreRenderFunction)(void *);
void tex2dPreRender(void *property);
PreRenderFunction getPreRender(MaterialType type);

MaterialProperty *materialPropertyCreate(const char *name, MaterialType type,
                                         void *data) {
    MaterialProperty *property = malloc(sizeof(MaterialProperty));
    property->Name = name;
    property->Type = type;
    property->PreRender = getPreRender(type);
    property->Data = data;
    return property;
}
void materialPropertyFree(MaterialProperty *property) { free(property); }

Material *materialCreate(unsigned int shader, int propertyCount) {
    Material *material = malloc(sizeof(Material));
    material->Shader = shader;
    material->PropertyCount = propertyCount;
    material->Properties = malloc(propertyCount * sizeof(MaterialProperty *));
    return material;
}

void materialPreRender(Material *material) {
    materialApplyProperties(material);
    for (int i = 0; i < material->PropertyCount; i++) {
        if (material->Properties[i]->PreRender)
            material->Properties[i]->PreRender(material->Properties[i]);
    }
}
void applyProperty(MaterialProperty *property, unsigned int shader) {
    vec2s vec2;
    vec3s vec3;
    vec4s vec4;
    mat2s mat2;
    mat3s mat3;
    mat4s mat4;
    switch (property->Type) {
    case MATTYPE_INT:
        break;
    case MATTYPE_FLOAT:
        glUniform1f(glGetUniformLocation(shader, property->Name),
                    *(float *)property->Data);
        break;
    case MATTYPE_VEC2:
        vec2 = *(vec2s *)property->Data;
        glUniform2fv(glGetUniformLocation(shader, property->Name), 1, vec2.raw);
        break;
    case MATTYPE_VEC3:
        vec3 = *(vec3s *)property->Data;
        glUniform3fv(glGetUniformLocation(shader, property->Name), 1, vec3.raw);
        break;
    case MATTYPE_VEC4:
        vec4 = *(vec4s *)property->Data;
        glUniform4fv(glGetUniformLocation(shader, property->Name), 1, vec4.raw);
        break;
    case MATTYPE_MAT2:
        mat2 = *(mat2s *)property->Data;
        glUniformMatrix2fv(glGetUniformLocation(shader, property->Name), 1,
                           GL_FALSE, mat2.raw[0]);
    case MATTYPE_MAT3:
        mat3 = *(mat3s *)property->Data;
        glUniformMatrix3fv(glGetUniformLocation(shader, property->Name), 1,
                           GL_FALSE, mat3.raw[0]);
        break;
    case MATTYPE_MAT4:
        mat4 = *(mat4s *)property->Data;
        glUniformMatrix4fv(glGetUniformLocation(shader, property->Name), 1,
                           GL_FALSE, mat4.raw[0]);
        break;
    case MATTYPE_TEXTURE2D:
        // nothing to do, textures are binded at render
        break;
    default:
        fprintf(stderr,
                "material property error: property \"%s\" uses type \"%d\" "
                "which is not defined\n",
                property->Name, property->Type);
        break;
    }
}
void materialApplyProperties(Material *material) {
    for (int i = 0; i < material->PropertyCount; i++) {
        applyProperty(material->Properties[i], material->Shader);
    }
}
Material *materialCopy(Material *source) {
    Material *newMaterial =
        materialCreate(source->Shader, source->PropertyCount);
    MaterialProperty *property;
    for (int i = 0; i < source->PropertyCount; i++) {
        property = source->Properties[i];
        newMaterial->Properties[i] = materialPropertyCreate(
            property->Name, property->Type, property->Data);
    }
    return newMaterial;
}
void materialChangeProperty(Material *material, const char *propertyName,
                            void *newData) {
    for (int i = 0; i < material->PropertyCount; i++) {
        if (!strcmp(propertyName, material->Properties[i]->Name)) {
            material->Properties[i]->Data = newData;
            materialApplyProperties(material);
            return;
        }
    }
    fprintf(stderr,
            "material change property error: material doesn't have property "
            "\"%s\"\n",
            propertyName);
}
void materialFree(Material *material) {
    for (int i = 0; i < material->PropertyCount; i++)
        free(material->Properties[i]);
    free(material->Properties);
    free(material);
}

PreRenderFunction getPreRender(MaterialType type) {
    switch (type) {
    case MATTYPE_TEXTURE2D:
        return tex2dPreRender;
    default:
        return NULL;
    }
}
void tex2dPreRender(void *property) {
    MaterialTextureData *data =
        (MaterialTextureData *)((MaterialProperty *)property)->Data;
    glActiveTexture(GL_TEXTURE0 + data->Index);
    glBindTexture(GL_TEXTURE_2D, data->TextureID);
}
