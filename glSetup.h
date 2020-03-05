#pragma once
#include <Windows.h>

#ifndef __GL_SETUP_H__
#define __GL_SETUP_H__

//#include <GL/glew.h> //OpenGL Extension Wrangler Library
#if defined(__APPLE__) && defined(__MACH__)
#include <OpenGL/glu.h>
#else
#endif

#include "include/GL/gl.h"
#include "include/GL/glu.h"
#include "include/GL/glut.h"

#include "include/GLFW/glfw3.h"
#include <stdbool.h> //for using type name 'bool'
#include <string.h>
#include <iostream>
#include <cstdlib> //for using EXIT_FAILURE

#pragma comment(lib, "lib/glfw3.lib")
#pragma comment(lib, "lib/glu32.lib")
#pragma comment(lib, "lib/glut32.lib")
#pragma comment(lib, "lib/opengl32.lib")

using namespace std;

float screenScale = 0.5; //Portion of the screen when not using full screen
int screenW = 0, screenH = 0; //screenScale portion of the screen
int windowW, windowH; //Framebuffer size
float aspect; //Aspect ratio = windowW/windowH
float dpiScaling = 0; //DPI scaling for HIDPI: windowW dpiScaling x screenW

int vsync = 1; //Vertical sync on/off

bool perspectiveView = true; //Perspective or orthographic viewing
float fovy = 46.4; //Field of view in the y direction
float nearDist = 1.0; //Near plane
float farDist = 20.0; //Far plane

void
errorCallback(int error, const char* description)
{
	cerr << "####" << description << endl;
}

void
setupProjectionMatrix()
{
	if (perspectiveView)
		gluPerspective(fovy, aspect, nearDist, farDist);
	else
		glOrtho(-1.0 * aspect, 1.0 * aspect, -1.0, 1.0, -nearDist, farDist);
}

void reshape(GLFWwindow* window, int w, int h)
{
	aspect = (float)w / h;

	//Set the viewport
	windowW = w;
	windowH = h;
	glViewport(0, 0, w, h);

	//Initialize the projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	//Set up a projection matrix
	setupProjectionMatrix();

	//The screen size is required for mouse interaction.
	glfwGetWindowSize(window, &screenW, &screenH);
	cerr << "reshape(" << w << ", " << h << ")";
	cerr << " with screen " << screenW << " x " << screenH << endl;
}

GLFWwindow* initializeOpenGL(int argc, char* argv[], GLfloat bgColor[4])
{
	glfwSetErrorCallback(errorCallback);

	//Init GLFW
	if (!glfwInit()) exit(EXIT_FAILURE);

	//Enable OpenGL 2.1 in OS X
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

	//Create the window
	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	int monitorW, monitorH;
	glfwGetMonitorPhysicalSize(monitor, &monitorW, &monitorH);
	cerr << "Status : Montior " << monitorW << "mm x " << monitorH << "mm" << endl;

	const GLFWvidmode* videoMode = glfwGetVideoMode(monitor);
	if (screenW == 0) screenW = videoMode->width * screenScale;
	if (screenH == 0) screenH = videoMode->height * screenScale;

	GLFWwindow * window = glfwCreateWindow(screenW, screenH, argv[0], NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		cerr << "Failed in glfwCreateWindow()" << endl;
		return NULL;
	}

	//Context
	glfwMakeContextCurrent(window);

	//Clear the background ASAP
	glClearColor(bgColor[0], bgColor[1], bgColor[2], bgColor[3]);
	glClear(GL_COLOR_BUFFER_BIT);
	glFlush();
	glfwSwapBuffers(window);

	//Check the size of the window
	glfwGetWindowSize(window, &screenW, &screenH);
	cerr << "Status : Screen " << screenW << " x " << screenH << endl;

	glfwGetFramebufferSize(window, &windowW, &windowH);
	cerr << "Status : Framebuffer " << windowW << " x " << windowH << endl;

	// DPI scaling
	if (dpiScaling == 0) dpiScaling = float(windowW) / screenW;

	//Callbacks
	glfwSetFramebufferSizeCallback(window, reshape);

	//Get the OpenGL version and renderer
	cout << "Status : Renderer " << glGetString(GL_RENDERER) << endl;
	cout << "Status : ventor " << glGetString(GL_VENDOR) << endl;
	cout << "Status : OpenGL " << glGetString(GL_VERSION) << endl;

	//Vertical sync...
	glfwSwapInterval(vsync); // 0 for immediate mode (Tearing possible)

	return window;
}

void drawAxes(float l, float w)
{
	glLineWidth(w);
	glBegin(GL_LINES);
	glColor3f(1, 0, 0); glVertex3f(0, 0, 0); glVertex3f(2, 0, 0); //x_axis
	glColor3f(0, 1, 0); glVertex3f(0, 0, 0); glVertex3f(0, 2, 0); //y_axis
	glColor3f(0, 0, 1); glVertex3f(0, 0, 0); glVertex3f(0, 0, 2); //z_axis
	glEnd();
}

#endif // __GL_SETUP_H__