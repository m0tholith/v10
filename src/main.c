#include "armature.h"
#include "cglm/struct/affine-pre.h"
#include "cglm/struct/affine.h"
#include "cglm/struct/vec3.h"
#include "texture.h"
#include "window.h"

#include "camera.h"
#include "input.h"
#include "rendering.h"

#include "animation.h"
#ifdef ENABLE_ERRORCHECKING
#include "error.h"
#endif
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
#ifdef ENABLE_ERRORCHECKING
    errorInit();
#endif

    Camera camera =
        cameraCreate((vec3s){{0.0f, 1.0f, 1.0f}}, GLMS_QUAT_IDENTITY);
    cameraSetProjectionMatrixPersp(&camera, 60, 0.1f, 100.0f);
    cameraLookAt(&camera, GLMS_VEC3_ZERO);

    vec3s pointLightPos = (vec3s){{-2.2f, 1.2f, -0.6f}};
    vec3s pointLightAmbient = (vec3s){{0.1f, 0.1f, 0.1f}};
    vec3s pointLightDiffuse = (vec3s){{0.2f, 0.2f, 0.2f}};
    vec3s pointLightSpecular = (vec3s){{1.0f, 1.0f, 1.0f}};
    float pointLightIntensity = 1;
    float pointLightDistance = 10;
    float pointLightDecay = 1.5f;
    MaterialProperty *matPropertyPointLightPos = materialPropertyCreate(
        "pointLight.position", MATTYPE_VEC3, (void *)&pointLightPos);
    MaterialProperty *matPropertyPointLightAmbient = materialPropertyCreate(
        "pointLight.ambient", MATTYPE_VEC3, (void *)&pointLightAmbient);
    MaterialProperty *matPropertyPointLightDiffuse = materialPropertyCreate(
        "pointLight.diffuse", MATTYPE_VEC3, (void *)&pointLightDiffuse);
    MaterialProperty *matPropertyPointLightSpecular = materialPropertyCreate(
        "pointLight.specular", MATTYPE_VEC3, (void *)&pointLightSpecular);
    MaterialProperty *matPropertyPointLightIntensity = materialPropertyCreate(
        "pointLight.intensity", MATTYPE_FLOAT, (void *)&pointLightIntensity);
    MaterialProperty *matPropertyPointLightDistance = materialPropertyCreate(
        "pointLight.distance", MATTYPE_FLOAT, (void *)&pointLightDistance);
    MaterialProperty *matPropertyPointLightDecay = materialPropertyCreate(
        "pointLight.decay", MATTYPE_FLOAT, (void *)&pointLightDecay);
    vec3s directionalLightDir = glms_vec3_normalize((vec3s){{3, 4, -12}});
    vec3s directionalLightAmbient = (vec3s){{0, 0, 0}};
    vec3s directionalLightDiffuse = (vec3s){{0.7f, 0.7f, 0.7f}};
    vec3s directionalLightSpecular = (vec3s){{0.7f, 0.7f, 0.7f}};
    MaterialProperty *matPropertyDirectionalLightDir =
        materialPropertyCreate("directionalLight.direction", MATTYPE_VEC3,
                               (void *)&directionalLightDir);
    MaterialProperty *matPropertyDirectionalLightAmbient =
        materialPropertyCreate("directionalLight.ambient", MATTYPE_VEC3,
                               (void *)&directionalLightAmbient);
    MaterialProperty *matPropertyDirectionalLightDiffuse =
        materialPropertyCreate("directionalLight.diffuse", MATTYPE_VEC3,
                               (void *)&directionalLightDiffuse);
    MaterialProperty *matPropertyDirectionalLightSpecular =
        materialPropertyCreate("directionalLight.specular", MATTYPE_VEC3,
                               (void *)&directionalLightSpecular);

    uint32_t lightShader =
        shaderCreate("light_source_vert.glsl", "light_source_frag.glsl");
    Model *pointLight = modelLoad("light.glb", 0);
    pointLight->Materials[0] =
        materialCreate(lightShader, 1,
                       materialPropertyCreate("diffuse", MATTYPE_VEC3,
                                              (void *)&pointLightDiffuse));
    pointLight->WorldFromModel =
        glms_translate(GLMS_MAT4_IDENTITY, pointLightPos);

    Model *directionalLight = modelLoad("arrow.glb", 0);
    directionalLight->Materials[0] = materialCreate(
        lightShader, 1,
        materialPropertyCreate("diffuse", MATTYPE_VEC3,
                               (void *)&directionalLightDiffuse));
    directionalLight->WorldFromModel =
        glms_rotate_z(glms_rotate_y(glms_rotate_x(GLMS_MAT4_IDENTITY,
                                                  atan2(directionalLightDir.y,
                                                        directionalLightDir.x)),
                                    asin(directionalLightDir.z)),
                      0);

    uint32_t skinningShader =
        shaderCreate("skinning_vert.glsl", "light_affected_frag.glsl");
    Model *skinningModel =
        modelLoad("BrainStem.glb", MODELOPTS_IMPORT_MATERIALS);
    for (int i = 0; i < skinningModel->MaterialCount; i++) {
        skinningModel->Materials[i]->Shader = skinningShader;
        materialAddProperty(skinningModel->Materials[i],
                            matPropertyPointLightPos);
        materialAddProperty(skinningModel->Materials[i],
                            matPropertyPointLightAmbient);
        materialAddProperty(skinningModel->Materials[i],
                            matPropertyPointLightDiffuse);
        materialAddProperty(skinningModel->Materials[i],
                            matPropertyPointLightSpecular);
        materialAddProperty(skinningModel->Materials[i],
                            matPropertyPointLightIntensity);
        materialAddProperty(skinningModel->Materials[i],
                            matPropertyPointLightDistance);
        materialAddProperty(skinningModel->Materials[i],
                            matPropertyPointLightDecay);

        materialAddProperty(skinningModel->Materials[i],
                            matPropertyDirectionalLightDir);
        materialAddProperty(skinningModel->Materials[i],
                            matPropertyDirectionalLightAmbient);
        materialAddProperty(skinningModel->Materials[i],
                            matPropertyDirectionalLightDiffuse);
        materialAddProperty(skinningModel->Materials[i],
                            matPropertyDirectionalLightSpecular);
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
    homeModel->Materials[0] = materialCreate(
        homeShader, 10, matPropertyPointLightPos, matPropertyPointLightAmbient,
        matPropertyPointLightDiffuse, matPropertyPointLightSpecular,
        matPropertyPointLightIntensity, matPropertyPointLightDistance,
        matPropertyPointLightDecay, matPropertyDirectionalLightDir,
        matPropertyDirectionalLightAmbient, matPropertyDirectionalLightDiffuse,
        matPropertyDirectionalLightSpecular);
    modelSetDefaultMaterial(homeModel, homeModel->Materials[0]);

    const int TexturedBoxCount = 7;
    Model **texturedBoxes = malloc(TexturedBoxCount * sizeof(Model *));
    uint32_t texturedShader = shaderCreate("light_affected_vert.glsl",
                                           "light_affected_tex_frag.glsl");
    for (int i = 0; i < TexturedBoxCount; i++) {
        texturedBoxes[i] = modelLoad("BoxTextured.glb", 0);
        texturedBoxes[i]->Materials[0] = materialCreate(
            texturedShader, 11,
            materialPropertyCreate("_diffuseTex", MATTYPE_TEXTURE2D,
                                   (void *)materialTextureDataCreate(
                                       texturedBoxes[i]->Textures[0], 0)),
            matPropertyPointLightPos, matPropertyPointLightAmbient,
            matPropertyPointLightDiffuse, matPropertyPointLightSpecular,
            matPropertyPointLightIntensity, matPropertyPointLightDistance,
            matPropertyPointLightDecay, matPropertyDirectionalLightDir,
            matPropertyDirectionalLightAmbient,
            matPropertyDirectionalLightDiffuse,
            matPropertyDirectionalLightSpecular);
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

        pointLightPos.x = sinf(currentTime * M_PI) * 2.3f + 0.7f;
        pointLightPos.y = sinf(currentTime * M_PI * 0.8f) * 1.7f + 0.7f;
        pointLightPos.z = sinf(currentTime * M_PI * 1.3f) * 2.8f + -1.2f;
        pointLightDiffuse.x =
            (sinf(currentTime * M_PI / 4) * 0.5 + 0.5) * 0.9f + 0.6f;
        pointLightDiffuse.y =
            (sinf(currentTime * 0.7f / 4) * 0.5 + 0.5) * 0.9f + 0.6f;
        pointLightDiffuse.z =
            (sinf(currentTime * 1.3f / 4) * 0.5 + 0.5) * 0.9f + 0.6f;
        pointLight->WorldFromModel =
            glms_translate(GLMS_MAT4_IDENTITY, pointLightPos);
        modelSetNodeWorldMatrices(pointLight);
        modelRender(pointLight);
        modelSetNodeWorldMatrices(directionalLight);
        modelRender(directionalLight);

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
    materialPropertyFree(matPropertyPointLightPos);
    materialPropertyFree(matPropertyPointLightAmbient);
    materialPropertyFree(matPropertyPointLightDiffuse);
    materialPropertyFree(matPropertyPointLightSpecular);
    materialPropertyFree(matPropertyPointLightIntensity);
    materialPropertyFree(matPropertyPointLightDistance);
    materialPropertyFree(matPropertyPointLightDecay);
    materialPropertyFree(matPropertyDirectionalLightDir);
    materialPropertyFree(matPropertyDirectionalLightAmbient);
    materialPropertyFree(matPropertyDirectionalLightDiffuse);
    materialPropertyFree(matPropertyDirectionalLightSpecular);
    materialFree(homeModel->Materials[0]);
    materialPropertyFree(pointLight->Materials[0]->Properties[0]);
    materialFree(pointLight->Materials[0]);
    materialPropertyFree(directionalLight->Materials[0]->Properties[0]);
    materialFree(directionalLight->Materials[0]);
    modelFree(skinningModel);
    modelFree(pointLight);
    modelFree(directionalLight);
    modelFree(homeModel);
    for (int i = 0; i < TexturedBoxCount; i++) {
        materialPropertyFree(texturedBoxes[i]->Materials[0]->Properties[0]);
        materialFree(texturedBoxes[i]->Materials[0]);
        modelFree(texturedBoxes[i]);
    }
    armatureFree(armature);

    textureFreeCache();
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
