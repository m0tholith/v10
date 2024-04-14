#include "window.h"

#include "camera.h"
#include "input.h"
#include "renderer.h"

#include <stdlib.h>

GLFWwindow *window;

vec3s wsadqe;

int main(void) {
    window = windowCreate();

    glfwSetKeyCallback(window, inputKeyCallback);

    Camera camera = cameraCreate((vec3s){{0.0f, 0.0f, 0.0f}},
                                 (versors){{0.0f, 0.0f, 0.0f, 0.0f}});
    // cameraLookAt(&camera, (vec3s){{-4.0f, -2.0f, 0.0f}});
    // cameraCalculateViewMatrix(&camera);

    // rectangle
    float vertices[] = {
        //    positions   //      colors     //tex coords//
        -1.0f, -1.0f, 1.0f,  1.0f, 1.0f, 1.0f, 0.0f, 0.0f, // 0, 17, 21
        -1.0f, 1.0f,  1.0f,  1.0f, 1.0f, 1.0f, 0.0f, 1.0f, // 1, 5, 22
        1.0f,  1.0f,  1.0f,  1.0f, 1.0f, 1.0f, 1.0f, 1.0f, // 2, 6, 10
        1.0f,  -1.0f, 1.0f,  1.0f, 1.0f, 1.0f, 1.0f, 0.0f, // 3, 9, 18

        -1.0f, 1.0f,  -1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, // 4, 13, 23
        -1.0f, 1.0f,  1.0f,  1.0f, 1.0f, 1.0f, 0.0f, 1.0f, // 1, 5, 22
        1.0f,  1.0f,  1.0f,  1.0f, 1.0f, 1.0f, 1.0f, 1.0f, // 2, 6, 10
        1.0f,  1.0f,  -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, // 7, 11, 14

        1.0f,  -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, // 8, 15, 19
        1.0f,  -1.0f, 1.0f,  1.0f, 1.0f, 1.0f, 0.0f, 1.0f, // 3, 9, 18
        1.0f,  1.0f,  1.0f,  1.0f, 1.0f, 1.0f, 1.0f, 1.0f, // 2, 6, 10
        1.0f,  1.0f,  -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, // 7, 11, 14

        -1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, // 12, 16, 20
        -1.0f, 1.0f,  -1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, // 4, 13, 23
        1.0f,  1.0f,  -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, // 7, 11, 14
        1.0f,  -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, // 8, 15, 19

        -1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, // 12, 16, 20
        -1.0f, -1.0f, 1.0f,  1.0f, 1.0f, 1.0f, 0.0f, 1.0f, // 0, 17, 21
        1.0f,  -1.0f, 1.0f,  1.0f, 1.0f, 1.0f, 1.0f, 1.0f, // 3, 9, 18
        1.0f,  -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, // 8, 15, 19

        -1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, // 12, 16, 20
        -1.0f, -1.0f, 1.0f,  1.0f, 1.0f, 1.0f, 0.0f, 1.0f, // 0, 17, 21
        -1.0f, 1.0f,  1.0f,  1.0f, 1.0f, 1.0f, 1.0f, 1.0f, // 1, 5, 22
        -1.0f, 1.0f,  -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, // 4, 13, 23
    };
    int indices[] = {
        0,  1,  2, // triangle 1
        0,  2,  3, // triangle 2

        4,  5,  6, // triangle 3
        4,  6,  7, // triangle 4

        8,  9,  10, // triangle 5
        8,  10, 11, // triangle 6

        12, 13, 14, // triangle 7
        12, 14, 15, // triangle 8

        16, 17, 18, // triangle 9
        16, 18, 19, // triangle 10

        20, 21, 22, // triangle 11
        20, 22, 23, // triangle 12
    };
    BasicObject *object = basicObjectInit(
        vertices, indices, sizeof(vertices) / sizeof(vertices[0]),
        sizeof(indices) / sizeof(indices[0]), "textures/crate.jpg",
        "shaders/vertex_shader.vert", "shaders/fragment_shader.frag");
    object->Transform =
        glms_translate(object->Transform, (vec3s){{-4.0f, -2.0f, 0.0f}});
    BasicObject *object2 = basicObjectInit(
        vertices, indices, sizeof(vertices) / sizeof(vertices[0]),
        sizeof(indices) / sizeof(indices[0]), "textures/crate.jpg",
        "shaders/vertex_shader.vert", "shaders/fragment_shader.frag");
    object2->Transform =
        glms_translate(object2->Transform, (vec3s){{4.0f, 2.0f, 0.0f}});
    BasicObject *object3 = basicObjectInit(
        vertices, indices, sizeof(vertices) / sizeof(vertices[0]),
        sizeof(indices) / sizeof(indices[0]), "textures/crate.jpg",
        "shaders/vertex_shader.vert", "shaders/fragment_shader.frag");
    object3->Transform =
        glms_translate(object3->Transform, (vec3s){{0.0f, 0.0f, -4.0f}});
    BasicObject *object4 = basicObjectInit(
        vertices, indices, sizeof(vertices) / sizeof(vertices[0]),
        sizeof(indices) / sizeof(indices[0]), "textures/crate.jpg",
        "shaders/vertex_shader.vert", "shaders/fragment_shader.frag");
    object4->Transform =
        glms_translate(object4->Transform, (vec3s){{0.0f, 0.0f, 4.0f}});

    ProjectionMatrix = glms_perspective(
        glm_rad(60.0f), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f,
        100.0f);

    float lastTime = 0, currentTime = 0, deltaTime = 0;
    vec3s eulerAngles = GLMS_VEC3_ZERO;
    while (!glfwWindowShouldClose(window)) {
        currentTime = glfwGetTime();
        deltaTime = currentTime - lastTime;

        vec3s deltaEulerAngles = glms_vec3_scale(wsadqe, deltaTime * 3);
        deltaEulerAngles = (vec3s){{
            deltaEulerAngles.y,
            -deltaEulerAngles.x,
            deltaEulerAngles.z,
        }};
        eulerAngles =
            glms_vec3_add(eulerAngles, deltaEulerAngles);
        cameraSetEulerAngles(&camera, eulerAngles);

        cameraCalculateViewMatrix(&camera);
        ViewMatrix = camera.ViewMatrix;

        lastTime = currentTime;

        // HINT: drawing goes here
        // glClearColor(0.12f, 0.12f, 0.18f, 1.0f);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        basicObjectDraw(object);
        basicObjectDraw(object2);
        basicObjectDraw(object3);
        basicObjectDraw(object4);

        windowDraw(window);
    }

    free(object);
    free(object2);
    free(object3);
    free(object4);

    windowClose();
    return 0;
}
