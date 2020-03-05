#include <Windows.h>

#include "glSetup.h"
//#include "glSetup.cpp"

#include "include/GLFW/glfw3.h"
#include "include/GL/gl.h"
#include "include/GL/glut.h"

#include "include/glm/glm.hpp" //OpenGl Mathematics
#include "include/glm/gtc/type_ptr.hpp" //glm::value_ptr()
#include "include/glm/gtc/matrix_transform.hpp"

#pragma comment(lib, "lib/glfw3.lib")
#pragma comment(lib, "lib/opengl32.lib")
#pragma comment(lib, "lib/glut32.lib")
using namespace glm;

#include <iostream>
using namespace std;

#define	PI	3.1415

void 	init();
void	animate();
void	quit();
void	render(GLFWwindow* window);
void	keyboard(GLFWwindow* window, int key, int code, int action, int mods);

// Camera configuation
vec3	eye(3, 3, 3);
vec3	center(0, 0, 0);
vec3	up(0, 1, 0);

// Light configuation
vec4	light(5.0, 5.0, 5.0, 1);	// Light position

									// Global coordinate frame
float	AXIS_LENGTH = 3;
float	AXIS_LINE_WIDTH = 2;

// Colors
GLfloat	bgColor[4] = { 1, 1, 1, 1 };

// Paly configuration
bool	pause = false;

float	timeStep = 1.0 / 120;	// 120fps. 60fps using vsync =1
float	period = 4.0;

// Current frame
int 	frame = 0;


// Selected example
int selection = 0;


// Torus
GLUquadricObj* torus = NULL;

void
animate()
{
	frame += 1;
}

int
main(int argc, char* argv[])
{
	// Initialize the OpenGL system
	GLFWwindow* window = initializeOpenGL(argc, argv, bgColor);
	if (window == NULL) return -1;

	// Callbacks
	glfwSetKeyCallback(window, keyboard);

	// Depth test
	glEnable(GL_DEPTH_TEST);

	// Normal vectors are normalized after transformation.
	glEnable(GL_NORMALIZE);

	// Viewport and perspective setting
	reshape(window, windowW, windowH);

	// Initialization - Main loop - Finalization

	init();

	// Main loop
	float	previous = glfwGetTime();
	float	elapsed = 0;
	while (!glfwWindowShouldClose(window))
	{
		render(window);		// Draw one frame
		glfwSwapBuffers(window);	// Swap buffers
		glfwPollEvents();		// Events
	}

	// Finalization
	quit();

	// Terminate the glfw system
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}


void
init()
{
	// Prepare torus shape
	torus = gluNewQuadric();
	gluQuadricDrawStyle(torus, GLU_FILL);
	gluQuadricNormals(torus, GLU_SMOOTH);
	gluQuadricOrientation(torus, GLU_OUTSIDE);
	gluQuadricTexture(torus, GL_FALSE);

	// Keyboard
	cout << "Keyboard input: a for increase the sweep angle by 10 degree around the y-axis " << endl;
	cout << "Keyboard input: s for decrease the sweep angle by -10 degree around the y-axis " << endl;
	cout << "Keyboard input: j for increase the sweep angle by 20 degree around the z-axis " << endl;
	cout << "Keyboard input: k for increase the sweep angle by -20 degree around the z-axis " << endl;
	cout << "Keyboard input: 1 for draw 36x18 data points using 'GL_POINTS " << endl;
	cout << "Keyboard input: 2 for draw the wireframe only " << endl;
	cout << "Keyboard input: 3 for draw the quads only " << endl;
	cout << "Keyboard input: 4 for draw the quads and the wireframe " << endl;
	cout << "Keyboard input: 5 for draw the normal vectors of the polygons " << endl;
	cout << "Keyboard input: 6 for two-sided constant shading with the wireframe " << endl;

}

void
quit()
{
	// Delete torus
	gluDeleteQuadric(torus);
}



// Light
void
setupLight()
{
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	GLfloat ambient[4] = { 0.1, 0.1, 0.1, 1 };
	GLfloat diffuse[4] = { 1.0, 1.0, 1.0, 1 };
	GLfloat specular[4] = { 1.0, 1.0, 1.0, 1 };

	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
	glLightfv(GL_LIGHT0, GL_POSITION, value_ptr(light));

}

// Material
void
setupMaterial()
{
	// Material
	GLfloat mat_ambient[4] = { 0.1, 0.1, 0.1, 1 };
	GLfloat mat_specular[4] = { 0.5, 0.5, 0.5, 1 };
	GLfloat mat_shininess = 128;

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);

}

void
setDiffuseColor(const vec3& color)
{
	GLfloat mat_diffuse[4] = { color[0], color[1], color[2], 1 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
}


void
drawPoints()
{
	glPointSize(5.0);
	glColor3f(0, 0, 0);
	glBegin(GL_POINTS);
	glm::vec4       p(0.3f, 0.3f, 0.0f, 1.0f);
	glm::mat4       TranslationMatrix = glm::translate(mat4(1.0), glm::vec3(1.0f, 1.0f, 0.0f));
	for (int i = 0; i < 18; i++)
	{
		for (int j = 0; j < 36; j++)
		{
			glm::mat4       RotationMatrix = glm::rotate(mat4(1.0), radians(20.0f * i), glm::vec3(0.0f, 0.0f, 1.0f));
			glm::vec4       points = TranslationMatrix * RotationMatrix * p;
			glVertex3f(points.x, points.y, points.z);

			glm::mat4	RotationMatrix2 = glm::rotate(mat4(1.0), radians(10.0f * j), glm::vec3(0.0f, 1.0f, 0.0f));
			glm::vec4	points2 = RotationMatrix2 * points;
			glVertex3f(points2.x, points2.y, points2.z);
		}
	}
	glEnd();
}

void
drawWireframe()
{
	glBegin(GL_QUADS);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);





	glEnd();
}

void
drawQuads()
{
	glBegin(GL_QUADS);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void
drawNormalVectors()
{

}

void
shading()
{

}




// Draw a torus after setting up its material
void
drawTorus()
{
	glBegin(GL_QUADS);


}

void
render(GLFWwindow* window)
{
	// Background color
	glClearColor(bgColor[0], bgColor[1], bgColor[2], bgColor[3]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Modelview matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(eye[0], eye[1], eye[2], center[0], center[1], center[2], up[0], up[1], up[2]);

	// Axes
	glDisable(GL_LIGHTING);
	drawAxes(AXIS_LENGTH, AXIS_LINE_WIDTH * dpiScaling);

	//Lighting
	setupLight();

	// Material
	setupMaterial();

	// Rotation

	// Draw
	switch (selection)
	{
	case 1: drawPoints();		break;
	case 2: drawWireframe();	break;
	case 3: drawQuads();		break;
	case 4:	drawTorus();		break;
	case 5: drawNormalVectors();	break;
	case 6: shading();		break;
	}
}



void
keyboard(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS || action == GLFW_REPEAT)
	{
		switch (key)
		{

			// Quit
		case GLFW_KEY_Q:
		case GLFW_KEY_ESCAPE:	glfwSetWindowShouldClose(window, GL_TRUE);	break;

			// y-axis
		case GLFW_KEY_A:	break;
		case GLFW_KEY_S:	break;

			// z-axis
		case GLFW_KEY_J:	break;
		case GLFW_KEY_K:	break;


		case GLFW_KEY_1:	selection = 1;	break;
		case GLFW_KEY_2:	selection = 2;	break;
		case GLFW_KEY_3:	selection = 3;	break;
		case GLFW_KEY_4:	selection = 4;	break;
		case GLFW_KEY_5:	selection = 5;	break;
		case GLFW_KEY_6:	selection = 6;	break;



		}


	}

}















