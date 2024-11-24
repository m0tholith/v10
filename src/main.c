#include "v10/window.h"

#ifdef ENABLE_ERRORCHECKING
#include "v10/error.h"
#endif
#include "v10/animation.h"
#include "v10/armature.h"
#include "v10/camera.h"
#include "v10/cubemap.h"
#include "v10/entity.h"
#include "v10/framebuffer.h"
#include "v10/input.h"
#include "v10/light.h"
#include "v10/material.h"
#include "v10/model.h"
#include "v10/shader.h"
#include "v10/skybox.h"
#include "v10/texture.h"

#include <cglm/struct/affine-pre.h>
#include <cglm/struct/affine.h>
#include <cglm/struct/vec3.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#define MOVE_SPEED 10.0f
#define EVENT_COUNT 3

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
    inputSetEvents(events, EVENT_COUNT);
    inputInit(window);
#ifdef ENABLE_ERRORCHECKING
    errorInit();
#endif

    ShaderCache shaderCache = shaderCacheCreate();
    shaderUseCache(shaderCache);

    Camera *camera = cameraCreate(GLMS_VEC3_ZERO, GLMS_QUAT_IDENTITY);
    cameraSetProjectionMatrixPersp(camera, 60, 0.1f, 100.0f);
    cameraLookAt(camera, GLMS_VEC3_ZERO);

    DirLight *dirLights = malloc(DIRLIGHTS_MAX * sizeof(DirLight));
    memset(dirLights, 0, DIRLIGHTS_MAX * sizeof(DirLight));
    dirLights[0] = dirLightCreate(
        glms_vec3_normalize(glms_vec3_negate((vec3s){{-3, -4, 7}})),
        (vec3s){{0.2f, 0.2f, 0.2f}}, (vec3s){{0.694f, 0.688f, 0.578f}},
        (vec3s){{0.7f, 0.7f, 0.7f}});

    PointLight *pointLights = malloc(POINTLIGHTS_MAX * sizeof(PointLight));
    memset(pointLights, 0, POINTLIGHTS_MAX * sizeof(PointLight));
    pointLights[0] = pointLightCreate(
        (vec3s){{-2.2f, 1.2f, -0.6f}}, (vec3s){{0.2f, 0.2f, 0.2f}},
        (vec3s){{1.0f, 1.0f, 1.0f}}, 1.0f, 10.0f, 1.5f);

    SpotLight *spotLights = malloc(SPOTLIGHTS_MAX * sizeof(SpotLight));
    memset(spotLights, 0, SPOTLIGHTS_MAX * sizeof(SpotLight));
    spotLights[0] = spotLightCreate(
        (vec3s){{5.2f, -3.2f, -5.6f}},
        glms_vec3_normalize(glms_vec3_negate((vec3s){{-3, 2, 1}})),
        (vec3s){{1.0f, 0.2f, 1.0f}}, (vec3s){{1.0f, 1.0f, 1.0f}}, 5.0f, 15.0f,
        1.5f, 12.5f, 12.5f + 20.0f);

    LightScene *lightScene =
        lightSceneCreate(dirLights, pointLights, spotLights);

    int ENTITY_COUNT = 0;

    Shader *lightShader =
        shaderCreate("light_source.vert", "", "light_source.frag");
    Model *pointLightModel =
        modelLoad("light.glb", MODELOPTS_DONT_IMPORT_MATERIALS);
    pointLightModel->Materials[0] =
        materialCreate(lightShader, 1,
                       materialPropertyCreate("diffuse", MATTYPE_VEC3,
                                              (void *)&pointLights[0].Diffuse));
    pointLightModel->WorldFromModel =
        glms_translate(GLMS_MAT4_IDENTITY, pointLights[0].Position);
    ENTITY_COUNT++;

    Model *dirLightModel =
        modelLoad("arrow.glb", MODELOPTS_DONT_IMPORT_MATERIALS);
    dirLightModel->Materials[0] =
        materialCreate(lightShader, 1,
                       materialPropertyCreate("diffuse", MATTYPE_VEC3,
                                              (void *)&dirLights[0].Diffuse));
    vec3s startAxis = (vec3s){{1, 0, 0}};
    float angle = acos(glms_vec3_dot(startAxis, dirLights[0].Direction));
    vec3s axis = glms_vec3_cross(startAxis, dirLights[0].Direction);
    dirLightModel->WorldFromModel =
        glms_rotate(GLMS_MAT4_IDENTITY, angle, axis);
    ENTITY_COUNT++;

    Model *spotLightModel =
        modelLoad("flashlight.glb", MODELOPTS_DONT_IMPORT_MATERIALS);
    spotLightModel->Materials[0] =
        materialCreate(lightShader, 1,
                       materialPropertyCreate("diffuse", MATTYPE_VEC3,
                                              (void *)&spotLights[0].Diffuse));
    angle = acos(glms_vec3_dot(startAxis, spotLights[0].Direction));
    axis = glms_vec3_cross(startAxis, spotLights[0].Direction);
    spotLightModel->WorldFromModel =
        glms_rotate(glms_translate(GLMS_MAT4_IDENTITY, spotLights[0].Position),
                    angle, axis);
    ENTITY_COUNT++;

    Shader *homeShader =
        shaderCreate("light_affected.vert", "", "light_affected.frag");
    Model *homeModel = modelLoad("home.glb", 0);
    for (int i = 0; i < homeModel->MaterialCount; i++) {
        homeModel->Materials[i]->Shader = homeShader;
    }
    ENTITY_COUNT++;

    Shader *skinningShader =
        shaderCreate("skinning.vert", "", "light_affected.frag");
    Model *skinningModel = modelLoad("BrainStem.glb", 0);
    skinningModel->TexDepthShader =
        shaderCreate("depth_skinning_tex2d.vert", "", "depth_tex2d.frag");
    skinningModel->CubemapDepthShader =
        shaderCreate("depth_skinning_cubemap.vert", "depth_cubemap.geom",
                     "depth_cubemap.frag");
    for (int i = 0; i < skinningModel->MaterialCount; i++) {
        skinningModel->Materials[i]->Shader = skinningShader;
    }
    skinningModel->WorldFromModel = glms_translate(
        glms_scale(glms_rotate_x(glms_rotate_z(GLMS_MAT4_IDENTITY, glm_rad(90)),
                                 glm_rad(180)),
                   (vec3s){{1.5f, 1.5f, 1.5f}}),
        (vec3s){{-3.0f, 0.0f, -1.4f}});
    Armature *armature = armatureCreate(skinningModel);
    ENTITY_COUNT++;

    const int TexturedBoxCount = 7;
    Model **texturedBoxes = malloc(TexturedBoxCount * sizeof(Model *));
    Shader *texturedShader =
        shaderCreate("light_affected.vert", "", "light_affected.frag");
    for (int i = 0; i < TexturedBoxCount; i++) {
        texturedBoxes[i] = modelLoad("BoxTextured.glb", 0);
        for (int j = 0; j < texturedBoxes[i]->MaterialCount; j++) {
            texturedBoxes[i]->Materials[j]->Shader = texturedShader;
        }
        texturedBoxes[i]->WorldFromModel = glms_translate(
            glms_rotate_y(GLMS_MAT4_IDENTITY, glm_rad(23)),
            (vec3s){{-1.7f + (float)i / ((TexturedBoxCount - 1) * 2), -4.0f + i,
                     -4.4f}});
        ENTITY_COUNT++;
    }

    Entity **entities = malloc(ENTITY_COUNT * sizeof(Entity *));
    entities[0] = entityCreate(pointLightModel, NULL);
    entities[1] = entityCreate(dirLightModel, NULL);
    entities[2] = entityCreate(spotLightModel, NULL);
    entities[3] = entityCreate(homeModel, NULL);
    entities[4] = entityCreate(skinningModel, armature);
    for (int i = 0; i < TexturedBoxCount; i++) {
        entities[i + 5] = entityCreate(texturedBoxes[i], NULL);
    }

    glEnable(GL_CULL_FACE);

    Cubemap *cubemap = cubemapCreate("sky");
    skyboxInit();
    skyboxSet(cubemap, NULL);

    float lastTime = 0, currentTime = 0, deltaTime = 0;
    vec3s eulerAngles = GLMS_VEC3_ZERO;
    vec3s movementInput = GLMS_VEC3_ZERO;
    vec3s positionDelta = GLMS_VEC3_ZERO;

    InputEvent *movementEvent = inputGetEvent("movement");
    InputEvent *exitEvent = inputGetEvent("exit");
    InputEvent *sprintEvent = inputGetEvent("sprint");
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
        if (sprintEvent->State > 0)
            movementInput = glms_vec3_scale(movementInput, 2);
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
        dirLightModel->WorldFromModel =
            glms_rotate(GLMS_MAT4_IDENTITY, angle, axis);

        lastTime = currentTime;

#define fov(x) (1.0f / 112.0f) * (x - 800.0f) + 60.0f
        cameraSetProjectionMatrixPersp(camera, fov(WINDOW_WIDTH), 0.1f, 100.0f);

        lightSceneRenderShadowMaps(lightScene, entities, ENTITY_COUNT);
        for (int i = 0; i < shaderCache->size; i++) {
            glUseProgram(shaderCacheIndex(shaderCache, i)->ID);
            glActiveTexture(GL_TEXTURE10);
            glBindTexture(GL_TEXTURE_2D,
                          lightScene->DirLightShadowMaps[0]->Target);
            glActiveTexture(GL_TEXTURE9);
            glBindTexture(GL_TEXTURE_CUBE_MAP,
                          lightScene->PointLightShadowMaps[0]->Target);
            glActiveTexture(GL_TEXTURE8);
            glBindTexture(GL_TEXTURE_2D,
                          lightScene->SpotLightShadowMaps[0]->Target);
        }

        cameraPreRender(camera);
        lightScenePreRender(lightScene);

        for (int i = 0; i < ENTITY_COUNT && i < 3; i++) {
            entityRender(entities[i], ENTITYRENDER_NOAPPLYTRANSFORMS |
                                          ENTITYRENDER_WIREFRAME);
        }
        for (int i = 3; i < ENTITY_COUNT; i++) {
            entityRender(entities[i], ENTITYRENDER_NOAPPLYTRANSFORMS);
        }

        skyboxRender();

        windowDraw(window);
    }

    cubemapFreeCache();
    skyboxDestroy();

    for (int entityIdx = 0; entityIdx < ENTITY_COUNT; entityIdx++) {
        for (int i = 0; i < entities[entityIdx]->Model->MaterialCount; i++) {
            Material *material = entities[entityIdx]->Model->Materials[i];
            for (int j = 0; j < material->PropertyCount; j++) {
                MaterialProperty *property = material->Properties[j];
                if (strncmp(property->Name, "_material.", 9) == 0) {
                    materialPropertyDelete(property);
                }
            }
        }
    }
    for (int i = 0; i < skinningModel->MaterialCount; i++)
        materialFree(skinningModel->Materials[i]);
    for (int i = 0; i < homeModel->MaterialCount; i++)
        materialFree(homeModel->Materials[i]);

    materialPropertyFree(pointLightModel->Materials[0]->Properties[0]);
    materialFree(pointLightModel->Materials[0]);
    materialPropertyFree(dirLightModel->Materials[0]->Properties[0]);
    materialFree(dirLightModel->Materials[0]);
    materialPropertyFree(spotLightModel->Materials[0]->Properties[0]);
    materialFree(spotLightModel->Materials[0]);
    modelFree(skinningModel);
    modelFree(spotLightModel);
    modelFree(pointLightModel);
    modelFree(dirLightModel);
    modelFree(homeModel);
    for (int i = 0; i < TexturedBoxCount; i++) {
        for (int j = 0; i < texturedBoxes[i]->MaterialCount; i++)
            materialFree(texturedBoxes[i]->Materials[j]);
        modelFree(texturedBoxes[i]);
    }
    free(texturedBoxes);
    armatureFree(armature);

    for (int i = 0; i < ENTITY_COUNT; i++) {
        entityFree(entities[i]);
    }
    free(entities);

    free(spotLights);
    free(pointLights);
    free(dirLights);
    lightSceneFree(lightScene);

    textureFreeCache();
    shaderCacheFree(shaderCache);

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

    events[2] = (InputEvent){
        .Name = "sprint",
        .Type = INPUTEVENT_BUTTON,
        .KeyCount = 1,
    };
    events[2].Keys = malloc(1 * sizeof(struct InputKey));
    events[2].Keys[0] = (struct InputKey){
        .Value = GLFW_KEY_LEFT_SHIFT,
    };

    return events;
}
void freeInputEventArray(InputEvent *events) {
    for (int i = 0; i < EVENT_COUNT; i++) {
        free(events[i].Keys);
    }
    free(events);
}
