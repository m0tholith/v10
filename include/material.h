#ifndef MATERIAL_H
#define MATERIAL_H

#include "texture.h"

/// all types that can be defined as uniforms in shaders. if you want to add
/// more, you need to define a case for your new type in
/// `materialApplyProperties`.
///
/// `MATTYPE_TEXTURE2D` is used with `MaterialTextureData` type
enum MaterialType {
    MATTYPE_INT,
    MATTYPE_FLOAT,
    MATTYPE_VEC2,
    MATTYPE_VEC3,
    MATTYPE_VEC4,
    MATTYPE_MAT2,
    MATTYPE_MAT3,
    MATTYPE_MAT4,
    MATTYPE_TEXTURE2D,
};

typedef struct {
    enum MaterialType Type;
    char *Name;
    void *Data;
} MaterialProperty;
typedef struct {
    Texture *Texture;
    char Index;
} MaterialTextureData;

/// returns material property to be used in `materialCreate`. free with
/// `materialPropertyFree`
/// @param void *`data`: pointer to data
MaterialProperty *materialPropertyCreate(const char *name,
                                         enum MaterialType type, void *data);
/// frees property and its name
void materialPropertyFree(MaterialProperty *property);
/// frees property and its name, along with its data
///
/// use this e.x. for freeing properties imported by model (_diffuse, etc.)
void materialPropertyDelete(MaterialProperty *property);
/// returns material texture property data. free with `materialTextureDataFree`
MaterialTextureData *materialTextureDataCreate(Texture *texture, int index);
void materialTextureDataFree(MaterialTextureData *data);

typedef struct {
    uint32_t Shader;

    int PropertyCount;
    MaterialProperty **Properties;
} Material;

/// returns material for use with models
/// @param int `propertyCount`: the number of properties to allocate for
/// @param MaterialProperty *...: x number of properties to set
Material *materialCreate(uint32_t shader, int propertyCount, ...);
void materialApplyProperties(Material *material);
Material *materialCopy(Material *source);
void materialChangeProperty(Material *material, const char *propertyName,
                            void *newData);
void materialAddProperty(Material *material, MaterialProperty *property);
void materialFree(Material *material);

#endif // !MATERIAL_H
