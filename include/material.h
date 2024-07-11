#ifndef MATERIAL_H
#define MATERIAL_H

// all types that can be defined as uniforms in shaders. if you want to add
// more, you need to define a case for your new type in
// `materialApplyProperties`
typedef enum {
    MATTYPE_INT,
    MATTYPE_FLOAT,
    MATTYPE_VEC2,
    MATTYPE_VEC3,
    MATTYPE_VEC4,
    MATTYPE_MAT2,
    MATTYPE_MAT3,
    MATTYPE_MAT4,
    MATTYPE_TEXTURE2D,
} MaterialType;

typedef struct {
    MaterialType Type;
    const char *Name;
    void *Data;
} MaterialProperty;
typedef struct {
    unsigned int TextureID;
    char Index;
} MaterialTextureData;

MaterialProperty *materialPropertyCreate(const char *name, MaterialType type,
                                         void *data);
void materialPropertyFree(MaterialProperty *property);
MaterialTextureData *materialTextureDataCreate(unsigned int texture, int index);
void materialTextureDataFree(MaterialTextureData *data);

typedef struct {
    unsigned int Shader;

    int PropertyCount;
    MaterialProperty **Properties;
} Material;

Material *materialCreate(unsigned int shader, int propertyCount, ...);
void materialApplyProperties(Material *material);
Material *materialCopy(Material *source);
void materialChangeProperty(Material *material, const char *propertyName,
                            void *newData);
void materialFree(Material *material);

#endif // !MATERIAL_H
