#ifndef WINDOW_H
#define WINDOW_H

#include "v10/lib/glad/glad.h"
#include <GLFW/glfw3.h>
#include <stdio.h>

extern int WINDOW_WIDTH;
extern int WINDOW_HEIGHT;

GLFWwindow *windowCreate();
void windowSetSkybox(float r, float g, float b);
void windowDraw(GLFWwindow *window);
void windowClose(GLFWwindow *window);

#endif // !WINDOW_H
