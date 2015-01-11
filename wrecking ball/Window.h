#ifndef _WINDOW_H_
#define _WINDOW_H_

#define M_PI 3.14159265358979323846
#define ROT_SCALAR 90.0;
#define ZOOM_SCALAR 1.0;

#include <time.h>
#include <iostream>
#include <vector>
#include <string>
#include <math.h>
#include <thread> 

#include "main.h"
#include "Constant.h"
#include "Camera.h"
#include "MatrixTransform.h"
#include "Matrix4d.h"
#include "Vector3d.h"
#include "Vector4d.h"
#include "Cube.h"
#include <bullet/btBulletDynamicsCommon.h>
#include <bullet/BulletSoftBody/btSoftRigidDynamicsWorld.h>
#include <bullet/BulletSoftBody/btDefaultSoftBodySolver.h>
#include <bullet/BulletSoftBody/btSoftBodyHelpers.h>
#include <bullet/BulletSoftBody/btSoftBodyRigidBodyCollisionConfiguration.h>

using namespace std;

struct bulletObject{
	int id;
	float r, g, b;
	bool hit;
	bool selected;
	btRigidBody* body;
	bulletObject(btRigidBody* b, int i, float r0, float g0, float b0) : body(b), id(i), r(r0), g(g0), b(b0), hit(false), selected(false) {}
};

class Window	  // OpenGL output window related routines
{
	public:
	// Window Size
	static int width, height;

	// Track Ball
	static int prevX, prevY;
	static Vector3d lastPoint;
	static int movement;

	// Scene
	static MatrixTransform* root;
	static MatrixTransform* rotation;
	static MatrixTransform* scaling;

	// Frame Calculator
	static int time;
	static int baseTime;
	static int frames;
	static GLdouble fps;

	// Camera
	static Camera camera;

	static void init();

	// Callback Functions
	static void idleCallback(void);
	static void reshapeCallback(int, int);
	static void displayCallback(void);

	// communicate with keyboard
	static void keyBoardCallBack(unsigned char, int, int);
	static void SpecialKeysCallBack(int, int, int);

	// communicate with mouse
	static void MouseClickCallBack(int, int, int, int);
	static void MouseMotionCallBack(int, int);

	// Track Ball mapping
	static Vector3d trackBallMapping(int, int);

	// bullet related functions
	static btRigidBody* addSphere(float rad, float x, float y, float z, float mass);
	static void renderSphere(bulletObject* bobj);
	static btRigidBody* addBox(float width, float height, float depth, float x, float y, float z, float mass);
	static void renderBox(bulletObject* bobj);
	static void renderPlane(bulletObject* bobj);
	static void renderSoftbody(btSoftBody* b);

};

#endif