#include <Windows.h>

#include "glSetup.h"

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
#include <fstream>

void 	init();
void	animate();
void	quit();
void	render(GLFWwindow* window);
void	keyboard(GLFWwindow* window, int key, int code, int action, int mods);

// Camera configuration
vec3	eye(10, 10, 10);
vec3	center(0, 0, 0);
vec3	up(0, 1, 0);

// Light configuation
vec4	lightInitialP(1.5, 1.5, 0.0, 1);	// Initial light position

// Global coordinate frame
float	AXIS_LENGTH = 3;
float	AXIS_LINE_WIDTH = 2;

glm::vec3		p[18][36];
glm::mat4       TranslationMatrix = glm::translate(mat4(1.0), glm::vec3(1.0f, 1.0f, 0.0f));

// Colors
GLfloat	bgColor[4] = { 1, 1, 1, 1 };

// control variable

float	thetaLight[3];

bool	lightOn[3];		// Point = 0, distant = 1, spot = 2 lights
bool	attenuation = false;	// Attenuation for point light

bool	exponent = false;
float	exponentInitial = 0.0;			// [0, 128]
float	exponentValue = exponentInitial;
float	exponentNorm = exponentValue / 128.0;	// [0, 1]

bool	cutoff = false;
float	cutoffMax = 60;				// [0, 90] degree
float	cutoffInitial = 30.0;			// [0, cutoffMax] degree
float	cutoffValue = cutoffInitial;
float	cutoffNorm = cutoffValue / cutoffMax;	// [0, 1]

// Paly configuration
bool	pause = true;

float	timeStep = 1.0 / 120;	// 120fps. 60fps using vsync =1
float	period = 4.0;

int		selection = 0;
// Current frame
int 	frame = 0;

GLUquadricObj*	sphere = NULL;
GLUquadricObj*	cylinder = NULL;
GLUquadricObj*	cone = NULL;


void
reinitialize()
{
	frame = 0;

	lightOn[0] = true;	// Turn on only the point light
	lightOn[1] = false;
	lightOn[2] = false;

	for (int i = 0; i < 3; i++)
		thetaLight[i] = 0;

	exponentValue = exponentInitial;
	exponentNorm = exponentValue / 128.0;

	cutoffValue = cutoffInitial;
	cutoffNorm = cutoffValue / cutoffMax;
}

void
animate()
{
	frame += 1;

	// Rotation angle of the light

		for (int i = 0; i < 3; i++)
			if (lightOn[i]) thetaLight[i] += 4 / period;	//degree
	
													// periodically change the exponent and/or cutoff value of the spot light */
	if (lightOn[2] && exponent)
	{
		exponentNorm += radians(4.0 / period) / 3.14;
		exponentValue = 128.0 * (acos(cos(exponentNorm * 3.14)) / 3.14);
	}
	if (lightOn[2] && cutoff)
	{
		cutoffNorm += radians(4.0 / period) / 3.14;
		cutoffValue = cutoffMax * (acos(cos(cutoffNorm * 3.14)) / 3.14);
	}
}

void
init()
{
	// Animation system
	reinitialize();

	sphere = gluNewQuadric();
	gluQuadricDrawStyle(sphere, GLU_FILL);
	gluQuadricNormals(sphere, GLU_SMOOTH);
	gluQuadricOrientation(sphere, GLU_OUTSIDE);
	gluQuadricTexture(sphere, GL_FALSE);

	cylinder = gluNewQuadric();
	gluQuadricDrawStyle(cylinder, GLU_FILL);
	gluQuadricNormals(cylinder, GLU_SMOOTH);
	gluQuadricOrientation(cylinder, GLU_OUTSIDE);
	gluQuadricTexture(cylinder, GL_FALSE);

	cone = gluNewQuadric();
	gluQuadricDrawStyle(cone, GLU_FILL);
	gluQuadricNormals(cone, GLU_SMOOTH);
	gluQuadricOrientation(cone, GLU_OUTSIDE);
	gluQuadricTexture(cone, GL_FALSE);


	for (int i = 0; i < 18; i++)
	{
		for (int j = 0; j < 36; j++)
		{
			glm::vec4		s(0.3f, 0.3f, 0.0f, 1.0f);
			glm::mat4       RotationMatrix = glm::rotate(mat4(1.0), radians(20.0f*i), glm::vec3(0.0f, 0.0f, 1.0f));
			glm::mat4		RotationMatrix2 = glm::rotate(mat4(1.0), radians(10.0f*j), glm::vec3(0.0f, 1.0f, 0.0f));
			glm::vec4       points = RotationMatrix2 * TranslationMatrix * RotationMatrix * s;
			p[i][j] = vec3(points.x, points.y, points.z);
		}
	}


	// Keyboard
	cout << "Keyboard input: 1 for draw a torus model" << endl;
	cout << "Keyboard input: n for draw the vertex normal vectors" << endl;
	cout << "Keyboard input: p for Point light" << endl;
	cout << "Keyboard input: d for Directional light" << endl;
	cout << "Keyboard input: s for Spot light" << endl;
	cout << "Keyboard input: t for Time-varing shininess coefficient in specular reflection" << endl;
}


void
quit()
{

	gluDeleteQuadric(sphere);
	gluDeleteQuadric(cylinder);
	gluDeleteQuadric(cone);
}

// Material
void
setupMaterial()
{
	glColorMaterial(GL_FRONT, GL_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);

	GLfloat mat_ambient[4] = { 0.1, 0.1, 0.1, 1 };
	GLfloat mat_specular[4] = { 0.5, 0.5, 0.5, 1 };
	GLfloat mat_shininess = 100;

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
}



void
drawPoints()
{
	glBegin(GL_POINTS);
	glPointSize(10.0);
	glColor3f(0, 0, 0);
	glm::vec4       p(0.3f, 0.3f, 0.0f, 1.0f);
	glm::mat4       TranslationMatrix = glm::translate(mat4(1.0), glm::vec3(1.0f, 1.0f, 0.0f));
	for (int i = 0; i < 18; i++)
	{
			glm::mat4       RotationMatrix = glm::rotate(mat4(1.0), radians(20.0f*i), glm::vec3(0.0f, 0.0f, 1.0f));
			glm::vec4       points = TranslationMatrix * RotationMatrix * p;
			glVertex3f(points.x, points.y, points.z);

		for (int j = 0; j<36; j++)
		{
			glm::mat4	RotationMatrix2 = glm::rotate(mat4(1.0), radians(10.0f*j), glm::vec3(0.0f, 1.0f, 0.0f));
			glm::vec4	points2 = RotationMatrix2 * points;
			glVertex3f(points2.x, points2.y, points2.z);
		}
	}
	glEnd();
}



void
drawWireframe()
{
	glDisable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(0.0f, 0.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glColor3f(0, 0, 0);
	glBegin(GL_QUADS);
	for (int i = 0; i < 18; i++)
	{
		for (int j = 0; j < 36; j++)
		{
			int a = i, b = j;
			if ((i + 1) == 18 && (j + 1) == 36) {
				glNormal3f(p[a][b].x, p[a][b].y, p[a][b].z);
				glVertex3f(p[a][b].x, p[a][b].y, p[a][b].z);
				glNormal3f(p[0][b].x, p[0][b].y, p[0][b].z);
				glVertex3f(p[0][b].x, p[0][b].y, p[0][b].z);
				glNormal3f(p[0][0].x, p[0][0].y, p[0][0].z);
				glVertex3f(p[0][0].x, p[0][0].y, p[0][0].z);
				glNormal3f(p[a][0].x, p[a][0].y, p[a][0].z);
				glVertex3f(p[a][0].x, p[a][0].y, p[a][0].z);
			}
			else if ((i + 1) == 18 && (j + 1) != 36) {
				glNormal3f(p[a][b].x, p[a][b].y, p[a][b].z);
				glVertex3f(p[a][b].x, p[a][b].y, p[a][b].z);
				glNormal3f(p[0][b].x, p[0][b].y, p[0][b].z);
				glVertex3f(p[0][b].x, p[0][b].y, p[0][b].z);
				glNormal3f(p[0][b + 1].x, p[0][b + 1].y, p[0][b + 1].z);
				glVertex3f(p[0][b + 1].x, p[0][b + 1].y, p[0][b + 1].z);
				glNormal3f(p[a][b + 1].x, p[a][b + 1].y, p[a][b + 1].z);
				glVertex3f(p[a][b + 1].x, p[a][b + 1].y, p[a][b + 1].z);
			}
			else if ((i + 1) != 18 && (j + 1) == 36) {
				glNormal3f(p[a][b].x, p[a][b].y, p[a][b].z);
				glVertex3f(p[a][b].x, p[a][b].y, p[a][b].z);
				glNormal3f(p[a + 1][b].x, p[a + 1][b].y, p[a + 1][b].z);
				glVertex3f(p[a + 1][b].x, p[a + 1][b].y, p[a + 1][b].z);
				glNormal3f(p[a + 1][0].x, p[a + 1][0].y, p[a + 1][0].z);
				glVertex3f(p[a + 1][0].x, p[a + 1][0].y, p[a + 1][0].z);
				glNormal3f(p[a][0].x, p[a][0].y, p[a][0].z);
				glVertex3f(p[a][0].x, p[a][0].y, p[a][0].z);
			}
			else {
				glNormal3f(p[a][b].x, p[a][b].y, p[a][b].z);
				glVertex3f(p[a][b].x, p[a][b].y, p[a][b].z);
				glNormal3f(p[a + 1][b].x, p[a + 1][b].y, p[a + 1][b].z);
				glVertex3f(p[a + 1][b].x, p[a + 1][b].y, p[a + 1][b].z);
				glNormal3f(p[a + 1][b + 1].x, p[a + 1][b + 1].y, p[a + 1][b + 1].z);
				glVertex3f(p[a + 1][b + 1].x, p[a + 1][b + 1].y, p[a + 1][b + 1].z);
				glNormal3f(p[a][b + 1].x, p[a][b + 1].y, p[a][b + 1].z);
				glVertex3f(p[a][b + 1].x, p[a][b + 1].y, p[a][b + 1].z);
			}
		}
	}
	glEnd();
}

void
drawQuads()
{
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1.0f, 1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glColor3f(0, 1, 0);

	for (int i = 0; i < 18; i++)
	{
		for (int j = 0; j < 36; j++)
		{
			glBegin(GL_QUADS);
			int a = i, b = j;
			if ((i + 1) == 18 && (j + 1) == 36) {
				glNormal3f(p[a][b].x, p[a][b].y, p[a][b].z);
				glVertex3f(p[a][b].x, p[a][b].y, p[a][b].z);
				glNormal3f(p[0][b].x, p[0][b].y, p[0][b].z);
				glVertex3f(p[0][b].x, p[0][b].y, p[0][b].z);
				glNormal3f(p[0][0].x, p[0][0].y, p[0][0].z);
				glVertex3f(p[0][0].x, p[0][0].y, p[0][0].z);
				glNormal3f(p[a][0].x, p[a][0].y, p[a][0].z);
				glVertex3f(p[a][0].x, p[a][0].y, p[a][0].z);
			}
			else if ((i + 1) == 18 && (j + 1) != 36) {
				glNormal3f(p[a][b].x, p[a][b].y, p[a][b].z);
				glVertex3f(p[a][b].x, p[a][b].y, p[a][b].z);
				glNormal3f(p[0][b].x, p[0][b].y, p[0][b].z);
				glVertex3f(p[0][b].x, p[0][b].y, p[0][b].z);
				glNormal3f(p[0][b + 1].x, p[0][b + 1].y, p[0][b + 1].z);
				glVertex3f(p[0][b + 1].x, p[0][b + 1].y, p[0][b + 1].z);
				glNormal3f(p[a][b + 1].x, p[a][b + 1].y, p[a][b + 1].z);
				glVertex3f(p[a][b + 1].x, p[a][b + 1].y, p[a][b + 1].z);
			}
			else if ((i + 1) != 18 && (j + 1) == 36) {
				glNormal3f(p[a][b].x, p[a][b].y, p[a][b].z);
				glVertex3f(p[a][b].x, p[a][b].y, p[a][b].z);
				glNormal3f(p[a + 1][b].x, p[a + 1][b].y, p[a + 1][b].z);
				glVertex3f(p[a + 1][b].x, p[a + 1][b].y, p[a + 1][b].z);
				glNormal3f(p[a + 1][0].x, p[a + 1][0].y, p[a + 1][0].z);
				glVertex3f(p[a + 1][0].x, p[a + 1][0].y, p[a + 1][0].z);
				glNormal3f(p[a][0].x, p[a][0].y, p[a][0].z);
				glVertex3f(p[a][0].x, p[a][0].y, p[a][0].z);
			}
			else {
				glNormal3f(p[a][b].x, p[a][b].y, p[a][b].z);
				glVertex3f(p[a][b].x, p[a][b].y, p[a][b].z);
				glNormal3f(p[a + 1][b].x, p[a + 1][b].y, p[a + 1][b].z);
				glVertex3f(p[a + 1][b].x, p[a + 1][b].y, p[a + 1][b].z);
				glNormal3f(p[a + 1][b + 1].x, p[a + 1][b + 1].y, p[a + 1][b + 1].z);
				glVertex3f(p[a + 1][b + 1].x, p[a + 1][b + 1].y, p[a + 1][b + 1].z);
				glNormal3f(p[a][b + 1].x, p[a][b + 1].y, p[a][b + 1].z);
				glVertex3f(p[a][b + 1].x, p[a][b + 1].y, p[a][b + 1].z);
			}
			glEnd();

		}
	}glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}





// Compute the rotation axis and angle from a to b
//
// Axis is not normalized.
// theta is represented in degrees.
//
void
computeRotation(const vec3& a, const vec3& b, float& theta, vec3& axis)
{
	axis = cross(a, b);
	float	sinTheta = length(axis);
	float	cosTheta = dot(a, b);
	theta = atan2(sinTheta, cosTheta) * 180 / 3.14;
}



// Draw a torus after setting up its material


void
drawSphere(float radius, int slices, int stacks)
{
	gluSphere(sphere, radius, slices, stacks);
}

void
drawCylinder(float radius, float height, int slices, int stacks)
{
	gluCylinder(cylinder, radius, radius, height, slices, stacks);
}

void
drawCone(float radius, float height, int slices, int stacks)
{
	gluCylinder(cone, 0, radius, height, slices, stacks);
}


// Light
void
setupLight(const vec4& p, int i)
{
	GLfloat ambient[4] = { 0.1, 0.1, 0.1, 1 };
	GLfloat diffuse[4] = { 1.0, 1.0, 1.0, 1 };
	GLfloat specular[4] = { 1.0, 1.0, 1.0, 1 };

	glLightfv(GL_LIGHT0 + i, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0 + i, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0 + i, GL_SPECULAR, specular);
	glLightfv(GL_LIGHT0 + i, GL_POSITION, value_ptr(p));

	// Attenuation for the point light
	if (i == 0 && attenuation)
	{
		glLightf(GL_LIGHT0 + i, GL_CONSTANT_ATTENUATION, 1.0);
		glLightf(GL_LIGHT0 + i, GL_LINEAR_ATTENUATION, 0.1);
		glLightf(GL_LIGHT0 + i, GL_QUADRATIC_ATTENUATION, 0.05);
	}
	else {	// Default value
		glLightf(GL_LIGHT0 + i, GL_CONSTANT_ATTENUATION, 1.0);
		glLightf(GL_LIGHT0 + i, GL_LINEAR_ATTENUATION, 0.0);
		glLightf(GL_LIGHT0 + i, GL_QUADRATIC_ATTENUATION, 0.0);
	}

	if (i == 2)  // Spot light
	{
		vec3	spotDirection = -vec3(p);
		glLightfv(GL_LIGHT0 + i, GL_SPOT_DIRECTION, value_ptr(spotDirection));
		glLightf(GL_LIGHT0 + i, GL_SPOT_CUTOFF, cutoffValue);	// [0, 90]
		glLightf(GL_LIGHT0 + i, GL_SPOT_EXPONENT, exponentValue);	// [0, 90]
	}
	else {    // Point and distant light.
			  // 180 to turn off cutoff when it was used as a spot light.
		glLightf(GL_LIGHT0 + i, GL_SPOT_CUTOFF, 180);	// uniform light distribution
	}
}

void
drawArrow(const vec3& p, bool tailOnly)
{
	// Make it possible to change a subset of material parameters
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);

	// Common material
	GLfloat mat_specular[4] = { 1, 1, 1, 1 };
	GLfloat mat_shininess = 25;
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);

	// Transformation
	glPushMatrix();

	glTranslatef(p.x, p.y, p.z);

	if (!tailOnly)
	{
		float theta;
		vec3 axis;
		computeRotation(vec3(0, 0, 1), vec3(0, 0, 0) - vec3(p), theta, axis);
		glRotatef(theta, axis.x, axis.y, axis.z);
	}

	// Tail sphere
	float	arrowTailRadius = 0.05;
	glColor3f(1, 0, 0);	// ambient and diffuse
	drawSphere(arrowTailRadius, 16, 16);

	if (!tailOnly)
	{
		// Shaft cylinder
		float	arrowShaftRadius = 0.02;
		float	arrowShaftLength = 0.2;
		glColor3f(0, 1, 0);
		drawCylinder(arrowShaftRadius, arrowShaftLength, 16, 5);

		// Head cone
		float	arrowheadHeight = 0.09;
		float	arrowheadRadius = 0.06;
		glTranslatef(0, 0, arrowShaftLength + arrowheadHeight);
		glRotatef(180, 1, 0, 0);
		glColor3f(0, 0, 1);	// ambient and diffuse
		drawCone(arrowheadRadius, arrowheadHeight, 16, 5);
	}
	glPopMatrix();

	// For convential material setting
	glDisable(GL_COLOR_MATERIAL);
}

void
drawSpotLight(const vec3& p, float cutoff)
{
	glPushMatrix();

	glTranslatef(p.x, p.y, p.z);

	float	theta;
	vec3	axis;
	computeRotation(vec3(0, 0, 1), vec3(0, 0, 0) - vec3(p), theta, axis);
	glRotatef(theta, axis.x, axis.y, axis.z);

	// Color
	setupMaterial();

	// tan(cutoff) = r/h
	float	h = 0.15;
	float	r = h * tan(radians(cutoff));
	drawCone(r, h, 16, 5);

	// Color
	float	apexRadius = 0.06 * (0.5 + exponentValue / 128.0);
	drawSphere(apexRadius, 16, 16);

	glPopMatrix();
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
	drawAxes(AXIS_LENGTH, AXIS_LINE_WIDTH*dpiScaling);

	// Smooth shading
	glShadeModel(GL_SMOOTH);

	// Rotation of the light or 3x3 models
	vec3	axis(0, 1, 0);

	// Lighting
	//
	glEnable(GL_LIGHTING);

	// Set up the lights
	vec4	lightP[3];
	for (int i = 0; i < 3; i++)
	{
		// Just turn off the i-th light, if not lit
		if (!lightOn[i]) { glDisable(GL_LIGHT0 + i); continue; }

		// Turn on the i-th light
		glEnable(GL_LIGHT0 + i);

		// Dealing with the distant light
		lightP[i] = lightInitialP;

		if (i == 1) lightP[i].w = 0;

		// LIghts rotate around the center of the world coordinate system
		mat4	R = rotate(mat4(1.0), radians(thetaLight[i]), axis);
		lightP[i] = R * lightP[i];
		// Set up the i-th light
		setupLight(lightP[i], i);
	}

	// Draw the geometries of the lights
	for (int i = 0; i < 3; i++)
	{
		if (!lightOn[i])		continue;

		if (i == 2) drawSpotLight(lightP[i], cutoffValue);
		else	    drawArrow(lightP[i], i == 0);	// Tail only for a point light
	}
	switch (selection)
	{
	case 1: drawQuads();           break;
	}

}


void
keyboard(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS || action == GLFW_REPEAT)
	{
		switch (key)
		{

		// Initialization
		case GLFW_KEY_I: 	reinitialize();	break;

		// Play on/off
		case GLFW_KEY_SPACE:	pause = !pause; 		break;

		// Turn on/off the point, distant, spot light
		case GLFW_KEY_P: lightOn[0] = !lightOn[0];	break;
		case GLFW_KEY_D: lightOn[1] = !lightOn[1];	break;
		case GLFW_KEY_S: lightOn[2] = !lightOn[2];	break;

		// Time
		case GLFW_KEY_T:         break;


		case GLFW_KEY_1:        selection = 1;  break;

		}
	}
}


int
main(int argc, char* argv[])
{
	// Field of view of 85mm lens in degree
	fovy = 16.1;

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
	//
	init();

	// Main loop
	float	previous = glfwGetTime();
	float	elapsed = 0;
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();		// Events

								// Time passed during a single loop
		float	now = glfwGetTime();
		float	delta = now - previous;
		previous = now;

		// Time passed after the previous frame
		elapsed += delta;

		// Deal with the current frame
		if (elapsed > timeStep)
		{
			// Animate 1 frame
			if (!pause) animate();

			elapsed = 0;		// Reset the elapsed time
		}
		render(window);			// Draw one frame
		glfwSwapBuffers(window);	// Swap buffers
	}
	// Finalization
	quit();

	// Terminate the glfw system
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;

}



