#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string>
#include <GL/glew.h> // include GLEW and new version of GL on Windows
#include <assimp/cimport.h> // C importer
#include <assimp/scene.h> // collects data
#include <assimp/postprocess.h> // various extra operations
#include "maths_funcs.h"
#include "gl_utils.h"
#include "tools.h"

#define _USE_MATH_DEFINES

#define VERTEX_SHADER_FILE "shaders/test_vs.glsl"
#define FRAGMENT_SHADER_FILE "shaders/test_fs.glsl"
#define GL_LOG_FILE "log/gl.log"
#include <iostream>

using namespace std;



bool load_mesh (const char* file_name, GLuint* vao, int* point_count) {
	const aiScene* scene = aiImportFile (file_name, aiProcess_Triangulate);
	if (!scene) {
		fprintf (stderr, "ERROR: reading mesh %s\n", file_name);
		return false;
	}
	printf ("  %i animations\n", scene->mNumAnimations);
	printf ("  %i cameras\n", scene->mNumCameras);
	printf ("  %i lights\n", scene->mNumLights);
	printf ("  %i materials\n", scene->mNumMaterials);
	printf ("  %i meshes\n", scene->mNumMeshes);
	printf ("  %i textures\n", scene->mNumTextures);
	
	/* get first mesh in file only */
	const aiMesh* mesh = scene->mMeshes[0];
	printf ("    %i vertices in mesh[0]\n", mesh->mNumVertices);
	
	/* pass back number of vertex points in mesh */
	*point_count = mesh->mNumVertices;
	
	/* generate a VAO, using the pass-by-reference parameter that we give to the
	function */
	glGenVertexArrays (1, vao);
	glBindVertexArray (*vao);
	
	/* we really need to copy out all the data from AssImp's funny little data
	structures into pure contiguous arrays before we copy it into data buffers
	because assimp's texture coordinates are not really contiguous in memory.
	i allocate some dynamic memory to do this. */
	GLfloat* points = NULL; // array of vertex points
	GLfloat* normals = NULL; // array of vertex normals
	GLfloat* texcoords = NULL; // array of texture coordinates
	if (mesh->HasPositions ()) {
		points = (GLfloat*)malloc (*point_count * 3 * sizeof (GLfloat));
		for (int i = 0; i < *point_count; i++) {
			const aiVector3D* vp = &(mesh->mVertices[i]);
			points[i * 3] = (GLfloat)vp->x;
			points[i * 3 + 1] = (GLfloat)vp->y;
			points[i * 3 + 2] = (GLfloat)vp->z;
		}
	}
	if (mesh->HasNormals ()) {
		normals = (GLfloat*)malloc (*point_count * 3 * sizeof (GLfloat));
		for (int i = 0; i < *point_count; i++) {
			const aiVector3D* vn = &(mesh->mNormals[i]);
			normals[i * 3] = (GLfloat)vn->x;
			normals[i * 3 + 1] = (GLfloat)vn->y;
			normals[i * 3 + 2] = (GLfloat)vn->z;
		}
	}
	if (mesh->HasTextureCoords (0)) {
		texcoords = (GLfloat*)malloc (*point_count * 2 * sizeof (GLfloat));
		for (int i = 0; i < *point_count; i++) {
			const aiVector3D* vt = &(mesh->mTextureCoords[0][i]);
			texcoords[i * 2] = (GLfloat)vt->x;
			texcoords[i * 2 + 1] = (GLfloat)vt->y;
		}
	}
	
	/* copy mesh data into VBOs */
	if (mesh->HasPositions ()) {
		GLuint vbo;
		glGenBuffers (1, &vbo);
		glBindBuffer (GL_ARRAY_BUFFER, vbo);
		glBufferData (
			GL_ARRAY_BUFFER,
			3 * *point_count * sizeof (GLfloat),
			points,
			GL_STATIC_DRAW
		);
		glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray (0);
		free (points);
	}
	if (mesh->HasNormals ()) {
		GLuint vbo;
		glGenBuffers (1, &vbo);
		glBindBuffer (GL_ARRAY_BUFFER, vbo);
		glBufferData (
			GL_ARRAY_BUFFER,
			3 * *point_count * sizeof (GLfloat),
			normals,
			GL_STATIC_DRAW
		);
		glVertexAttribPointer (1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray (1);
		free (normals);
	}
	if (mesh->HasTextureCoords (0)) {
		GLuint vbo;
		glGenBuffers (1, &vbo);
		glBindBuffer (GL_ARRAY_BUFFER, vbo);
		glBufferData (
			GL_ARRAY_BUFFER,
			2 * *point_count * sizeof (GLfloat),
			texcoords,
			GL_STATIC_DRAW
		);
		glVertexAttribPointer (2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray (2);
		free (texcoords);
	}
	if (mesh->HasTangentsAndBitangents ()) {
		// NB: could store/print tangents here
	}
	
	aiReleaseImport (scene);
	printf ("mesh loaded\n");
	
	return true;
}



void init()
{
	restart_gl_log ();
/*------------------------------start GL context------------------------------*/
	start_gl ();

	//glfwSetKeyCallback(g_window, input_teclado);

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

	float tiempo = dif_tiempo();
	GLuint shader;
	int view_mat_location = glGetUniformLocation (shader, "view");

	bool cam_moved = false;

	float cam_speed = 1.5f; // 1 unit per second
	float cam_pos[] = {0.0f, 0.0f, 5.0f}; // don't start at zero, or we will be too close
	float cam_yaw = 0.0f; // y-rotation in degrees

	mat4 T = translate (identity_mat4 (), vec3 (-cam_pos[0], -cam_pos[1], -cam_pos[2]));
	mat4 R = rotate_y_deg (identity_mat4 (), -cam_yaw);
	mat4 view_mat = R * T;

    if (action == GLFW_REPEAT || action == GLFW_PRESS)
    {
    	switch(key)
    	{
    	case GLFW_KEY_A:
    		cam_pos[0] -= cam_speed * tiempo;

			std::cout << cam_pos[0] << endl;
			cam_moved = true;
    		std::cout<<"A"<<endl;
    		break;
    	case GLFW_KEY_W:
			cam_pos[2] -= cam_speed * tiempo;
			cam_moved = true;

    		std::cout<<"W"<<endl;
    		break;
    	case GLFW_KEY_S:
    		cam_pos[2] += cam_speed *tiempo;
			cam_moved = true;

    		std::cout<<"S"<<endl;
    		break;
    	case GLFW_KEY_D:
    		cam_pos[0] += cam_speed * tiempo;
			cam_moved = true;
    		std::cout<<"D"<<endl;
    		break;
    	case GLFW_KEY_ESCAPE:
    		glfwSetWindowShouldClose (window, 1);
    		break;
    	}
    } 
    // update view matrix
		if (cam_moved) {
			mat4 T = translate (identity_mat4 (), vec3 (-cam_pos[0], -cam_pos[1], -cam_pos[2])); // cam translation
			mat4 R = rotate_y_deg (identity_mat4 (), -cam_yaw); // 
			mat4 view_mat = R * T ;
			glUniformMatrix4fv (view_mat_location, 1, GL_FALSE, view_mat.m);
		}
}

float dif_tiempo()

{
	static double previous_seconds = glfwGetTime ();
	double current_seconds = glfwGetTime ();
	double elapsed_seconds = current_seconds - previous_seconds;
	previous_seconds = current_seconds;

	return elapsed_seconds;
}


void create_shaders(GLuint shader)
{
	#define ONE_DEG_IN_RAD (2.0 * M_PI) / 360.0 // 0.017444444
	// input variables
	float near = 0.1f; // clipping plane
	float far = 100.0f; // clipping plane
	float fov = 67.0f * ONE_DEG_IN_RAD; // convert 67 degrees to radians
	float aspect = (float)g_gl_width / (float)g_gl_height; // aspect ratio
	// matrix components
	float range = tan (fov * 0.5f) * near;
	float Sx = (2.0f * near) / (range * aspect + range * aspect);
	float Sy = near / range;
	float Sz = -(far + near) / (far - near);
	float Pz = -(2.0f * far * near) / (far - near);
	
	GLfloat proj_mat[] = {
		Sx, 0.0f, 0.0f, 0.0f,
		0.0f, Sy, 0.0f, 0.0f,
		0.0f, 0.0f, Sz, -1.0f,
		0.0f, 0.0f, Pz, 0.0f
	};	

	float cam_speed = 1.5f; // 1 unit per second
	float cam_yaw_speed = 10.0f; // 10 degrees per second
	float cam_pos[] = {0.0f, 0.0f, 5.0f}; // don't start at zero, or we will be too close
	float cam_yaw = 0.0f; // y-rotation in degrees
	mat4 T = translate (identity_mat4 (), vec3 (-cam_pos[0], -cam_pos[1], -cam_pos[2]));
	mat4 R = rotate_y_deg (identity_mat4 (), -cam_yaw);
	mat4 view_mat = R * T;
	
	int view_mat_location = glGetUniformLocation (shader, "view");
	glUseProgram (shader);
	glUniformMatrix4fv (view_mat_location, 1, GL_FALSE, view_mat.m);
	int proj_mat_location = glGetUniformLocation (shader, "proj");
	glUseProgram (shader);
	glUniformMatrix4fv (proj_mat_location, 1, GL_FALSE, proj_mat);
}

void control_teclado(GLFWwindow *window ,  GLuint shaders, int view_mat_location, double tiempo ,float cam_speed, float	cam_yaw_speed , float &cam_yaw , float cam_pos[3])
{
	bool cam_moved = false;
	float cam_yaw_rads;

	if (glfwGetKey (window, GLFW_KEY_A)) {
		cam_pos[0] -= cam_speed * tiempo;
		//std::cout << cam_pos[0] << endl;
		cam_yaw_rads = ONE_DEG_IN_RAD * cam_yaw;
        cam_pos[0] -= cos(cam_yaw_rads) * cam_speed * tiempo;
        cam_pos[2] += sin(cam_yaw_rads) * cam_speed * tiempo;
        
		cam_moved = true;
	}
	if (glfwGetKey (window, GLFW_KEY_D)) {
		//cam_pos[0] += cam_speed * tiempo;
		cam_yaw_rads = ONE_DEG_IN_RAD * cam_yaw;
        cam_pos[0] += cos(cam_yaw_rads) * cam_speed * tiempo;
        cam_pos[2] -= sin(cam_yaw_rads) * cam_speed * tiempo;
		//std::cout << cam_pos[0] << endl;

		cam_moved = true;
	}	
	if (glfwGetKey (window, GLFW_KEY_W)) {;
		//cam_pos[2] -= cam_speed * tiempo;
		cam_yaw_rads = ONE_DEG_IN_RAD * cam_yaw;
        cam_pos[0] -= sin(cam_yaw_rads) * cam_speed * tiempo;
        cam_pos[2] -= cos(cam_yaw_rads) * cam_speed * tiempo;
		cam_moved = true;
	}
	if (glfwGetKey (window, GLFW_KEY_S)) {
		//cam_pos[2] += cam_speed * tiempo;
		cam_yaw_rads = ONE_DEG_IN_RAD * cam_yaw;
        cam_pos[0] += sin(cam_yaw_rads) * cam_speed * tiempo;
        cam_pos[2] += cos(cam_yaw_rads) * cam_speed * tiempo;
		cam_moved = true;
	}
	if (glfwGetKey (window, GLFW_KEY_DOWN)) {
		cam_pos[1] -= cam_speed * tiempo;
		cam_moved = true;
	}
	if (glfwGetKey (window, GLFW_KEY_UP)) {
		cam_pos[1] += cam_speed * tiempo;
		cam_moved = true;
	}
	if (glfwGetKey (window, GLFW_KEY_LEFT)) {
		cam_yaw += cam_yaw_speed * tiempo;

		std::cout << cam_yaw << endl;
		cam_moved = true;
	}
	if (glfwGetKey (window, GLFW_KEY_RIGHT)) {
		cam_yaw -= cam_yaw_speed * tiempo;
		std::cout << cam_yaw << endl;
		cam_moved = true;
	}
	if (glfwGetKey (window, GLFW_KEY_ESCAPE)) {
		glfwSetWindowShouldClose (window, 1);
	}
	// update view matrix
	if (cam_moved) {
		mat4 T = translate (identity_mat4 (), vec3 (-cam_pos[0], -cam_pos[1], -cam_pos[2])); // cam translation
		mat4 R = rotate_y_deg (identity_mat4 (), -cam_yaw); // 
		mat4 view_mat = R * T ;
		glUniformMatrix4fv (view_mat_location, 1, GL_FALSE, view_mat.m);
	}
}
