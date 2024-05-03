#ifndef MATERIAL_H
#define MATERIAL_H

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
    char *Name;
    void (*PreRender)(void *materialProperty);
    void *Data;
} MaterialProperty;
typedef struct {
    char Index;
    unsigned int TextureID;
} MaterialTextureData;

MaterialProperty *materialPropertyCreate(char *name, MaterialType type,
                                         void *data);
void materialPropertyDestroy(MaterialProperty *property);

typedef struct {
    unsigned int Shader;

    int PropertyCount;
    MaterialProperty **Properties;
} Material;

Material *materialCreate(unsigned int shader, int propertyCount);
void materialPreRender(Material *material);
void materialApplyProperties(Material *material);

#endif // !MATERIAL_H
