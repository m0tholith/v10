#include "window.h"

#include "camera.h"
#include "input.h"
#include "material.h"
#include "model.h"
#include "rendering.h"
#include "shader.h"
#include "texture.h"

#include <unistd.h>

#define MOVE_SPEED 10.0f

GLFWwindow *window;

mat4s ViewMatrix, ProjectionMatrix;
vec3s movementInput;
vec2s mousePosition;
vec2s mouseDelta;
vec2s mouseSensitivity;

int main(void) {
    window = windowCreate();
    inputInit(window);

    Camera camera =
        cameraCreate((vec3s){{0.0f, 1.0f, -1.0f}}, GLMS_QUAT_IDENTITY);
    cameraLookAt(&camera, GLMS_VEC3_ZERO);

    // shader init
    unsigned int shader = shaderCreate("shaders/vertex_shader.vert",
                                       "shaders/fragment_shader.frag");

    // material init
    vec3s *tintValue = malloc(sizeof(vec3s));
    *tintValue = (vec3s){{0.3f, 0.7f, 0.4f}};
    MaterialProperty *tint =
        materialPropertyCreate("tint", MATTYPE_VEC3, (void *)tintValue);

    Texture texture = textureCreate("textures/crate.jpg", TEXTURETYPE_RGB);
    MaterialProperty *textureProperty = materialPropertyCreate(
        "albedo", MATTYPE_TEXTURE2D, (void *)&texture.Id);

    Material *material = materialCreate(shader, 2);
    material->Properties[0] = textureProperty;
    material->Properties[1] = tint;
    materialApplyProperties(material);

    Model *model1 = modelLoad("models/nodes_test.glb");

    ProjectionMatrix = glms_perspective(
        glm_rad(60.0f), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f,
        100.0f);

    float lastTime = 0, currentTime = 0, deltaTime = 0;
    vec3s eulerAngles = GLMS_VEC3_ZERO;
    vec3s positionDelta = GLMS_VEC3_ZERO;
    while (!glfwWindowShouldClose(window)) {
        currentTime = glfwGetTime();
        deltaTime = currentTime - lastTime;

        inputMouseUpdate(window);
        eulerAngles = glms_vec3_add(
            eulerAngles,
            (vec3s){{-mouseDelta.y * mouseSensitivity.x * deltaTime,
                     -mouseDelta.x * mouseSensitivity.y * deltaTime, 0}});
        cameraSetEulerAngles(&camera, eulerAngles);

        positionDelta = (vec3s){{movementInput.x, 0, -movementInput.y}};
        positionDelta = glms_quat_rotatev(camera.Quaternion, positionDelta);
        positionDelta =
            glms_vec3_add(positionDelta, (vec3s){{0, -movementInput.z, 0}});
        camera.Position = glms_vec3_add(
            camera.Position,
            glms_vec3_scale(positionDelta, MOVE_SPEED * deltaTime));
        cameraCalculateViewMatrix(&camera);
        ViewMatrix = camera.ViewMatrix;

        lastTime = currentTime;

        // HINT: drawing goes here
        // glClearColor(0.12f, 0.12f, 0.18f, 1.0f);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        materialPreRender(material);
        modelRender(model1, shader);

        windowDraw(window);
    }

    modelDelete(model1);
    materialFree(material);
    free(tintValue);

    windowClose();
    return 0;
}
