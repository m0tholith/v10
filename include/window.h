#ifndef WINDOW_H
#define WINDOW_H

#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <stdio.h>

extern int WINDOW_WIDTH;
extern int WINDOW_HEIGHT;

GLFWwindow *windowCreate();
void windowSetSkybox(float r, float g, float b);
void windowDraw(GLFWwindow *window);
void windowClose();

#endif // !WINDOW_H
