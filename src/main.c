#include "window.h"

#include "animation.h"
#include "armature.h"
#include "camera.h"
#include "input.h"
#include "texture.h"
#ifdef ENABLE_ERRORCHECKING
#include "error.h"
#endif
#include "light.h"
#include "material.h"
#include "model.h"
#include "shader.h"

#include <cglm/struct/affine-pre.h>
#include <cglm/struct/affine.h>
#include <cglm/struct/vec3.h>
#include <math.h>
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

    Camera *camera = cameraCreate(GLMS_VEC3_ZERO, GLMS_QUAT_IDENTITY);
    cameraSetProjectionMatrixPersp(camera, 60, 0.1f, 100.0f);
    cameraLookAt(camera, GLMS_VEC3_ZERO);

    DirectionalLight *dirLights =
        malloc(DIRLIGHTS_MAX * sizeof(DirectionalLight));
    memset(dirLights, 0, DIRLIGHTS_MAX * sizeof(DirectionalLight));
    dirLights[0] = directionalLightCreate(
        glms_vec3_normalize(glms_vec3_negate((vec3s){{-3, -4, 7}})),
        (vec3s){{0.0f, 0.0f, 0.0f}}, (vec3s){{0.694f, 0.688f, 0.578f}},
        (vec3s){{0.7f, 0.7f, 0.7f}});

    PointLight *pointLights = malloc(POINTLIGHTS_MAX * sizeof(PointLight));
    memset(pointLights, 0, POINTLIGHTS_MAX * sizeof(PointLight));
    pointLights[0] = pointLightCreate(
        (vec3s){{-2.2f, 1.2f, -0.6f}}, (vec3s){{0.1f, 0.1f, 0.1f}},
        (vec3s){{0.2f, 0.2f, 0.2f}}, (vec3s){{1.0f, 1.0f, 1.0f}}, 1.0f, 10.0f,
        1.5f);

    SpotLight *spotLights = malloc(SPOTLIGHTS_MAX * sizeof(SpotLight));
    memset(spotLights, 0, SPOTLIGHTS_MAX * sizeof(SpotLight));
    spotLights[0] = spotLightCreate(
        (vec3s){{5.2f, -3.2f, -5.6f}},
        glms_vec3_normalize(glms_vec3_negate((vec3s){{-3, 2, 1}})),
        (vec3s){{0.1f, 0.1f, 0.1f}}, (vec3s){{1.0f, 0.2f, 1.0f}},
        (vec3s){{1.0f, 1.0f, 1.0f}}, 5.0f, 15.0f, 1.5f, 12.5f, 17.5f);

    LightScene *lightScene =
        lightSceneCreate(dirLights, pointLights, spotLights);

    Shader *lightShader =
        shaderCreate("light_source.vert", "light_source.frag");
    Model *pointLightModel = modelLoad("light.glb", 0);
    pointLightModel->Materials[0] =
        materialCreate(lightShader, 1,
                       materialPropertyCreate("diffuse", MATTYPE_VEC3,
                                              (void *)&pointLights[0].Diffuse));
    pointLightModel->WorldFromModel =
        glms_translate(GLMS_MAT4_IDENTITY, pointLights[0].Position);

    Model *directionalLightModel = modelLoad("arrow.glb", 0);
    directionalLightModel->Materials[0] =
        materialCreate(lightShader, 1,
                       materialPropertyCreate("diffuse", MATTYPE_VEC3,
                                              (void *)&dirLights[0].Diffuse));
    vec3s startAxis = (vec3s){{1, 0, 0}};
    float angle = acos(glms_vec3_dot(startAxis, dirLights[0].Direction));
    vec3s axis = glms_vec3_cross(startAxis, dirLights[0].Direction);
    directionalLightModel->WorldFromModel =
        glms_rotate(GLMS_MAT4_IDENTITY, angle, axis);

    Model *spotLightModel = modelLoad("flashlight.glb", 0);
    spotLightModel->Materials[0] =
        materialCreate(lightShader, 1,
                       materialPropertyCreate("diffuse", MATTYPE_VEC3,
                                              (void *)&spotLights[0].Diffuse));
    angle = acos(glms_vec3_dot(startAxis, spotLights[0].Direction));
    axis = glms_vec3_cross(startAxis, spotLights[0].Direction);
    spotLightModel->WorldFromModel =
        glms_rotate(glms_translate(GLMS_MAT4_IDENTITY, spotLights[0].Position),
                    angle, axis);

    Shader *homeShader =
        shaderCreate("light_affected.vert", "light_affected.frag");
    Model *homeModel = modelLoad("home.glb", 0);
    for (int i = 0; i < homeModel->MaterialCount; i++) {
        homeModel->Materials[i]->Shader = homeShader;
    }

    Shader *skinningShader =
        shaderCreate("skinning.vert", "light_affected.frag");
    Model *skinningModel = modelLoad("BrainStem.glb", 0);
    for (int i = 0; i < skinningModel->MaterialCount; i++) {
        skinningModel->Materials[i]->Shader = skinningShader;
    }
    skinningModel->WorldFromModel = glms_translate(
        glms_scale(glms_rotate_x(glms_rotate_z(GLMS_MAT4_IDENTITY, glm_rad(90)),
                                 glm_rad(180)),
                   (vec3s){{1.5f, 1.5f, 1.5f}}),
        (vec3s){{-3.0f, 0.0f, -1.4f}});
    Armature *armature = armatureCreate(skinningModel);

    const int TexturedBoxCount = 7;
    Model **texturedBoxes = malloc(TexturedBoxCount * sizeof(Model *));
    Shader *texturedShader =
        shaderCreate("light_affected.vert", "light_affected.frag");
    for (int i = 0; i < TexturedBoxCount; i++) {
        texturedBoxes[i] = modelLoad("BoxTextured.glb", 0);
        for (int j = 0; j < texturedBoxes[i]->MaterialCount; j++) {
            texturedBoxes[i]->Materials[j]->Shader = texturedShader;
        }
        texturedBoxes[i]->WorldFromModel = glms_translate(
            glms_rotate_y(GLMS_MAT4_IDENTITY, glm_rad(23)),
            (vec3s){{-1.7f + (float)i / ((TexturedBoxCount - 1) * 2), -4.0f + i,
                     -4.4f}});
    }

    //
    uint32_t depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);

    int maxTextureSize;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
    maxTextureSize /= 4;
    const int SHADOW_WIDTH = maxTextureSize, SHADOW_HEIGHT = maxTextureSize;
    uint32_t depthMapTexture;
    glGenTextures(1, &depthMapTexture);
    glBindTexture(GL_TEXTURE_2D, depthMapTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH,
                 SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
                           depthMapTexture, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    Shader *depthShader = shaderCreate("depth.vert", "depth.frag");
    Shader *depthSkinningShader =
        shaderCreate("depth_skinning.vert", "depth.frag");

    MaterialProperty *shadowMapProperty = materialPropertyCreate(
        "shadowMap", MATTYPE_TEXTURE2D,
        materialTextureDataCreate((Texture *)&depthMapTexture, 10));
    for (int i = 0; i < homeModel->MaterialCount; i++) {
        materialAddProperty(homeModel->Materials[i], shadowMapProperty);
    }
    for (int i = 0; i < skinningModel->MaterialCount; i++) {
        materialAddProperty(skinningModel->Materials[i], shadowMapProperty);
    }
    for (int boxId = 0; boxId < TexturedBoxCount; boxId++) {
        for (int i = 0; i < texturedBoxes[boxId]->MaterialCount; i++) {
            materialAddProperty(texturedBoxes[boxId]->Materials[i],
                                shadowMapProperty);
        }
    }
    mat4s lightProjectionFromViewMatrix = glms_ortho(-20, 20, -20, 20, -20, 20);
    mat4s lightViewFromWorldMatrix =
        glms_lookat(dirLights[0].Direction, GLMS_VEC3_ZERO, (vec3s){{0, 1, 0}});
    mat4s lightProjectionFromWorld =
        glms_mul(lightProjectionFromViewMatrix, lightViewFromWorldMatrix);

    glBindBuffer(GL_UNIFORM_BUFFER, camera->MatricesUBO);
    glBufferSubData(GL_UNIFORM_BUFFER, 64, 64, &lightProjectionFromWorld);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    //

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
        cameraSetEulerAngles(camera, eulerAngles);

        movementInput = (vec3s){{
            glm_clamp(movementEvent->State & (1 << 0), -1, 1) -
                glm_clamp(movementEvent->State & (1 << 1), -1, 1),
            glm_clamp(movementEvent->State & (1 << 2), -1, 1) -
                glm_clamp(movementEvent->State & (1 << 3), -1, 1),
            glm_clamp(movementEvent->State & (1 << 4), -1, 1) -
                glm_clamp(movementEvent->State & (1 << 5), -1, 1),
        }};
        positionDelta = (vec3s){{movementInput.x, 0, -movementInput.z}};
        positionDelta = glms_quat_rotatev(camera->Quaternion, positionDelta);
        positionDelta =
            glms_vec3_add(positionDelta, (vec3s){{0, movementInput.y, 0}});
        camera->Position = glms_vec3_add(
            camera->Position,
            glms_vec3_scale(positionDelta, MOVE_SPEED * deltaTime));
        cameraCalculateViewMatrix(camera);

        pointLights[0].Position.x = sinf(currentTime * M_PI) * 2.3f + 0.7f;
        pointLights[0].Position.y =
            sinf(currentTime * M_PI * 0.8f) * 1.7f + 0.7f;
        pointLights[0].Position.z =
            sinf(currentTime * M_PI * 1.3f) * 2.8f + -1.2f;
        pointLights[0].Diffuse.x =
            (sinf(currentTime * M_PI / 4) * 0.5 + 0.5) * 0.9f + 0.6f;
        pointLights[0].Diffuse.y =
            (sinf(currentTime * 0.7f / 4) * 0.5 + 0.5) * 0.9f + 0.6f;
        pointLights[0].Diffuse.z =
            (sinf(currentTime * 1.3f / 4) * 0.5 + 0.5) * 0.9f + 0.6f;
        pointLightModel->WorldFromModel =
            glms_translate(GLMS_MAT4_IDENTITY, pointLights[0].Position);

        vec3s startAxis = (vec3s){{1, 0, 0}};
        float angle = acos(glms_vec3_dot(startAxis, dirLights[0].Direction));
        vec3s axis = glms_vec3_cross(startAxis, dirLights[0].Direction);
        directionalLightModel->WorldFromModel =
            glms_rotate(GLMS_MAT4_IDENTITY, angle, axis);

        lastTime = currentTime;

        lightScenePreRender(lightScene);
        cameraPreRender(camera);

        //
        meshOverrideShaders(depthShader->ID);
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        glCullFace(GL_FRONT);

        ///
        modelPreRender(pointLightModel);
        modelRender(pointLightModel);

        modelPreRender(directionalLightModel);
        modelRender(directionalLightModel);

        modelPreRender(spotLightModel);
        modelRender(spotLightModel);

        meshOverrideShaders(depthSkinningShader->ID);
        modelPreRender(skinningModel);
        armatureApplyTransformations(armature);
        glUseProgram(depthSkinningShader->ID);
        glUniformMatrix4fv(glGetUniformLocation(depthSkinningShader->ID,
                                                "_boneTransformations"),
                           MAX_BONES, GL_FALSE,
                           (void *)&armature->BoneTransformations);
        modelRender(skinningModel);
        meshOverrideShaders(depthShader->ID);

        modelPreRender(homeModel);
        modelRender(homeModel);

        for (int i = 0; i < TexturedBoxCount; i++) {
            modelPreRender(texturedBoxes[i]);
            modelRender(texturedBoxes[i]);
        }
        ///
        glCullFace(GL_BACK);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
        glActiveTexture(GL_TEXTURE10);
        glBindTexture(GL_TEXTURE_2D, depthMapTexture);
        meshOverrideShaders(-1);
        //

        modelRender(pointLightModel);
        modelRender(directionalLightModel);
        modelRender(spotLightModel);
        modelRender(skinningModel);
        modelRender(homeModel);
        for (int i = 0; i < TexturedBoxCount; i++) {
            modelRender(texturedBoxes[i]);
        }

        windowDraw(window);
    }

    for (int i = 0; i < skinningModel->MaterialCount; i++) {
        Material *material = skinningModel->Materials[i];
        for (int j = 0; j < material->PropertyCount; j++) {
            MaterialProperty *property = material->Properties[j];
            if (strncmp(property->Name, "_material.", 9) == 0) {
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
        materialFree(texturedBoxes[i]->Materials[0]);
        modelFree(texturedBoxes[i]);
    }
    free(texturedBoxes);
    armatureFree(armature);

    free(pointLights);
    free(dirLights);
    lightSceneFree(lightScene);

    textureFreeCache();
    shaderFreeCache();

    windowClose();

    cameraFree(camera);

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
