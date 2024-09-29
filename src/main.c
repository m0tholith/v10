#include "armature.h"
#include "texture.h"
#include "window.h"
#include <cglm/struct/affine-pre.h>
#include <cglm/struct/affine.h>
#include <cglm/struct/vec3.h>

#include "camera.h"
#include "input.h"

#include "animation.h"
#ifdef ENABLE_ERRORCHECKING
#include "error.h"
#endif
#include "material.h"
#include "model.h"
#include "shader.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

#define MOVE_SPEED 10.0f
#define EVENT_COUNT 2

struct DirectionalLight {
    vec4s direction; // 16, 0, 16
    vec4s ambient;   // 16, 16, 32
    vec4s diffuse;   // 16, 32, 48
    vec4s specular;  // 16, 48, 64
};
struct PointLight {
    vec4s position; // 16, 64, 80
    vec4s ambient;  // 16, 80, 96
    vec4s diffuse;  // 16, 96, 112
    vec4s specular; // 16, 112, 128

    // 16, 128, 144
    float intensity;
    float distance;
    float decay;
    float padding;
};
uint32_t lightsUBO;

GLFWwindow *window;

vec2s mousePosition;
vec2s mouseDelta;
vec2s mouseSensitivity;

// just to make the main function more easily accessible
InputEvent *getInputEventArray();
void freeInputEventArray(InputEvent *events);

int main(void) {
    window = windowCreate();
    windowSetSkybox(0.117f, 0.117f, 0.18f);
    InputEvent *events = getInputEventArray();
    inputSetEvents(events, 2);
    inputInit(window);
#ifdef ENABLE_ERRORCHECKING
    errorInit();
#endif

    Camera camera = cameraCreate(GLMS_VEC3_ZERO, GLMS_QUAT_IDENTITY);
    cameraSetProjectionMatrixPersp(&camera, 60, 0.1f, 100.0f);
    cameraLookAt(&camera, GLMS_VEC3_ZERO);

    // set up light ubo
    glGenBuffers(1, &lightsUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, lightsUBO);
    glBufferData(GL_UNIFORM_BUFFER, 64 * 1 + 76 * 1, NULL, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, lightsUBO);

    struct DirectionalLight *directionalLight =
        malloc(sizeof(struct DirectionalLight));
    *directionalLight = (struct DirectionalLight){
        .direction =
            glms_vec4_normalize(glms_vec4_negate((vec4s){{-3, -4, 7}})),
        .ambient = (vec4s){{0, 0, 0}},
        .diffuse = (vec4s){{0.694f, 0.5f, 0.05f}},
        .specular = (vec4s){{0.7f, 0.7f, 0.7f}},
    };
    struct PointLight *pointLight = malloc(sizeof(struct PointLight));
    *pointLight = (struct PointLight){
        .position = (vec4s){{-2.2f, 1.2f, -0.6f}},
        .ambient = (vec4s){{0.1f, 0.1f, 0.1f}},
        .diffuse = (vec4s){{0.2f, 0.2f, 0.2f}},
        .specular = (vec4s){{1.0f, 1.0f, 1.0f}},
        .intensity = 1,
        .distance = 10,
        .decay = 1.5f,
    };

    glBufferSubData(GL_UNIFORM_BUFFER, 0, 64, directionalLight);
    glBufferSubData(GL_UNIFORM_BUFFER, 64, 76, pointLight);

    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    uint32_t lightShader =
        shaderCreate("light_source_vert.glsl", "light_source_frag.glsl");
    Model *pointLightModel = modelLoad("light.glb", 0);
    pointLightModel->Materials[0] =
        materialCreate(lightShader, 1,
                       materialPropertyCreate("diffuse", MATTYPE_VEC3,
                                              (void *)&pointLight->diffuse));
    pointLightModel->WorldFromModel =
        glms_translate(GLMS_MAT4_IDENTITY, glms_vec3(pointLight->position));

    Model *directionalLightModel = modelLoad("arrow.glb", 0);
    directionalLightModel->Materials[0] = materialCreate(
        lightShader, 1,
        materialPropertyCreate("diffuse", MATTYPE_VEC3,
                               (void *)&directionalLight->diffuse));
    vec3s startAxis = (vec3s){{1, 0, 0}};
    float angle =
        acos(glms_vec3_dot(startAxis, glms_vec3(directionalLight->direction)));
    vec3s axis =
        glms_vec3_cross(startAxis, glms_vec3(directionalLight->direction));
    directionalLightModel->WorldFromModel =
        glms_rotate(GLMS_MAT4_IDENTITY, angle, axis);
    uint32_t skinningShader =
        shaderCreate("skinning_vert.glsl", "light_affected_frag.glsl");
    Model *skinningModel =
        modelLoad("BrainStem.glb", MODELOPTS_IMPORT_MATERIALS);
    for (int i = 0; i < skinningModel->MaterialCount; i++) {
        skinningModel->Materials[i]->Shader = skinningShader;
    }
    skinningModel->WorldFromModel = glms_translate(
        glms_scale(glms_rotate_x(glms_rotate_z(GLMS_MAT4_IDENTITY, glm_rad(90)),
                                 glm_rad(180)),
                   (vec3s){{1.5f, 1.5f, 1.5f}}),
        (vec3s){{-3.0f, 0.0f, -1.4f}});
    Armature *armature = armatureCreate(skinningModel);

    uint32_t homeShader =
        shaderCreate("light_affected_vert.glsl", "light_affected_frag.glsl");
    Model *homeModel = modelLoad("home.glb", 0);
    homeModel->Materials[0] = materialCreate(homeShader, 0);
    modelSetDefaultMaterial(homeModel, homeModel->Materials[0]);

    const int TexturedBoxCount = 7;
    Model **texturedBoxes = malloc(TexturedBoxCount * sizeof(Model *));
    uint32_t texturedShader = shaderCreate("light_affected_vert.glsl",
                                           "light_affected_tex_frag.glsl");
    for (int i = 0; i < TexturedBoxCount; i++) {
        texturedBoxes[i] = modelLoad("BoxTextured.glb", 0);
        texturedBoxes[i]->Materials[0] = materialCreate(
            texturedShader, 1,
            materialPropertyCreate("_diffuseTex", MATTYPE_TEXTURE2D,
                                   (void *)materialTextureDataCreate(
                                       texturedBoxes[i]->Textures[0], 0)));
        modelSetDefaultMaterial(texturedBoxes[i],
                                texturedBoxes[i]->Materials[0]);
        texturedBoxes[i]->WorldFromModel = glms_translate(
            glms_rotate_y(GLMS_MAT4_IDENTITY, glm_rad(23)),
            (vec3s){{-1.7f + (float)i / ((TexturedBoxCount - 1) * 2), -4.0f + i,
                     -4.4f}});
    }

    glEnable(GL_CULL_FACE);

    float lastTime = 0, currentTime = 0, deltaTime = 0;
    vec3s eulerAngles = GLMS_VEC3_ZERO;
    vec3s movementInput = GLMS_VEC3_ZERO;
    vec3s positionDelta = GLMS_VEC3_ZERO;

    InputEvent *movementEvent = inputGetEvent("movement");
    InputEvent *exitEvent = inputGetEvent("exit");
    while (!glfwWindowShouldClose(window)) {
        currentTime = glfwGetTime();
        deltaTime = currentTime - lastTime;

        for (int i = 0; i < skinningModel->AnimationCount; i++) {
            animationStep(skinningModel->Animations[i], deltaTime);
        }
        for (int i = 0; i < homeModel->AnimationCount; i++) {
            animationStep(homeModel->Animations[i], deltaTime);
        }

        inputUpdate();
        if (exitEvent->State > 0)
            glfwSetWindowShouldClose(window, 1);
        inputMouseUpdate();
        eulerAngles = glms_vec3_add(
            eulerAngles,
            (vec3s){{-mouseDelta.y * mouseSensitivity.x * deltaTime,
                     -mouseDelta.x * mouseSensitivity.y * deltaTime, 0}});
        cameraSetEulerAngles(&camera, eulerAngles);

        movementInput = (vec3s){{
            glm_clamp(movementEvent->State & (1 << 0), -1, 1) -
                glm_clamp(movementEvent->State & (1 << 1), -1, 1),
            glm_clamp(movementEvent->State & (1 << 2), -1, 1) -
                glm_clamp(movementEvent->State & (1 << 3), -1, 1),
            glm_clamp(movementEvent->State & (1 << 4), -1, 1) -
                glm_clamp(movementEvent->State & (1 << 5), -1, 1),
        }};
        positionDelta = (vec3s){{movementInput.x, 0, -movementInput.z}};
        positionDelta = glms_quat_rotatev(camera.Quaternion, positionDelta);
        positionDelta =
            glms_vec3_add(positionDelta, (vec3s){{0, movementInput.y, 0}});
        camera.Position = glms_vec3_add(
            camera.Position,
            glms_vec3_scale(positionDelta, MOVE_SPEED * deltaTime));
        cameraCalculateViewMatrix(&camera);

        lastTime = currentTime;

        cameraPreRender(&camera);

        glBindBuffer(GL_UNIFORM_BUFFER, lightsUBO);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, 64, directionalLight);
        glBufferSubData(GL_UNIFORM_BUFFER, 64, 76, pointLight);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        pointLight->position.x = sinf(currentTime * M_PI) * 2.3f + 0.7f;
        pointLight->position.y = sinf(currentTime * M_PI * 0.8f) * 1.7f + 0.7f;
        pointLight->position.z = sinf(currentTime * M_PI * 1.3f) * 2.8f + -1.2f;
        pointLight->diffuse.x =
            (sinf(currentTime * M_PI / 4) * 0.5 + 0.5) * 0.9f + 0.6f;
        pointLight->diffuse.y =
            (sinf(currentTime * 0.7f / 4) * 0.5 + 0.5) * 0.9f + 0.6f;
        pointLight->diffuse.z =
            (sinf(currentTime * 1.3f / 4) * 0.5 + 0.5) * 0.9f + 0.6f;
        pointLightModel->WorldFromModel =
            glms_translate(GLMS_MAT4_IDENTITY, glms_vec3(pointLight->position));
        modelSetNodeWorldMatrices(pointLightModel);
        modelRender(pointLightModel);

        vec3s startAxis = (vec3s){{1, 0, 0}};
        float angle = acos(
            glms_vec3_dot(startAxis, glms_vec3(directionalLight->direction)));
        vec3s axis =
            glms_vec3_cross(startAxis, glms_vec3(directionalLight->direction));
        directionalLightModel->WorldFromModel =
            glms_rotate(GLMS_MAT4_IDENTITY, angle, axis);
        modelSetNodeWorldMatrices(directionalLightModel);
        modelRender(directionalLightModel);

        modelSetNodeWorldMatrices(skinningModel);
        armatureApplyTransformations(armature);
        modelRender(skinningModel);

        modelSetNodeWorldMatrices(homeModel);
        modelRender(homeModel);

        for (int i = 0; i < TexturedBoxCount; i++) {
            modelSetNodeWorldMatrices(texturedBoxes[i]);
            modelRender(texturedBoxes[i]);
        }

        windowDraw(window);
    }

    for (int i = 0; i < skinningModel->MaterialCount; i++) {
        Material *material = skinningModel->Materials[i];
        for (int j = 0; j < material->PropertyCount; j++) {
            MaterialProperty *property = material->Properties[j];
            if (strncmp(property->Name, "_material", 9) == 0) {
                materialPropertyDelete(property);
            }
        }
        materialFree(material);
    }
    materialFree(homeModel->Materials[0]);
    materialPropertyFree(pointLightModel->Materials[0]->Properties[0]);
    materialFree(pointLightModel->Materials[0]);
    materialPropertyFree(directionalLightModel->Materials[0]->Properties[0]);
    materialFree(directionalLightModel->Materials[0]);
    modelFree(skinningModel);
    modelFree(pointLightModel);
    modelFree(directionalLightModel);
    modelFree(homeModel);
    for (int i = 0; i < TexturedBoxCount; i++) {
        materialTextureDataFree(
            texturedBoxes[i]->Materials[0]->Properties[0]->Data);
        materialPropertyFree(texturedBoxes[i]->Materials[0]->Properties[0]);
        materialFree(texturedBoxes[i]->Materials[0]);
        modelFree(texturedBoxes[i]);
    }
    free(texturedBoxes);
    armatureFree(armature);

    glDeleteBuffers(1, &lightsUBO);
    free(directionalLight);
    free(pointLight);

    textureFreeCache();
    shaderFreeCache();

    windowClose();

    freeInputEventArray(events);
    return 0;
}

InputEvent *getInputEventArray() {
    InputEvent *events = malloc(EVENT_COUNT * sizeof(InputEvent));

    events[0] = (InputEvent){
        .Name = "movement",
        .Type = INPUTEVENT_AXIS,
        .KeyCount = 6,
    };
    events[0].Keys = malloc(6 * sizeof(struct InputKey));
    // done in this particular order so every two values are the positive and
    // negative ends of x,y,z axis
    events[0].Keys[0] = (struct InputKey){
        .Value = GLFW_KEY_D,
    };
    events[0].Keys[1] = (struct InputKey){
        .Value = GLFW_KEY_A,
    };
    events[0].Keys[2] = (struct InputKey){
        .Value = GLFW_KEY_E,
    };
    events[0].Keys[3] = (struct InputKey){
        .Value = GLFW_KEY_Q,
    };
    events[0].Keys[4] = (struct InputKey){
        .Value = GLFW_KEY_W,
    };
    events[0].Keys[5] = (struct InputKey){
        .Value = GLFW_KEY_S,
    };

    events[1] = (InputEvent){
        .Name = "exit",
        .Type = INPUTEVENT_BUTTON,
        .KeyCount = 1,
    };
    events[1].Keys = malloc(1 * sizeof(struct InputKey));
    events[1].Keys[0] = (struct InputKey){
        .Value = GLFW_KEY_ESCAPE,
    };

    return events;
}
void freeInputEventArray(InputEvent *events) {
    for (int i = 0; i < EVENT_COUNT; i++) {
        free(events[i].Keys);
    }
    free(events);
}
