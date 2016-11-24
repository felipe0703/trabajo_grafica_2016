/******************************************************************************\
| OpenGL 4 Example Code.                                                       |
| Accompanies written series "Anton's OpenGL 4 Tutorials"                      |
| Email: anton at antongerdelan dot net                                        |
| First version 27 Jan 2014                                                    |
| Copyright Dr Anton Gerdelan, Trinity College Dublin, Ireland.                |
| See individual libraries for separate legal notices                          |
|******************************************************************************|
| This demo uses the Assimp library to load a mesh from a file, and supports   |
| many formats. The library is VERY big and complex. It's much easier to write |
| a simple Wavefront .obj loader. I have code for this in other demos. However,|
| Assimp will load animated meshes, which will we need to use later, so this   |
| demo is a starting point before doing skinning animation                     |
\******************************************************************************/
#include <assimp/cimport.h> // C importer
#include <assimp/scene.h> // collects data
#include <assimp/postprocess.h> // various extra operations
#include <GL/glew.h> // include GLEW and new version of GL on Windows
#include <GLFW/glfw3.h> // GLFW helper library
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <string>
#include "maths_funcs.h"
#include "gl_utils.h"
#include "tools.h"
#include "enemigo.h"

#define GL_LOG_FILE "log/gl.log"
#define VERTEX_SHADER_FILE "shaders/test_vs.glsl"
#define FRAGMENT_SHADER_FILE "shaders/test_fs.glsl"
//#define MESH_FILE "monkey2.obj"
//#define MESH_FILE "mallas/cosa.obj"
#define MESH_FILE "mallas/suzanne.obj"

#include <iostream>
using namespace std;





// keep track of window size for things like the viewport and the mouse cursor
int g_gl_width = 1280;
int g_gl_height = 690;
GLFWwindow* g_window = NULL;

int main(){


	init();


	glEnable (GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc (GL_LESS); // depth-testing interprets a smaller value as "closer"
	glEnable (GL_CULL_FACE); // cull face
	glCullFace (GL_BACK); // cull back face
	glFrontFace (GL_CCW); // set counter-clock-wise vertex order to mean the front
	
    /* objeto enemigo */
    enemigo *e1 = new enemigo((char*)"mallas/suzanne.obj");
    //enemigo *e2 = new enemigo((char*)"mallas/cosa.obj")
/*-------------------------------CREATE SHADERS-------------------------------*/
	GLuint shader_programme = create_programme_from_files (
		VERTEX_SHADER_FILE, FRAGMENT_SHADER_FILE
	);	
	create_shaders(shader_programme);
 /*	
	int v = glGetUniformLocation(shader_programme, "variar");
	glUseProgram(shader_programme); 
	float h, theta=0.0;
*/
	///----------------colocando esto funciona el shader-------------------/////
	int view_mat_location = glGetUniformLocation (shader_programme, "view");
	glUseProgram (shader_programme);

	float cam_speed = 1.5f; // 1 unit per second
	float cam_yaw_speed = 10.0f; // 10 degrees per second
	float cam_pos[] = {0.0f, 0.0f, 5.0f}; // don't start at zero, or we will be too close
	float cam_yaw = 0.0f; // y-rotation in degrees

	while (!glfwWindowShouldClose (g_window)) {

		float tiempo = dif_tiempo();		
	
		_update_fps_counter (g_window);
		// wipe the drawing surface clear
		
		display();
		reshape(g_window,g_gl_width, g_gl_height);;

		glUseProgram (shader_programme);

		//glBindVertexArray (monkey_vao);
		//glDrawArrays (GL_TRIANGLES, 0, monkey_point_count);
        glBindVertexArray(e1->getvao());
        glDrawArrays(GL_TRIANGLES,0,e1->getnumvertices());
		// update other events like input handling 
		glfwPollEvents ();

		// varia la cuarta coordenada de cada punto en el vertex shader
/*		glUniform1f(v, h);
		/*h = fabs(sin(theta));
		theta += 0.01;
		h = 1;
*/
		// control keys
		control_teclado(g_window, shader_programme, view_mat_location, tiempo, cam_speed , cam_yaw_speed , cam_yaw , cam_pos);
	
		// put the stuff we've been drawing onto the display
		glfwSwapBuffers (g_window);
	}
	
	// close GL context and any other GLFW resources
	glfwTerminate();
	return 0;
}
