#include "tools.h"
#include "gl_utils.h"
#include <GL/glew.h> // include GLEW and new version of GL on Windows
#include <GLFW/glfw3.h> // GLFW helper library
//#define GL_LOG_FILE "gl.log"


#include <iostream>

using namespace std;


void init()
{
	restart_gl_log ();
/*------------------------------start GL context------------------------------*/
	start_gl ();

	glfwSetKeyCallback(g_window, input_teclado);

}

void display()
{
	// wipe the drawing surface clear
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.3f, 0.2f, 0.5f, 1.0);

}

void reshape(GLFWwindow *window, int width, int height)
{
	int gWidth = width;
	int gHeight = height;

	glViewport(0, 0, gWidth, gHeight);
}


void input_teclado(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS)
    {
    	switch(key)
    	{
    	case GLFW_KEY_A:
    		std::cout<<"A"<<endl;
    		break;
    	case GLFW_KEY_W:
    		std::cout<<"W"<<endl;
    		break;
    	case GLFW_KEY_S:
    		std::cout<<"S"<<endl;
    		break;
    	case GLFW_KEY_D:
    		std::cout<<"D"<<endl;
    		break;
    	case GLFW_KEY_ESCAPE:
    		glfwSetWindowShouldClose (window, 1);
    		break;



    	}

    }    	
}