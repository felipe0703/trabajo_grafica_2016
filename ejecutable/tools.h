#ifndef _TOOLS_H_
#define _TOOLS_H_

#include <GL/glew.h> // include GLEW and new version of GL on Windows
#include <GLFW/glfw3.h> // GLFW helper library
//#define GL_LOG_FILE "gl.log"


void init();

void display();

void reshape(GLFWwindow *window, int width, int height);

void input_teclado(GLFWwindow *window, int key, int scancode, int action, int mods);





#endif