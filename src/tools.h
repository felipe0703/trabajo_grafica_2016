#ifndef TOOLS_H
#define TOOLS_H

#include <GL/glew.h> // include GLEW and new version of GL on Windows
#include <GLFW/glfw3.h> // GLFW helper library


// funcion que carga una malla desde filename
bool load_mesh (const char* file_name, GLuint* vao, int* point_count);


void init();

void display();

void reshape(GLFWwindow *window, int width, int height);

void input_teclado(GLFWwindow *window, int key, int scancode, int action, int mods);

float dif_tiempo();

void create_shaders(GLuint shaders);
void control_teclado(GLFWwindow * window, GLuint shaders, int view_mat_location, double tiempo , 
						float cam_speed, float	cam_yaw_speed , float &cam_yaw , float cam_pos[3]);


#endif
