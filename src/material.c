#include "material.h"

#include "glad/glad.h"
#include <GL/gl.h>
#include <cglm/types-struct.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// `data` should be a pointer to a value defined in `MaterialType`
MaterialProperty *materialPropertyCreate(const char *name, MaterialType type,
                                         void *data) {
    MaterialProperty *property = malloc(sizeof(MaterialProperty));
    property->Name = name;
    property->Type = type;
    property->Data = data;
    return property;
}
void materialPropertyFree(MaterialProperty *property) { free(property); }

Material *materialCreate(unsigned int shader, int propertyCount, ...) {
    Material *material = malloc(sizeof(Material));
    material->Shader = shader;
    material->PropertyCount = propertyCount;
    material->Properties = malloc(propertyCount * sizeof(MaterialProperty *));
    va_list properties;
    va_start(properties, propertyCount);
    for (int i = 0; i < propertyCount; i++) {
        material->Properties[i] = va_arg(properties, MaterialProperty *);
    }
    va_end(properties);
    return material;
}

void applyProperty(MaterialProperty *property, unsigned int shader) {
    float floatValue;
    vec2s vec2;
    vec3s vec3;
    vec4s vec4;
    mat2s mat2;
    mat3s mat3;
    mat4s mat4;
    int textureData;
    switch (property->Type) {
    case MATTYPE_INT:
        break;
    case MATTYPE_FLOAT:
        floatValue = *(float *)property->Data;
        glUniform1f(glGetUniformLocation(shader, property->Name), floatValue);
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
        textureData = (int)(property->Data);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureData);
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
