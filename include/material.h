#ifndef MATERIAL_H
#define MATERIAL_H

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
    const char *Name;
    void *Data;
} MaterialProperty;
typedef struct {
    unsigned int TextureID;
    char Index;
} MaterialTextureData;

/// returns material property to be used in `materialCreate`. free with
/// `materialPropertyFree`
/// @param void *`data`: pointer to data
MaterialProperty *materialPropertyCreate(const char *name,
                                         enum MaterialType type, void *data);
void materialPropertyFree(MaterialProperty *property);
/// returns material texture property data. free with `materialTextureDataFree`
MaterialTextureData *materialTextureDataCreate(unsigned int texture, int index);
void materialTextureDataFree(MaterialTextureData *data);

typedef struct {
    unsigned int Shader;

    int PropertyCount;
    MaterialProperty **Properties;
} Material;

/// returns material for use with models
/// @param int `propertyCount`: the number of properties to allocate for
/// @param MaterialProperty *...: x number of properties to set
Material *materialCreate(unsigned int shader, int propertyCount, ...);
void materialApplyProperties(Material *material);
Material *materialCopy(Material *source);
void materialChangeProperty(Material *material, const char *propertyName,
                            void *newData);
void materialFree(Material *material);

#endif // !MATERIAL_H
