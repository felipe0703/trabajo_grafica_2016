#include <GL/glew.h>
#define GLFW_DLL
#include <GLFW/glfw3.h> 
#include <stdio.h>


int main () {
//Abrir archivo para escribir el log de errores
FILE *log;
log = fopen("log.txt", "w+");

// start GL context and O/S window using the GLFW helper library
if (!glfwInit ()) {
fprintf (log, "ERROR: could not start GLFW3\n");
return 1;
} 
GLFWwindow* window = glfwCreateWindow (640, 480, "Hello Triangle", NULL, NULL);
if (!window) {
fprintf (log, "ERROR: could not open window with GLFW3\n");
glfwTerminate();
return 1;
}
glfwMakeContextCurrent (window);
 // start GLEW extension handler
glewExperimental = GL_TRUE;
glewInit ();
 // get version info
const GLubyte* renderer = glGetString (GL_RENDERER);
const GLubyte* version = glGetString (GL_VERSION);
printf ("Renderer: %s\n", renderer);
printf ("OpenGL version supported %s\n", version);
 // tell GL to only draw onto a pixel if the shape is closer to the viewer
glEnable (GL_DEPTH_TEST); // enable depth-testing
glDepthFunc (GL_LESS); // depth-testing interprets a smaller value as "closer"
 /* OTHER STUFF GOES HERE NEXT */
 // close GL context and any other GLFW resources
glfwTerminate();

int fclose(FILE *log);
return 0;
}
