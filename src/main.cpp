// Charlotte Avra
// 24-780 - Engineering Computation
// Individual Mini-Project
// Demo : A non-interactive demonstration of audio and visual programming
// 3D Graphics from Scratch (First Attempt)
// October 31, 2022

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <time.h>
#include <math.h>
#include <vector>
#include "fssimplewindow.h"
#include "yssimplesound.h"
#include "yspng.h"
#include "ysglfontdata.h"
using namespace std;

struct vec3d
{
	float x, y, z;
};

struct triangle
{
	vec3d p[3];
};

struct mesh
{
	vector<triangle> tris;
};

struct mat4x4
{
	float m[4][4] = {0};
};

class Object3D
{
public:
	mesh meshCube;
	mesh meshOct;
	mat4x4 matProj;
	float thetaCube, offsetCube, thetaOct, offsetOct, originOct;
	int stateCube, stateOct;

	void multiplyMatrixVector(vec3d &i, vec3d &o, mat4x4 &m);
	void createProjection(int ScreenWidth, int ScreenHeight);
	void initializeCube();
	void initializeOct();
	void rotateDrawCube(int ScreenWidth, int ScreenHeight);
	void rotateDrawOct(int ScreenWidth, int ScreenHeight);
	void updateCubeTheta(void);
	void updateCubeOffset(void);
	void updateOctTheta(void);
	void updateOctOrgin(void);
};

class Grid
{
public:
	double x1, y1, x2, y2;

	void Draw(void);
	void Move(double vel, double accel, double dt, int yScene);
	void Lower(double lowerVel);
};

class Road
{
public:
	double x1, y1, x2, y2, state;

	void Initialize(void);
	void Draw(void);
	void Move(void);
	void Lower(double lowerVel);
};

class Sun
{
public:
	int rad, state;
	double cx, cy;

	void Initialize(void);
	void Draw(void);
	void Move(void);
	void Lower(double lowerVel);
};

void setBackground(int yScene) // top two gradients that make up the sky
{
	glShadeModel(GL_SMOOTH); // gradient background
	glBegin(GL_QUADS);

	glColor3ub(1, 0, 16); // same RGB as bottom of stars.png so blends well
	glVertex2i(0, yScene - 400);

	glColor3ub(1, 0, 16);
	glVertex2i(800, yScene - 400);

	glColor3ub(78, 64, 177);
	glVertex2i(800, yScene + 200);

	glColor3ub(78, 64, 177);
	glVertex2i(0, yScene + 200);

	glEnd();

	glShadeModel(GL_SMOOTH); // gradient background
	glBegin(GL_QUADS);

	glColor3ub(78, 64, 177);
	glVertex2i(0, yScene + 200);

	glColor3ub(78, 64, 177);
	glVertex2i(800, yScene + 200);

	glColor3ub(58, 156, 253);
	glVertex2i(800, yScene + 400);

	glColor3ub(58, 156, 253);
	glVertex2i(0, yScene + 400);

	glEnd();
}

void setForeground(int yScene) // background underneath grid
{
	glBegin(GL_QUADS);

	glColor3ub(44, 40, 62);
	glVertex2i(0, yScene + 400);
	glVertex2i(800, yScene + 400);
	glVertex2i(800, yScene + 600);
	glVertex2i(0, yScene + 600);

	glEnd();
}

void setBlackScene(void)
{
	glBegin(GL_QUADS);

	glColor3ub(0, 0, 0);
	glVertex2i(0, 0);
	glVertex2i(800, 0);
	glVertex2i(800, 600);
	glVertex2i(0, 600);

	glEnd();
}

void moveScene(int &yScene, double lowerVel)
{
	yScene += lowerVel;
}

void Object3D::multiplyMatrixVector(vec3d &i, vec3d &o, mat4x4 &m)
{
	o.x = (i.x * m.m[0][0]) + (i.y * m.m[1][0]) + (i.z * m.m[2][0]) + m.m[3][0];
	o.y = (i.x * m.m[0][1]) + (i.y * m.m[1][1]) + (i.z * m.m[2][1]) + m.m[3][1];
	o.z = (i.x * m.m[0][2]) + (i.y * m.m[1][2]) + (i.z * m.m[2][2]) + m.m[3][2];
	float w = (i.x * m.m[0][3]) + (i.y * m.m[1][3]) + (i.z * m.m[2][3]) + m.m[3][3];

	if (w != 0.0f)
	{
		o.x /= w;
		o.y /= w;
		o.z /= w;
	}
}

void Object3D::initializeCube()
{
	meshCube.tris =
		{
			// front
			{0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f},
			{0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f},

			// back
			{1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f},
			{1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f},

			// left
			{0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f},
			{0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f},

			// right
			{1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f},
			{1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f},

			// top
			{0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f},
			{0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f},

			// bottom
			{1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f},
			{1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f},
		};
}

void Object3D::initializeOct()
{
	meshOct.tris =
		{
			// top front
			{-0.5f, originOct, -0.5f, 0.0f, originOct + 1.0f, 0.0f, 0.5f, originOct, -0.5f},

			// top right
			{0.5f, originOct, -0.5f, 0.0f, originOct + 1.0f, 0.0f, 0.5f, originOct, 0.5f},

			// top back
			{0.5f, originOct, 0.5f, 0.0f, originOct + 1.0f, 0.0f, -0.5f, originOct, 0.5f},

			// top left
			{-0.5f, originOct, 0.5f, 0.0f, originOct + 1.0f, 0.0f, -0.5f, originOct, -0.5f},

			// bottom front
			{-0.5f, originOct, 0.5f, 0.5f, originOct, -0.5f, 0.0f, originOct - 1.0f, 0.0f},

			// bottom right
			{0.5f, originOct, -0.5f, 0.5f, originOct, 0.5f, 0.0f, originOct - 1.0f, 0.0f},

			// bottom back
			{0.5f, originOct, 0.5f, -0.5f, originOct, 0.5f, 0.0f, originOct - 1.0f, 0.0f},

			// bottom left
			{-0.5f, originOct, 0.5f, -0.5f, originOct, -0.5f, 0.0f, originOct - 1.0f, 0.0f},
		};
}

void Object3D::createProjection(int ScreenWidth, int ScreenHeight)
{
	// projection matrix
	const double YS_PI = 3.14115927;
	float fNear = 0.1f;
	float fFar = 1000.0f;
	float znorm = fFar / (fFar - fNear);
	float fFov = 90.0f;
	float fAspectRatio = (float)ScreenHeight / (float)ScreenWidth;
	float fFovRad = 1.0f / tanf(fFov * 0.5f / 180.0f * YS_PI);

	matProj.m[0][0] = fAspectRatio * fFovRad;
	matProj.m[1][1] = fFovRad;
	matProj.m[2][2] = znorm;
	matProj.m[2][3] = 1.0f;
	matProj.m[3][2] = -1 * znorm * fNear;
}

void Object3D::rotateDrawCube(int ScreenWidth, int ScreenHeight)
{
	mat4x4 matRotZ, matRotX;

	// rotation about the z-axis
	matRotZ.m[0][0] = cosf(thetaCube);
	matRotZ.m[0][1] = sinf(thetaCube);
	matRotZ.m[1][0] = -sinf(thetaCube);
	matRotZ.m[1][1] = cosf(thetaCube);
	matRotZ.m[2][2] = 1;
	matRotZ.m[3][3] = 1;

	// roation about the x-axis
	matRotX.m[0][0] = 1;
	matRotX.m[1][1] = cosf(thetaCube * 0.5f);
	matRotX.m[1][2] = sinf(thetaCube * 0.5f);
	matRotX.m[2][1] = -sinf(thetaCube * 0.5f);
	matRotX.m[2][2] = cosf(thetaCube * 0.5f);
	matRotX.m[3][3] = 1;

	for (auto tri : meshCube.tris)
	{
		// define triangle coordinates
		triangle triProjected, triTranslated, triRotatedX, triRotatedZX;

		// rotate about origin of cube
		multiplyMatrixVector(tri.p[0], triRotatedX.p[0], matRotZ);
		multiplyMatrixVector(tri.p[1], triRotatedX.p[1], matRotZ);
		multiplyMatrixVector(tri.p[2], triRotatedX.p[2], matRotZ);

		multiplyMatrixVector(triRotatedX.p[0], triRotatedZX.p[0], matRotX);
		multiplyMatrixVector(triRotatedX.p[1], triRotatedZX.p[1], matRotX);
		multiplyMatrixVector(triRotatedX.p[2], triRotatedZX.p[2], matRotX);

		// translate z coordinate of triangle coordinates into real space
		triTranslated = triRotatedZX;
		triTranslated.p[0].z = triRotatedZX.p[0].z + offsetCube;
		triTranslated.p[1].z = triRotatedZX.p[1].z + offsetCube;
		triTranslated.p[2].z = triRotatedZX.p[2].z + offsetCube;

		multiplyMatrixVector(triTranslated.p[0], triProjected.p[0], matProj);
		multiplyMatrixVector(triTranslated.p[1], triProjected.p[1], matProj);
		multiplyMatrixVector(triTranslated.p[2], triProjected.p[2], matProj);

		// scale coordinates from (-1, 1) to (0, 2)
		triProjected.p[0].x += 1.0f;
		triProjected.p[0].y += 1.0f;
		triProjected.p[1].x += 1.0f;
		triProjected.p[1].y += 1.0f;
		triProjected.p[2].x += 1.0f;
		triProjected.p[2].y += 1.0f;

		// scale coordinates from (0, 2) to
		triProjected.p[0].x *= 0.5f * (float)ScreenWidth;
		triProjected.p[0].y *= 0.5f * (float)ScreenHeight;
		triProjected.p[1].x *= 0.5f * (float)ScreenWidth;
		triProjected.p[1].y *= 0.5f * (float)ScreenHeight;
		triProjected.p[2].x *= 0.5f * (float)ScreenWidth;
		triProjected.p[2].y *= 0.5f * (float)ScreenHeight;

		// draw triangles
		glBegin(GL_LINE_LOOP);
		glColor3ub(22, 255, 255);
		glVertex2i(triProjected.p[0].x, triProjected.p[0].y);
		glVertex2i(triProjected.p[1].x, triProjected.p[1].y);
		glVertex2i(triProjected.p[2].x, triProjected.p[2].y);
		glEnd();
	}
}

void Object3D::updateCubeTheta()
{
	thetaCube += 0.01f;
}

void Object3D::updateCubeOffset()
{
	if (offsetCube < 46.8 && offsetCube > 46.0)
	{
		offsetCube = 10.0f; // cube jumps closer to viewer
	}
	if (offsetCube < 7.2 && offsetCube > 6.0)
	{
		offsetCube = 3.0f; // cube jumps closer to viewer
	}
	if (offsetCube < -4.0 && offsetCube > -6.0)
	{
		offsetCube = -20.0f; // cube jumps further from viewer
	}
	else
	{
		offsetCube -= 0.008f;
	}
}

void Object3D::rotateDrawOct(int ScreenWidth, int ScreenHeight)
{
	mat4x4 matRotZ, matRotX;

	// rotation about the z-axis
	matRotZ.m[0][0] = cosf(thetaOct);
	matRotZ.m[0][2] = -sinf(thetaOct);
	matRotZ.m[2][0] = sinf(thetaOct);
	matRotZ.m[1][1] = 1;
	matRotZ.m[2][2] = cosf(thetaOct);
	matRotZ.m[3][3] = 1;

	for (auto tri : meshOct.tris)
	{
		// define triangle coordinates
		triangle triProjected, triTranslated, triRotatedX;

		// rotate about origin of cube
		multiplyMatrixVector(tri.p[0], triRotatedX.p[0], matRotZ);
		multiplyMatrixVector(tri.p[1], triRotatedX.p[1], matRotZ);
		multiplyMatrixVector(tri.p[2], triRotatedX.p[2], matRotZ);

		// translate z coordinate of triangle coordinates into real space
		triTranslated = triRotatedX;
		triTranslated.p[0].z = triRotatedX.p[0].z + offsetOct;
		triTranslated.p[1].z = triRotatedX.p[1].z + offsetOct;
		triTranslated.p[2].z = triRotatedX.p[2].z + offsetOct;

		multiplyMatrixVector(triTranslated.p[0], triProjected.p[0], matProj);
		multiplyMatrixVector(triTranslated.p[1], triProjected.p[1], matProj);
		multiplyMatrixVector(triTranslated.p[2], triProjected.p[2], matProj);

		// scale coordinates from (-1, 1) to (0, 2)
		triProjected.p[0].x += 1.0f;
		triProjected.p[0].y += 1.0f;
		triProjected.p[1].x += 1.0f;
		triProjected.p[1].y += 1.0f;
		triProjected.p[2].x += 1.0f;
		triProjected.p[2].y += 1.0f;

		// scale coordinates from (0, 2) to
		triProjected.p[0].x *= 0.5f * (float)ScreenWidth;
		triProjected.p[0].y *= 0.5f * (float)ScreenHeight;
		triProjected.p[1].x *= 0.5f * (float)ScreenWidth;
		triProjected.p[1].y *= 0.5f * (float)ScreenHeight;
		triProjected.p[2].x *= 0.5f * (float)ScreenWidth;
		triProjected.p[2].y *= 0.5f * (float)ScreenHeight;

		// draw triangles
		glBegin(GL_LINE_LOOP);
		glColor3ub(255, 255, 255);
		glVertex2i(triProjected.p[0].x, triProjected.p[0].y);
		glVertex2i(triProjected.p[1].x, triProjected.p[1].y);
		glVertex2i(triProjected.p[2].x, triProjected.p[2].y);
		glEnd();
	}
}

void Object3D::updateOctTheta()
{
	thetaOct += 0.01f;
}

void Object3D::updateOctOrgin()
{
	originOct += 0.01f;
}

void Grid::Draw()
{
	glBegin(GL_LINE_LOOP);
	glColor3ub(255, 0, 255);
	glVertex2i(x1, y1);
	glVertex2i(x2, y2);
	glEnd();
}

void Grid::Move(double vel, double accel, double dt, int yScene)
{
	float del, t;
	int foretoBack = yScene + 400;

	if (y1 - foretoBack > 200) // y1 should always equal y2 because the values passed to this function are those of a horizontal line
	{
		y1 = foretoBack + 1; // bring lines back to 1 pixels below the intersection of background and foreground (foretoBack) if they go off the bottom of the screen
		y2 = foretoBack + 1;
	}

	else
	{
		del = y1 - 400;								 // distance of y1 and y2 from starting position
		t = del * dt;								 // time y1 and y2 have been on the screen
		y1 += (vel * t) + (0.5 * accel * pow(t, 2)); // accelerate y value of horizontal line toward the bottom of the screen
		y2 += (vel * t) + (0.5 * accel * pow(t, 2));
	}
}

void Grid::Lower(double lowerVel)
{
	y1 += lowerVel; // y1 may not equal y2, for loop iterates through all grid lines horizontal or not
	y2 += lowerVel;
}

void Road::Initialize()
{
	x1 = 400 - 238;
	y1 = 600;
	x2 = 400 + 238;
	y2 = 600;
	state = 0; // 0 = not drawn; 1 = drawn; 2 = lowering; 3 = out of view
}

void Road::Draw()
{
	double x3, y3, x4, y4;
	x3 = 400 + 238;
	y3 = 600;
	x4 = 400 - 238;
	y4 = 600;

	glBegin(GL_QUADS);
	glColor3ub(37, 29, 52);
	glVertex2d(x1, y1);
	glVertex2d(x2, y2);
	glVertex2d(x3, y3);
	glVertex2d(x4, y4);
	glEnd();
}

void Road::Move()
{
	y1 -= 0.5;
	y2 -= 0.5;
	x1 = ((y1 - 400) / -1.04) + 354;
	x2 = ((y1 - 400) / 1.04) + 446;
}

void Road::Lower(double lowerVel)
{
	y1 += lowerVel;
	y2 += lowerVel;
}

void Sun::Initialize()
{
	rad = 150;
	state = 0; // 0 = not drawn; 1 = drawn; 2 = out of view
	cx = 400;
	cy = 400 + rad; // initially draw below the horizon line
}

void Sun::Draw()
{
	const double YS_PI = 3.14115927;
	glShadeModel(GL_SMOOTH);
	glBegin(GL_POLYGON); // sun (circle) is estimated as a polygon with 64 sides

	for (int i = 0; i < 64; i++)
	{
		double angle = ((double)i * (YS_PI / 32.0)) + (0.5 * YS_PI); // 32.0 represents the angles formed by 64 points on a circle
		double x = (double)cx + cos(angle) * (double)rad;
		double y = (double)cy + sin(angle) * (double)rad;
		if (angle == 0.5 * YS_PI)
		{
			glColor3ub(252, 20, 252);
		}
		else
		{
			glColor3ub(22, 255, 255);
		}
		glVertex2d(x, y);
	}
	glEnd();
}

void Sun::Move()
{
	cy -= 0.5;
}

void Sun::Lower(double lowerVel)
{
	cy += lowerVel;
}

void introText(void)
{
	glColor3ub(255, 255, 255);
	glRasterPos2d(100, 100);
	YsGlDrawFontBitmap12x16("3D Graphics from Scratch (First Attempt)");
	glFlush();

	glColor3ub(255, 255, 255);
	glRasterPos2d(100, 150);
	YsGlDrawFontBitmap10x14("Charlotte Avra, Demoscene Individual Project");
	glFlush();

	glColor3ub(255, 255, 255);
	glRasterPos2d(100, 200);
	YsGlDrawFontBitmap10x14("October 31, 2022");
	glFlush();
}

void theEnd(void)
{
	glColor3ub(255, 255, 255);
	glRasterPos2d(300, 400);
	YsGlDrawFontBitmap20x32("The End");
	glFlush();

	glColor3ub(255, 255, 255);
	glRasterPos2d(300, 450);
	YsGlDrawFontBitmap10x14("Thank you for watching");
	glFlush();
}

int main(void)
{
	// set up background music
	YsSoundPlayer player;
	YsSoundPlayer::SoundData wav;
	char fName[27] = "media/DensityTime_MAZE.wav";

	if (YSOK != wav.LoadWav(fName))
	{
		printf("Failed to read %s\n", fName);
		return 1;
	}

	// initialize screen variables
	int ScreenWidth = 800;
	int ScreenHeight = 600;
	double lowerVel = 2.0;

	// initialize class variables
	Object3D obj;
	obj.thetaCube = 0.0f;
	obj.offsetCube = 50.0f;
	obj.stateCube = 1; // 0 = not in view; 1 = in view; 2 = after stars.png

	obj.thetaOct = 0.0f;
	obj.offsetOct = 5.0f;
	obj.stateOct = 0; // 0 = not completed scene; 1 = completed scene
	obj.originOct = -7.0f;

	Road road;
	road.Initialize();

	Sun sun;
	sun.Initialize();

	const int numLines = 16;
	Grid grid[numLines];

	// initialize scene variables
	int yScene = 0;		// top of background scene
	int stateScene = 1; // 0 = not in view; 1 = in view
	int endState = 0;	// 0 = not in last scene; 1 = in last scene

	// initialize grid variables
	const double vel = 3, accel = 0.0001, dt = 0.01;

	// horizontal lines
	grid[0].x1 = 0;
	grid[0].y1 = 400;
	grid[0].x2 = 800;
	grid[0].y2 = 400;
	grid[1].x1 = 0;
	grid[1].y1 = 400 + 5;
	grid[1].x2 = 800;
	grid[1].y2 = 400 + 5;
	grid[2].x1 = 0;
	grid[2].y1 = 400 + 15;
	grid[2].x2 = 800;
	grid[2].y2 = 400 + 15;
	grid[3].x1 = 0;
	grid[3].y1 = 400 + 40;
	grid[3].x2 = 800;
	grid[3].y2 = 400 + 40;
	grid[4].x1 = 0;
	grid[4].y1 = 400 + 90;
	grid[4].x2 = 800;
	grid[4].y2 = 400 + 90;
	grid[5].x1 = 0;
	grid[5].y1 = 400 + 195;
	grid[5].x2 = 800;
	grid[5].y2 = 400 + 195;

	// vertical lines
	grid[6].x1 = 400 + 20;
	grid[6].y1 = 400;
	grid[6].x2 = 400 + 110;
	grid[6].y2 = 600;
	grid[7].x1 = 400 - 20;
	grid[7].y1 = 400;
	grid[7].x2 = 400 - 110;
	grid[7].y2 = 600;
	grid[8].x1 = 400 + 48;
	grid[8].y1 = 400;
	grid[8].x2 = 400 + 240;
	grid[8].y2 = 600;
	grid[9].x1 = 400 - 48;
	grid[9].y1 = 400;
	grid[9].x2 = 400 - 240;
	grid[9].y2 = 600;
	grid[10].x1 = 400 + 88;
	grid[10].y1 = 400;
	grid[10].x2 = 800;
	grid[10].y2 = 524;
	grid[11].x1 = 400 - 88;
	grid[11].y1 = 400;
	grid[11].x2 = 0;
	grid[11].y2 = 524;
	grid[12].x1 = 400 + 150;
	grid[12].y1 = 400;
	grid[12].x2 = 800;
	grid[12].y2 = 455;
	grid[13].x1 = 400 - 150;
	grid[13].y1 = 400;
	grid[13].x2 = 0;
	grid[13].y2 = 455;
	grid[14].x1 = 400 + 250;
	grid[14].y1 = 400;
	grid[14].x2 = 800;
	grid[14].y2 = 420;
	grid[15].x1 = 400 - 250;
	grid[15].y1 = 400;
	grid[15].x2 = 0;
	grid[15].y2 = 420;

	// initialize time variables
	int iniTime = time(NULL);
	int finalTime;
	int sunTime = 0, endTime = 0;

	// initialize road variables
	road.Initialize();

	// set up .png decoder
	YsRawPngDecoder png;
	png.Decode("media/stars.png");
	png.Flip();
	if (0 == png.wid || 0 == png.hei)
	{
		printf("Failed to load image.\n");
		return 1;
	}

	FsOpenWindow(0, 0, ScreenWidth, ScreenHeight, 1);

	///////////////////// STARTING SCENE (NO MUSIC) /////////////////////
	int terminate1 = 0;
	while (terminate1 == 0)
	{
		FsPollDevice();
		int key = FsInkey();

		switch (key)
		{
		case FSKEY_ESC:
			terminate1 = 1;
			break;
		}

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		int curTime = time(NULL);
		setBlackScene();
		introText();

		if (curTime - iniTime > 5.0)
		{
			terminate1 = 1;
		}
		FsSwapBuffers();
		FsSleep(10); // 10 milliseconds
	}

	player.Start();
	player.PlayOneShot(wav);

	iniTime = time(NULL);
	int terminate2 = 0;
	while (terminate2 == 0)
	{
		player.KeepPlaying();

		FsPollDevice();
		int key = FsInkey();

		switch (key)
		{
		case FSKEY_ESC:
			terminate2 = 1;
			player.End();
			break;
		}

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		///////////////////// SET BACKGROUND /////////////////////
		if (stateScene == 1)
		{
			setBackground(yScene);
		}

		///////////////////// DRAW AND RAISE SUN /////////////////////
		if (sun.state < 2)
		{
			sun.Draw();
		}

		if (road.state == 1 && sun.state == 0) // if the road is drawn raise the sun
		{
			sun.Move();

			if (sun.cy <= 330)
			{
				sun.state = 1; // sun is drawn
				sunTime = time(NULL);
			}
		}

		///////////////////// DRAW FOREGROUND OVER SUN /////////////////////
		if (stateScene == 1)
		{
			setForeground(yScene);

			///////////////////// DRAW MOVING GRIDLINES /////////////////////
			for (int i = 0; i < numLines - 9; i++) // iterate through only horizontal lines
			{
				grid[i].Draw();
				grid[i].Move(vel, accel, dt, yScene); // only horizontal lines move
			}

			for (int i = 6; i < numLines; i++) // iterate through vertical lines
			{
				grid[i].Draw();
			}
		}

		///////////////////// DRAW ROTATING CUBE /////////////////////
		if (obj.stateCube == 1)
		{
			obj.initializeCube();
			obj.createProjection(ScreenWidth, ScreenHeight);
			obj.rotateDrawCube(ScreenWidth, ScreenHeight);
			obj.updateCubeTheta();
			obj.updateCubeOffset();

			if (obj.offsetCube < -23.3)
			{
				obj.stateCube = 0; // draw cube until it reaches a point far in the distance
			}
		}

		///////////////////// DRAW ROAD OVER GRIDLINES /////////////////////
		if (obj.offsetCube < -2.2) // start to draw road once cube is at an offset of -2.2
		{
			road.Draw();

			if (road.state == 0 && road.y1 > 400)
			{
				road.Move();

				if (road.y1 <= 400)
				{
					road.state = 1; // road drawing complete
				}
			}
		}

		///////////////////// PAN USER VIEW UP BY LOWERING ALL OBJECTS IN FRAME /////////////////////
		int curTime = time(NULL);
		if (sun.state == 1 && (curTime - sunTime) > 2.0) // wait 2 seconds
		{
			moveScene(yScene, lowerVel); // grid and background underneath are lowered out of the screen to create a camera panning upward effect

			glRasterPos2i(0, yScene - 400);
			glDrawPixels(png.wid, png.hei, GL_RGBA, GL_UNSIGNED_BYTE, png.rgba);

			road.Lower(lowerVel);
			road.state = 2; // lowering out of view

			sun.Lower(lowerVel);

			for (int i = 0; i < numLines; i++)
			{
				grid[i].Lower(lowerVel);
			}

			if (yScene - 400 >= 600) // initial background completely out of view
			{
				stateScene = 0; // set all states of objects to out of view
				road.state = 3;
				sun.state = 2;
			}
		}

		///////////////////// ROTATING OCTEHEDRON FLOATS DOWN THROUGH SCREEN /////////////////////
		if (stateScene == 0)
		{
			glRasterPos2i(0, 600 - 1); // hold stars.png at position of bottom edge (0,600-1)
			glDrawPixels(png.wid, png.hei, GL_RGBA, GL_UNSIGNED_BYTE, png.rgba);

			obj.initializeOct();
			obj.createProjection(ScreenWidth, ScreenHeight);
			obj.rotateDrawOct(ScreenWidth, ScreenHeight);
			obj.updateOctTheta();
			obj.updateOctOrgin();

			if (obj.originOct > 7.0)
			{
				stateScene = 2;
				obj.stateOct = 1;
				endTime = time(NULL);
			}
		}

		///////////////////// END SCENE /////////////////////
		curTime = time(NULL);
		if (obj.stateOct == 1)
		{
			setBlackScene();
			theEnd();

			if (curTime - endTime > 3.0)
			{
				terminate2 = 1; // end program after "The End" is displayed for 3 seconds
			}
		}

		///////////////////// SWAP BUFFERS AND SLEEP /////////////////////
		FsSwapBuffers();
		FsSleep(10); // 10 milliseconds
	}

	finalTime = time(NULL);
	printf("Total runtime: %d seconds", finalTime - iniTime);

	return 0;
}