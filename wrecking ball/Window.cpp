#define _CRT_SECURE_NO_WARNINGS

#include "Window.h"
#include <vector>

// Window
int Window::width = 512;   // set window width in pixels here
int Window::height = 512;   // set window height in pixels here

// Track Ball
int Window::prevX = width / 2;
int Window::prevY = height / 2;
int Window::movement;
Vector3d Window::lastPoint;
MatrixTransform* Window::root;
MatrixTransform* Window::rotation;
MatrixTransform* Window::scaling;

// Frame Calculator 
int Window::time = 0;
int Window::baseTime = 0;
int Window::frames = 0;
GLdouble Window::fps = 0;

// Camera(center, look at, up)
Vector3d eye(0.0, 20.0, 50.0);
Vector3d lookat(0.0, 0.0, 0.0);
Vector3d up(0.0, 1.0, 0.0);
Camera Window::camera(eye, lookat, up);

// physics world parameters
btDynamicsWorld* world;
btDispatcher* dispatcher;
btCollisionConfiguration* collisionConfig;
btBroadphaseInterface* broadphase;
btConstraintSolver* solver;
btDefaultSoftBodySolver* softbodySolver;
std::vector<bulletObject*> bodies;





btRigidBody* Window::addSphere(float rad, float x, float y, float z, float mass)
{
	btTransform t;
	t.setIdentity();
	t.setOrigin(btVector3(x, y, z));
	btSphereShape* sphere = new btSphereShape(rad);
	btVector3 inertia(0, 0, 0);
	if (mass != 0.0)
		sphere->calculateLocalInertia(mass, inertia);

	btMotionState* motion = new btDefaultMotionState(t);
	btRigidBody::btRigidBodyConstructionInfo info(mass, motion, sphere, inertia);
	btRigidBody* body = new btRigidBody(info);
	world->addRigidBody(body);
	bodies.push_back(new bulletObject(body, 0, 1.0, 0.0, 0.0));
	body->setUserPointer(bodies[bodies.size() - 1]);
	return body;
}

void Window::renderSphere(bulletObject *bobj)
{
	btRigidBody* sphere = bobj->body;
	if (sphere->getCollisionShape()->getShapeType() != SPHERE_SHAPE_PROXYTYPE)
		return;
	if (!bobj->hit)
		glColor3f(bobj->r, bobj->g, bobj->b);
	else
		glColor3f(1, 0, 0);

	float r = ((btSphereShape*)sphere->getCollisionShape())->getRadius();
	btTransform t;
	sphere->getMotionState()->getWorldTransform(t);
	float mat[16];
	t.getOpenGLMatrix(mat);
	glPushMatrix();
	glMultMatrixf(mat);     //translation,rotation
	glutSolidSphere(1, 20, 20);
	glPopMatrix();
}

btRigidBody* Window::addBox(float width, float height, float depth, float x, float y, float z, float mass)
{
	btTransform t;
	t.setIdentity();
	t.setOrigin(btVector3(x, y, z));
	btBoxShape* sphere = new btBoxShape(btVector3(width / 2.0, height / 2.0, depth / 2.0));
	btVector3 inertia(0, 0, 0);
	if (mass != 0.0)
		sphere->calculateLocalInertia(mass, inertia);

	btMotionState* motion = new btDefaultMotionState(t);
	btRigidBody::btRigidBodyConstructionInfo info(mass, motion, sphere, inertia);
	btRigidBody* body = new btRigidBody(info);
	world->addRigidBody(body);
	bodies.push_back(new bulletObject(body, 3, 1.0, 1.0, 0.0));
	body->setUserPointer(bodies[bodies.size() - 1]);
	return body;
}

void Window::renderBox(bulletObject* bobj)
{
	btRigidBody* sphere = bobj->body;
	if (sphere->getCollisionShape()->getShapeType() != BOX_SHAPE_PROXYTYPE)
		return;
	if (!bobj->hit)
		glColor3f(bobj->r, bobj->g, bobj->b);
	else
		glColor3f(1, 0, 0);
	btVector3 extent = ((btBoxShape*)sphere->getCollisionShape())->getHalfExtentsWithoutMargin();
	btTransform t;
	sphere->getMotionState()->getWorldTransform(t);
	float mat[16];
	t.getOpenGLMatrix(mat);
	glPushMatrix();
	glMultMatrixf(mat);     //translation,rotation
	glBegin(GL_QUADS);
	glVertex3f(-extent.x(), extent.y(), -extent.z());
	glVertex3f(-extent.x(), -extent.y(), -extent.z());
	glVertex3f(-extent.x(), -extent.y(), extent.z());
	glVertex3f(-extent.x(), extent.y(), extent.z());
	glEnd();
	glBegin(GL_QUADS);
	glVertex3f(extent.x(), extent.y(), -extent.z());
	glVertex3f(extent.x(), -extent.y(), -extent.z());
	glVertex3f(extent.x(), -extent.y(), extent.z());
	glVertex3f(extent.x(), extent.y(), extent.z());
	glEnd();
	glBegin(GL_QUADS);
	glVertex3f(-extent.x(), extent.y(), extent.z());
	glVertex3f(-extent.x(), -extent.y(), extent.z());
	glVertex3f(extent.x(), -extent.y(), extent.z());
	glVertex3f(extent.x(), extent.y(), extent.z());
	glEnd();
	glBegin(GL_QUADS);
	glVertex3f(-extent.x(), extent.y(), -extent.z());
	glVertex3f(-extent.x(), -extent.y(), -extent.z());
	glVertex3f(extent.x(), -extent.y(), -extent.z());
	glVertex3f(extent.x(), extent.y(), -extent.z());
	glEnd();
	glBegin(GL_QUADS);
	glVertex3f(-extent.x(), extent.y(), -extent.z());
	glVertex3f(-extent.x(), extent.y(), extent.z());
	glVertex3f(extent.x(), extent.y(), extent.z());
	glVertex3f(extent.x(), extent.y(), -extent.z());
	glEnd();
	glBegin(GL_QUADS);
	glVertex3f(-extent.x(), -extent.y(), -extent.z());
	glVertex3f(-extent.x(), -extent.y(), extent.z());
	glVertex3f(extent.x(), -extent.y(), extent.z());
	glVertex3f(extent.x(), -extent.y(), -extent.z());
	glEnd();
	glPopMatrix();
}

void Window::renderPlane(bulletObject* bobj)
{
	btRigidBody* plane = bobj->body;
	if (plane->getCollisionShape()->getShapeType() != STATIC_PLANE_PROXYTYPE)
		return;
	if (!bobj->hit)
		glColor3f(bobj->r, bobj->g, bobj->b);
	else
		glColor3f(1, 0, 0);
	btTransform t;
	plane->getMotionState()->getWorldTransform(t);
	float mat[16];
	t.getOpenGLMatrix(mat);
	glPushMatrix();
	glMultMatrixf(mat);     //translation,rotation
	glBegin(GL_QUADS);
	glVertex3f(-1000, 0, 1000);
	glVertex3f(-1000, 0, -1000);
	glVertex3f(1000, 0, -1000);
	glVertex3f(1000, 0, 1000);
	glEnd();
	glPopMatrix();
}

void Window::init() {
	root = new MatrixTransform(Matrix4d());
	rotation = new MatrixTransform(Matrix4d());
	scaling = new MatrixTransform(Matrix4d());

	root->addChild(scaling);
	scaling->addChild(rotation);
	
	
	// setting up physics world
	collisionConfig = new btSoftBodyRigidBodyCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collisionConfig);
	broadphase = new btDbvtBroadphase();
	solver = new btSequentialImpulseConstraintSolver();
	softbodySolver = new btDefaultSoftBodySolver();
	world = new btSoftRigidDynamicsWorld(dispatcher, broadphase, solver, collisionConfig);
	world->setGravity(btVector3(0, -10, 0));


	// setting up ground
	btTransform t;
	t.setIdentity();
	t.setOrigin(btVector3(0, 0, 0));
	btStaticPlaneShape* plane = new btStaticPlaneShape(btVector3(0, 1, 0), 0);
	btMotionState* motion = new btDefaultMotionState(t);
	btRigidBody::btRigidBodyConstructionInfo info(0.0, motion, plane);
	btRigidBody* body = new btRigidBody(info);
	world->addRigidBody(body);
	bodies.push_back(new bulletObject(body, 4, 0.8, 0.8, 0.8));
	body->setUserPointer(bodies[bodies.size() - 1]);


	// setting up wall
	GLdouble wall_horizontal_offset = 5;
	GLdouble wall_vertical_offset = 5;
	for (int i = 0; i < 10; ++i) {
		for (int j = -5; j < 5; ++j) {

			addBox(5, 5, 5, j * wall_horizontal_offset + 2.5, i * wall_vertical_offset + 2.5, 0, 0.5);

			if (i % 2 == 1) {
				wall_horizontal_offset += 2.5;
			}
			else {
				wall_horizontal_offset -= 2.5;
			}
		}
	}

	addSphere(1.0, 0, 20, 0, 1.0);

}

//----------------------------------------------------------------------------
// Callback method called when system is idle.
void Window::idleCallback() {
	displayCallback();         // call display routine to show the cube

	frames++;
	time = glutGet(GLUT_ELAPSED_TIME);
	if ((time - baseTime) > 1000.0) {
		fps = frames * 1000.0 / (time - baseTime);
		baseTime = time;
		frames = 0;
	}
}

//----------------------------------------------------------------------------
// Callback method called by GLUT when graphics window is resized by the user
void Window::reshapeCallback(int w, int h) {
	cerr << "Window::reshapeCallback called" << endl;
	width = w;
	height = h;
	glViewport(0, 0, w, h);  // set new viewport size
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, GLdouble(width) / (GLdouble) height, 1.0, 1000.0); // set perspective projection viewing frustum
	//glTranslatef(0, 0, -50);    // move camera back 20 units so that it looks at the origin (or else it's in the origin)
	glMatrixMode(GL_MODELVIEW);
}

//----------------------------------------------------------------------------
// Callback method called by GLUT when window readraw is necessary or  when glutPostRedisplay() was called.
void Window::displayCallback() {
	for (int i = 0; i<bodies.size(); i++)
		bodies[i]->hit = false;
	world->stepSimulation(1 / 60.0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // clear color and depth buffers
	glMatrixMode(GL_MODELVIEW);  // make sure we're in Modelview mode
	glLoadIdentity();
	glLoadMatrixd(camera.getMatrix().getPointer());

	//root->draw(Matrix4d());
	for (int i = 0; i<bodies.size(); i++)
	{
		if (bodies[i]->body->getCollisionShape()->getShapeType() == STATIC_PLANE_PROXYTYPE)
			renderPlane(bodies[i]);
		else if (bodies[i]->body->getCollisionShape()->getShapeType() == SPHERE_SHAPE_PROXYTYPE)
			renderSphere(bodies[i]);
		else if (bodies[i]->body->getCollisionShape()->getShapeType() == BOX_SHAPE_PROXYTYPE)
			renderBox(bodies[i]);
	}

	
	cerr << "FPS: " << fps << endl;
	glFlush();
	glutSwapBuffers();
}

// communicate with keyboard
void Window::keyBoardCallBack(unsigned char key, int x, int y) {
	Matrix4d rotationUpdate;
	double angleUpdate = 1.5;
	
	switch (key) {
		// up
		case 'w':
			rotationUpdate.makeRotateX(angleUpdate);
			rotation->setMatrix(rotationUpdate * rotation->getMatrix());
			break;
		// down
		case 'a':
			rotationUpdate.makeRotateY(angleUpdate);
			rotation->setMatrix(rotationUpdate * rotation->getMatrix());
			break;
		// left
		case 's':
			rotationUpdate.makeRotateX(-angleUpdate);
			rotation->setMatrix(rotationUpdate * rotation->getMatrix());
			break;
		// right
		case 'd':
			rotationUpdate.makeRotateY(-angleUpdate);
			rotation->setMatrix(rotationUpdate * rotation->getMatrix());
			break;

		case ' ':
		{
			btRigidBody* sphere = addSphere(1.0, 0, 20, 0, 1.0);
			//Vector3d look = lookat;
			//look.scale(20);
			//sphere->setLinearVelocity(btVector3(look[0], look[1], look[2]));
		}
			
			break;
		default:
			break;
	}
}

void Window::SpecialKeysCallBack(int key, int x, int y) {
}

void Window::MouseClickCallBack(int button, int state, int x, int y) {
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
		movement = trackball::MOVEMENT::SCALING;
		prevX = x;
		prevY = y;
		lastPoint = trackBallMapping(x, y);
	}
	else
		movement = trackball::MOVEMENT::NONE;


	/*if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		movement = trackball::MOVEMENT::ROTATION;
		prevX = x;
		prevY = y;
		lastPoint = trackBallMapping(x, y);
		}
		else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
		movement = trackball::MOVEMENT::SCALING;
		prevX = x;
		prevY = y;
		lastPoint = trackBallMapping(x, y);
		}
		else
		movement = trackball::MOVEMENT::NONE;*/
}

void Window::MouseMotionCallBack(int x, int y) {
	Vector3d direction;
	double pixel_diff;
	double rot_angle, zoom_factor;
	Vector3d currentPoint;
	currentPoint = trackBallMapping(x, y);

	switch (movement) {
		case trackball::MOVEMENT::SCALING:
		{
			pixel_diff = currentPoint.get(1) - lastPoint.get(1);
			zoom_factor = 1.0 + pixel_diff * ZOOM_SCALAR;

			Matrix4d scailingUpdate;
			scailingUpdate.makeScale(zoom_factor, zoom_factor, zoom_factor);
			scaling->setMatrix(scailingUpdate * scaling->getMatrix());
			displayCallback();
		}
			break;
		default:
			break;
	}

	lastPoint = currentPoint;
	
	/*Vector3d direction;
	double pixel_diff;
	double rot_angle, zoom_factor;
	Vector3d currentPoint;
	currentPoint = trackBallMapping(x, y);

	switch (movement) {
	case trackball::MOVEMENT::ROTATION:
	{
	direction = currentPoint - lastPoint;
	double velocity = direction.magnitude();
	if (velocity > 0.0001) {
	Vector3d rotAxis = lastPoint.cross(currentPoint);
	rotAxis.normalize();
	rot_angle = velocity * ROT_SCALAR;
	Matrix4d rotationUpdate;
	rotationUpdate.makeRotate(rot_angle, rotAxis);
	rotation->setMatrix(rotationUpdate * rotation->getMatrix());
	}
	}
	break;
	case trackball::MOVEMENT::SCALING:
	{
	pixel_diff = currentPoint.get(1) - lastPoint.get(1);
	zoom_factor = 1.0 + pixel_diff * ZOOM_SCALAR;

	Matrix4d scailingUpdate;
	scailingUpdate.makeScale(zoom_factor, zoom_factor, zoom_factor);
	scaling->setMatrix(scailingUpdate * scaling->getMatrix());
	displayCallback();
	}
	break;
	default:
	break;
	}

	lastPoint = currentPoint;
	*/
}

Vector3d Window::trackBallMapping(int x, int y) {
	Vector3d v;
	GLdouble d;

	v.set(0, (2.0 * x - width) / width);
	v.set(1, (height - 2.0 * y) / height);
	v.set(2, 0.0);
	d = v.magnitude();
	d = (d < 1.0) ? d : 1.0;
	v.set(2, sqrtf(1.001 - d * d));
	v.normalize(); // Still need to normalize, since we only capped d, not v.

	return v;
}