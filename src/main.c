#include "armature.h"
#include "cglm/struct/affine-pre.h"
#include "cglm/struct/affine.h"
#include "window.h"

#include "camera.h"
#include "input.h"
#include "rendering.h"

#include "animation.h"
#include "error.h"
#include "material.h"
#include "model.h"
#include "shader.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MOVE_SPEED 10.0f
#define EVENT_COUNT 2

GLFWwindow *window;

vec2s mousePosition;
vec2s mouseDelta;
vec2s mouseSensitivity;

// just to make the main function more easily accessible
InputEvent *getInputEventArray();

int main(void) {
    window = windowCreate();
    windowSetSkybox(0.117f, 0.117f, 0.18f);
    InputEvent *events = getInputEventArray();
    inputSetEvents(events, 2);
    inputInit(window);
    errorInit();

    Camera camera =
        cameraCreate((vec3s){{0.0f, 1.0f, 1.0f}}, GLMS_QUAT_IDENTITY);
    cameraSetProjectionMatrixPersp(&camera, 60, 0.1f, 100.0f);
    cameraLookAt(&camera, GLMS_VEC3_ZERO);

    vec3s lightPos = (vec3s){{-2.2f, 1.2f, -0.6f}};
    vec3s lightAmbient = (vec3s){{0.2f, 0.2f, 0.2f}};
    vec3s lightDiffuse = (vec3s){{0.5f, 0.5f, 0.5f}};
    vec3s lightSpecular = (vec3s){{1.0f, 1.0f, 1.0f}};
    uint32_t lightShader =
        shaderCreate("light_source_vert.glsl", "light_source_frag.glsl");
    Model *light = modelLoad("light.glb", 0);
    MaterialProperty *matPropertyLightPos = materialPropertyCreate(
        "light.position", MATTYPE_VEC3, (void *)&lightPos);
    MaterialProperty *matPropertyLightAmbient = materialPropertyCreate(
        "light.ambient", MATTYPE_VEC3, (void *)&lightAmbient);
    MaterialProperty *matPropertyLightDiffuse = materialPropertyCreate(
        "light.diffuse", MATTYPE_VEC3, (void *)&lightDiffuse);
    MaterialProperty *matPropertyLightSpecular = materialPropertyCreate(
        "light.specular", MATTYPE_VEC3, (void *)&lightSpecular);
    light->Materials[0] =
        materialCreate(lightShader, 1, matPropertyLightDiffuse);
    light->WorldFromModel = glms_translate(GLMS_MAT4_IDENTITY, lightPos);

    uint32_t skinningShader =
        shaderCreate("skinning_vert.glsl", "light_affected_frag.glsl");
    Model *skinningModel =
        modelLoad("BrainStem.glb", MODELOPTS_IMPORT_MATERIALS);
    for (int i = 0; i < skinningModel->MaterialCount; i++) {
        skinningModel->Materials[i]->Shader = skinningShader;
        materialAddProperty(skinningModel->Materials[i], matPropertyLightPos);

        materialAddProperty(skinningModel->Materials[i],
                            matPropertyLightAmbient);
        materialAddProperty(skinningModel->Materials[i],
                            matPropertyLightDiffuse);
        materialAddProperty(skinningModel->Materials[i],
                            matPropertyLightSpecular);
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
    homeModel->Materials[0] = materialCreate(homeShader, 2, matPropertyLightPos,
                                             matPropertyLightDiffuse);
    modelSetDefaultMaterial(homeModel, homeModel->Materials[0]);

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

        lightPos.x = sinf(currentTime * M_PI) * 2.3f + 0.7f;
        lightPos.y = sinf(currentTime * M_PI * 0.8f) * 1.7f + 0.7f;
        lightPos.z = sinf(currentTime * M_PI * 1.3f) * 2.8f + -1.2f;
        lightDiffuse.x =
            (sinf(currentTime * M_PI / 4) * 0.5 + 0.5) * 0.9f + 0.6f;
        lightDiffuse.y =
            (sinf(currentTime * 0.7f / 4) * 0.5 + 0.5) * 0.9f + 0.6f;
        lightDiffuse.z =
            (sinf(currentTime * 1.3f / 4) * 0.5 + 0.5) * 0.9f + 0.6f;
        light->WorldFromModel = glms_translate(GLMS_MAT4_IDENTITY, lightPos);
        modelSetNodeWorldMatrices(light);
        modelRender(light);

        modelSetNodeWorldMatrices(skinningModel);
        armatureApplyTransformations(armature);
        modelRender(skinningModel);

        modelSetNodeWorldMatrices(homeModel);
        modelRender(homeModel);

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
    materialPropertyFree(matPropertyLightPos);
    materialPropertyFree(matPropertyLightAmbient);
    materialPropertyFree(matPropertyLightDiffuse);
    materialPropertyFree(matPropertyLightSpecular);
    materialFree(homeModel->Materials[0]);
    materialFree(light->Materials[0]);
    modelFree(skinningModel);
    modelFree(light);
    modelFree(homeModel);
    armatureFree(armature);
    shaderFreeCache();

    windowClose();

    free(events);
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
