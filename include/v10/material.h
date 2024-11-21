#ifndef MATERIAL_H
#define MATERIAL_H

#include "v10/shader.h"
#include "v10/texture.h"

/// all types that can be defined as uniforms in shaders. if you want to add
/// more, you need to define a case for your new type in
/// `materialApplyProperties`.
///
/// `MATTYPE_TEXTURE2D` is used with `MaterialTextureData` type
enum MaterialPropertyType {
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
    enum MaterialPropertyType Type;
    char *Name;
    void *Data;
} MaterialProperty;
typedef struct {
    Texture *Texture;
    char Index;
} MaterialTextureData;

/**
 * Creates a material property.
 *
 * This is typically used in materialCreate().
 *
 * @param: void *data Pointer to data to be supplied into shader. MUST be a
 *  pointer, no matter what type of property you want to send.
 * @return: MaterialProperty * New material property.
 */
MaterialProperty *materialPropertyCreate(const char *name,
                                         enum MaterialPropertyType type,
                                         void *data);
/// frees property and its name
void materialPropertyFree(MaterialProperty *property);
/// frees property and its name, along with its data
///
/// use this e.x. for freeing properties imported by model (_diffuse, etc.)
void materialPropertyDelete(MaterialProperty *property);
/// @note: Free with `materialTextureDataFree`.
MaterialTextureData *materialTextureDataCreate(Texture *texture, int index);
void materialTextureDataFree(MaterialTextureData *data);

typedef struct {
    Shader *Shader;

    int PropertyCount;
    MaterialProperty **Properties;
} Material;

/// @return: material for use with models
/// @param: int `propertyCount` The number of properties to allocate for
/// @param: MaterialProperty *... X number of properties to set
Material *materialCreate(Shader *shader, int propertyCount, ...);
void materialApplyProperties(Material *material);
Material *materialCopy(Material *source);
void materialChangeProperty(Material *material, const char *propertyName,
                            void *newData);
void materialAddProperty(Material *material, MaterialProperty *property);
void materialFree(Material *material);

#endif // !MATERIAL_H
