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
void	render(GLFWwindow* window);
void	keyboard(GLFWwindow* window, int key, int code, int action, int mods);

// Camera configuration
vec3	eye(10, 10, 10);
vec3	center(0, 0, 0);
vec3	up(0, 1, 0);

// Light configuation
vec4	light(1.0, 1.5, 0.5, 0);	// Initial light position

									// Global coordinate frame
float	AXIS_LENGTH = 3;
float	AXIS_LINE_WIDTH = 2;

glm::vec3		p[18][36];
glm::mat4       TranslationMatrix = glm::translate(mat4(1.0), glm::vec3(1.0f, 1.0f, 0.0f));

// Colors
GLfloat	bgColor[4] = { 1, 1, 1, 1 };

// Paly configuration
bool	pause = true;

float	timeStep = 1.0 / 120;	// 120fps. 60fps using vsync =1
float	period = 4.0;

int		selection = 0;
// Current frame
int 	frame = 0;

void
animate()
{
	frame += 1;
}


void readRAWImage(char* filename, GLbyte data[512][512][3]) {
	FILE * file;

	file = fopen(filename, "r");
	if (file != NULL) {
		fread(data, 512 * 512 * 3, 1, file);
		fclose(file);

	}
}

void
loadMarbleTexture()
{
	GLbyte marble[512][512][3];
	readRAWImage("Models\\marble.raw", marble);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, 512, 512, 0, GL_RGB, GL_UNSIGNED_BYTE, marble);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
}
void
loadWoodTexture()
{
	GLbyte wood[512][512][3];
	readRAWImage("Models\\wood.raw", wood);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, 512, 512, 0, GL_RGB, GL_UNSIGNED_BYTE, wood);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
}


// Texture
void
loadCheckerboardTexture()
{
	GLbyte checkerboard[512][512][3];
	readRAWImage("Models\\checker.raw", checkerboard);
	for (int i = 0; i < 512; i++) {
		for (int j = 0; j < 512; j++)
		{
			int c = (((i & 0x8) == 0) ^ ((j & 0x8) == 0)) * 255;

			checkerboard[i][j][0] = (GLbyte)c;
			checkerboard[i][j][1] = (GLbyte)c;
			checkerboard[i][j][2] = (GLbyte)c;
		}
	}

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, 512, 512, 0, GL_RGB, GL_UNSIGNED_BYTE, checkerboard);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
}

GLuint	texID[3];

void
init()
{
	//Texture
	glEnable(GL_TEXTURE_2D);
	glGenTextures(3, texID);

	glBindTexture(GL_TEXTURE_2D, texID[0]);
	loadMarbleTexture();

	glBindTexture(GL_TEXTURE_2D, texID[1]);
	loadWoodTexture();

	glBindTexture(GL_TEXTURE_2D, texID[2]);
	loadCheckerboardTexture();


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
	cout << "Keyboard input: 1 for draw a marble textured torus" << endl;
	cout << "Keyboard input: 2 for draw a wood textured torus " << endl;
	cout << "Keyboard input: 3 for draw a checker textured torus" << endl;
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
drawMarbleQuads()
{
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1.0f, 1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glColor3f(1, 1, 1);
	for (int i = 0; i < 18; i++)
	{

		for (int j = 0; j < 36; j++)
		{
			glBindTexture(GL_TEXTURE_2D, texID[0]);
			glBegin(GL_QUADS);
			int a = i, b = j;
			if ((i + 1) == 18 && (j + 1) == 36) {
				glNormal3f(p[a][b].x, p[a][b].y, p[a][b].z);
				glTexCoord2f(i / 18.0, j / 36.0);
				glVertex3f(p[a][b].x, p[a][b].y, p[a][b].z);

				glNormal3f(p[0][b].x, p[0][b].y, p[0][b].z);
				glTexCoord2f(1.0, j / 36.0);
				glVertex3f(p[0][b].x, p[0][b].y, p[0][b].z);

				glNormal3f(p[0][0].x, p[0][0].y, p[0][0].z);
				glTexCoord2f(1.0, 1.0);
				glVertex3f(p[0][0].x, p[0][0].y, p[0][0].z);

				glNormal3f(p[a][0].x, p[a][0].y, p[a][0].z);
				glTexCoord2f(i / 18.0, 1.0);
				glVertex3f(p[a][0].x, p[a][0].y, p[a][0].z);
			}
			else if ((i + 1) == 18 && (j + 1) != 36) {
				glNormal3f(p[a][b].x, p[a][b].y, p[a][b].z);
				glTexCoord2f(i / 18.0, j / 36.0);
				glVertex3f(p[a][b].x, p[a][b].y, p[a][b].z);

				glNormal3f(p[0][b].x, p[0][b].y, p[0][b].z);
				glTexCoord2f(1.0, j / 36.0);
				glVertex3f(p[0][b].x, p[0][b].y, p[0][b].z);

				glNormal3f(p[0][b + 1].x, p[0][b + 1].y, p[0][b + 1].z);
				glTexCoord2f(1.0, (j + 1) / 36.0);
				glVertex3f(p[0][b + 1].x, p[0][b + 1].y, p[0][b + 1].z);

				glNormal3f(p[a][b + 1].x, p[a][b + 1].y, p[a][b + 1].z);
				glTexCoord2f(i / 18.0, (j + 1) / 36.0);
				glVertex3f(p[a][b + 1].x, p[a][b + 1].y, p[a][b + 1].z);
			}
			else if ((i + 1) != 18 && (j + 1) == 36) {
				glNormal3f(p[a][b].x, p[a][b].y, p[a][b].z);
				glTexCoord2f(i / 18.0, j / 36.0);
				glVertex3f(p[a][b].x, p[a][b].y, p[a][b].z);

				glNormal3f(p[a + 1][b].x, p[a + 1][b].y, p[a + 1][b].z);
				glTexCoord2f((i + 1) / 18.0, j / 36.0);
				glVertex3f(p[a + 1][b].x, p[a + 1][b].y, p[a + 1][b].z);

				glNormal3f(p[a + 1][0].x, p[a + 1][0].y, p[a + 1][0].z);
				glTexCoord2f((i + 1) / 18.0, 1.0);
				glVertex3f(p[a + 1][0].x, p[a + 1][0].y, p[a + 1][0].z);

				glNormal3f(p[a][0].x, p[a][0].y, p[a][0].z);
				glTexCoord2f(i / 18.0, 1.0);
				glVertex3f(p[a][0].x, p[a][0].y, p[a][0].z);
			}
			else {
				glNormal3f(p[a][b].x, p[a][b].y, p[a][b].z);
				glTexCoord2f(i / 18.0, j / 36.0);
				glVertex3f(p[a][b].x, p[a][b].y, p[a][b].z);

				glNormal3f(p[a + 1][b].x, p[a + 1][b].y, p[a + 1][b].z);
				glTexCoord2f((i + 1) / 18.0, j / 36.0);
				glVertex3f(p[a + 1][b].x, p[a + 1][b].y, p[a + 1][b].z);

				glNormal3f(p[a + 1][b + 1].x, p[a + 1][b + 1].y, p[a + 1][b + 1].z);
				glTexCoord2f((i + 1) / 18.0, (j + 1) / 36.0);
				glVertex3f(p[a + 1][b + 1].x, p[a + 1][b + 1].y, p[a + 1][b + 1].z);

				glNormal3f(p[a][b + 1].x, p[a][b + 1].y, p[a][b + 1].z);
				glTexCoord2f(i / 18.0, (j + 1) / 36.0);
				glVertex3f(p[a][b + 1].x, p[a][b + 1].y, p[a][b + 1].z);
			}
			glEnd();
		}
	}
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

}


void
drawWoodQuads()
{
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1.0f, 1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glColor3f(1, 1, 1);
	for (int i = 0; i < 18; i++)
	{

		for (int j = 0; j < 36; j++)
		{
			glBindTexture(GL_TEXTURE_2D, texID[1]);
			glBegin(GL_QUADS);
			int a = i, b = j;
			if ((i + 1) == 18 && (j + 1) == 36) {
				glNormal3f(p[a][b].x, p[a][b].y, p[a][b].z);
				glTexCoord2f(i / 18.0, j / 36.0);
				glVertex3f(p[a][b].x, p[a][b].y, p[a][b].z);

				glNormal3f(p[0][b].x, p[0][b].y, p[0][b].z);
				glTexCoord2f(1.0, j / 36.0);
				glVertex3f(p[0][b].x, p[0][b].y, p[0][b].z);

				glNormal3f(p[0][0].x, p[0][0].y, p[0][0].z);
				glTexCoord2f(1.0, 1.0);
				glVertex3f(p[0][0].x, p[0][0].y, p[0][0].z);

				glNormal3f(p[a][0].x, p[a][0].y, p[a][0].z);
				glTexCoord2f(i / 18.0, 1.0);
				glVertex3f(p[a][0].x, p[a][0].y, p[a][0].z);
			}
			else if ((i + 1) == 18 && (j + 1) != 36) {
				glNormal3f(p[a][b].x, p[a][b].y, p[a][b].z);
				glTexCoord2f(i / 18.0, j / 36.0);
				glVertex3f(p[a][b].x, p[a][b].y, p[a][b].z);

				glNormal3f(p[0][b].x, p[0][b].y, p[0][b].z);
				glTexCoord2f(1.0, j / 36.0);
				glVertex3f(p[0][b].x, p[0][b].y, p[0][b].z);

				glNormal3f(p[0][b + 1].x, p[0][b + 1].y, p[0][b + 1].z);
				glTexCoord2f(1.0, (j + 1) / 36.0);
				glVertex3f(p[0][b + 1].x, p[0][b + 1].y, p[0][b + 1].z);

				glNormal3f(p[a][b + 1].x, p[a][b + 1].y, p[a][b + 1].z);
				glTexCoord2f(i / 18.0, (j + 1) / 36.0);
				glVertex3f(p[a][b + 1].x, p[a][b + 1].y, p[a][b + 1].z);
			}
			else if ((i + 1) != 18 && (j + 1) == 36) {
				glNormal3f(p[a][b].x, p[a][b].y, p[a][b].z);
				glTexCoord2f(i / 18.0, j / 36.0);
				glVertex3f(p[a][b].x, p[a][b].y, p[a][b].z);

				glNormal3f(p[a + 1][b].x, p[a + 1][b].y, p[a + 1][b].z);
				glTexCoord2f((i + 1) / 18.0, j / 36.0);
				glVertex3f(p[a + 1][b].x, p[a + 1][b].y, p[a + 1][b].z);

				glNormal3f(p[a + 1][0].x, p[a + 1][0].y, p[a + 1][0].z);
				glTexCoord2f((i + 1) / 18.0, 1.0);
				glVertex3f(p[a + 1][0].x, p[a + 1][0].y, p[a + 1][0].z);

				glNormal3f(p[a][0].x, p[a][0].y, p[a][0].z);
				glTexCoord2f(i / 18.0, 1.0);
				glVertex3f(p[a][0].x, p[a][0].y, p[a][0].z);
			}
			else {
				glNormal3f(p[a][b].x, p[a][b].y, p[a][b].z);
				glTexCoord2f(i / 18.0, j / 36.0);
				glVertex3f(p[a][b].x, p[a][b].y, p[a][b].z);

				glNormal3f(p[a + 1][b].x, p[a + 1][b].y, p[a + 1][b].z);
				glTexCoord2f((i + 1) / 18.0, j / 36.0);
				glVertex3f(p[a + 1][b].x, p[a + 1][b].y, p[a + 1][b].z);

				glNormal3f(p[a + 1][b + 1].x, p[a + 1][b + 1].y, p[a + 1][b + 1].z);
				glTexCoord2f((i + 1) / 18.0, (j + 1) / 36.0);
				glVertex3f(p[a + 1][b + 1].x, p[a + 1][b + 1].y, p[a + 1][b + 1].z);

				glNormal3f(p[a][b + 1].x, p[a][b + 1].y, p[a][b + 1].z);
				glTexCoord2f(i / 18.0, (j + 1) / 36.0);
				glVertex3f(p[a][b + 1].x, p[a][b + 1].y, p[a][b + 1].z);
			}
			glEnd();
		}
	}
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void
drawCheckQuads()
{
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1.0f, 1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glColor3f(1, 1, 1);

	for (int i = 0; i < 18; i++)
	{
		for (int j = 0; j < 36; j++)
		{
			glBindTexture(GL_TEXTURE_2D, texID[2]);
			glBegin(GL_QUADS);
			int a = i, b = j;
			if ((i + 1) == 18 && (j + 1) == 36) {
				glNormal3f(p[a][b].x, p[a][b].y, p[a][b].z);
				glTexCoord2f(i/18.0, j/36.0);
				glVertex3f(p[a][b].x, p[a][b].y, p[a][b].z);

				glNormal3f(p[0][b].x, p[0][b].y, p[0][b].z);
				glTexCoord2f(1.0, j/36.0);
				glVertex3f(p[0][b].x, p[0][b].y, p[0][b].z);

				glNormal3f(p[0][0].x, p[0][0].y, p[0][0].z);
				glTexCoord2f(1.0, 1.0);
				glVertex3f(p[0][0].x, p[0][0].y, p[0][0].z);

				glNormal3f(p[a][0].x, p[a][0].y, p[a][0].z);
				glTexCoord2f(i/18.0, 1.0);
				glVertex3f(p[a][0].x, p[a][0].y, p[a][0].z);
			}
			else if ((i + 1) == 18 && (j + 1) != 36) {
				glNormal3f(p[a][b].x, p[a][b].y, p[a][b].z);
				glTexCoord2f(i/18.0, j / 36.0);
				glVertex3f(p[a][b].x, p[a][b].y, p[a][b].z);

				glNormal3f(p[0][b].x, p[0][b].y, p[0][b].z);
				glTexCoord2f(1.0, j / 36.0);
				glVertex3f(p[0][b].x, p[0][b].y, p[0][b].z);

				glNormal3f(p[0][b + 1].x, p[0][b + 1].y, p[0][b + 1].z);
				glTexCoord2f(1.0, (j + 1) / 36.0);
				glVertex3f(p[0][b + 1].x, p[0][b + 1].y, p[0][b + 1].z);

				glNormal3f(p[a][b + 1].x, p[a][b + 1].y, p[a][b + 1].z);
				glTexCoord2f(i/18.0, (j + 1) / 36.0);
				glVertex3f(p[a][b + 1].x, p[a][b + 1].y, p[a][b + 1].z);
			}
			else if ((i + 1) != 18 && (j + 1) == 36) {
				glNormal3f(p[a][b].x, p[a][b].y, p[a][b].z);
				glTexCoord2f(i / 18.0, j/36.0);
				glVertex3f(p[a][b].x, p[a][b].y, p[a][b].z);

				glNormal3f(p[a + 1][b].x, p[a + 1][b].y, p[a + 1][b].z);
				glTexCoord2f((i + 1) / 18.0, j/36.0);
				glVertex3f(p[a + 1][b].x, p[a + 1][b].y, p[a + 1][b].z);

				glNormal3f(p[a + 1][0].x, p[a + 1][0].y, p[a + 1][0].z);
				glTexCoord2f((i + 1) / 18.0, 1.0);
				glVertex3f(p[a + 1][0].x, p[a + 1][0].y, p[a + 1][0].z);

				glNormal3f(p[a][0].x, p[a][0].y, p[a][0].z);
				glTexCoord2f(i / 18.0, 1.0);
				glVertex3f(p[a][0].x, p[a][0].y, p[a][0].z);
			}
			else {
				glNormal3f(p[a][b].x, p[a][b].y, p[a][b].z);
				glTexCoord2f(i / 18.0, j / 36.0);
				glVertex3f(p[a][b].x, p[a][b].y, p[a][b].z);

				glNormal3f(p[a + 1][b].x, p[a + 1][b].y, p[a + 1][b].z);
				glTexCoord2f((i + 1) / 18.0, j / 36.0);
				glVertex3f(p[a + 1][b].x, p[a + 1][b].y, p[a + 1][b].z);

				glNormal3f(p[a + 1][b + 1].x, p[a + 1][b + 1].y, p[a + 1][b + 1].z);
				glTexCoord2f((i + 1) / 18.0, (j + 1) / 36.0);
				glVertex3f(p[a + 1][b + 1].x, p[a + 1][b + 1].y, p[a + 1][b + 1].z);

				glNormal3f(p[a][b + 1].x, p[a][b + 1].y, p[a][b + 1].z);
				glTexCoord2f(i / 18.0, (j + 1) / 36.0);
				glVertex3f(p[a][b + 1].x, p[a][b + 1].y, p[a][b + 1].z);
			}
			glEnd();
		}
	}
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}



//Light
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

void
setupMaterial()
{
	// Make it possible to change a subset of material parameters
	glColorMaterial(GL_FRONT, GL_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);

	GLfloat mat_ambient[4] = { 0.1, 0.1, 0.1, 1 };
	GLfloat mat_specular[4] = { 0.5, 0.5, 0.5, 1 };
	GLfloat mat_shininess = 25;

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);

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
	;

	// Draw
	float	theta1 = frame * 4 / period;
	vec3	axis(0, 1, 0);
	glRotatef(theta1, axis.x, axis.y, axis.z);

	// Lighting
	setupLight();

	//setupMaterial();

	switch (selection)
	{
	case 1: drawMarbleQuads();           break;
	case 2: drawWoodQuads();			 break;
	case 3: drawCheckQuads();			 break;
	}

}


void
keyboard(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS || action == GLFW_REPEAT)
	{
		switch (key)
		{

			//Quit
		case GLFW_KEY_Q:
		case GLFW_KEY_ESCAPE: glfwSetWindowShouldClose(window, GL_TRUE); break;

			// Play on/off
		case GLFW_KEY_SPACE:	pause = !pause; 		break;


			// Time
		case GLFW_KEY_T:         break;

		case GLFW_KEY_1:        selection = 1;  break;
		case GLFW_KEY_2:        selection = 2;  break;
		case GLFW_KEY_3:        selection = 3;  break;

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
	//quit();

	// Terminate the glfw system
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;

}



