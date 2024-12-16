#ifndef WINDOW_H
#define WINDOW_H

#include "v10/lib/glad/glad.h"
#include <GLFW/glfw3.h>

typedef struct {
    GLFWwindow *glWin;
    int Width;
    int Height;
} Window;

Window *windowCreate(int width, int height, char *title);
void windowSetSkybox(float r, float g, float b);
void windowPreDraw(Window *window);
void windowDraw(Window *window);
void windowClose(Window *window);

#endif // !WINDOW_H
