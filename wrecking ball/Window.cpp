#define _CRT_SECURE_NO_WARNINGS

#include "Window.h"
#include <vector>

// Debugger
bool Window::debugOn = false;

// Window
int Window::width = 512;   // set window width in pixels here
int Window::height = 512;   // set window height in pixels here

// Track Ball
int Window::prevX = width / 2;
int Window::prevY = height / 2;
int Window::movement = trackball::MOVEMENT::NONE;
Vector3d Window::lastPoint;
/*MatrixTransform* Window::root;
MatrixTransform* Window::rotation;
MatrixTransform* Window::scaling;*/

// Frame Calculator 
bool Window::fpsOn = true;
int Window::time = 0;
int Window::baseTime = 0;
int Window::frames = 0;
GLdouble Window::fps = 0;

// Camera(center, look at, up)
Vector3d Window::eye(0.0, 50.0, 200.0);
Vector3d Window::lookat(0.0, 0.0, 0.0);
Vector3d Window::up(0.0, 1.0, 0.0);
Camera Window::camera(eye, lookat, up);
double Window::motion_displacement = 1.0;

// physics world parameters
btSoftRigidDynamicsWorld* Window::world;
btDispatcher* Window::dispatcher;
btCollisionConfiguration* Window::collisionConfig;
btBroadphaseInterface* Window::broadphase;
btConstraintSolver* Window::solver;
btDefaultSoftBodySolver* Window::softbodySolver;
vector<bulletObject*> Window::bodies;
bulletObject* Window::m_pickedBody;
btVector3 Window::m_pickPos;
btScalar Window::m_pickDist;
btGeneric6DofConstraint* Window::m_pickConstraint;

// wall parameter
double Window::brick_height = 10.0;
double Window::brick_width = 10.0;
double Window::brick_depth = 10.0;
int Window::wall_height = 10;
int Window::wall_width = 10;
int Window::wall_thickness = 1;

// ray tracing
double Window::scaler = 10000.0;
int Window::rayTraceX = Window::width / 2;
int Window::rayTraceY = Window::height / 2;


btRigidBody* Window::addSphere(float rad, float x, float y, float z, float mass, float r, float g, float b)
{
	btTransform t;
	t.setIdentity();
	t.setOrigin(btVector3(x, y, z));
	btCollisionShape* sphere = new btSphereShape(rad);
	btVector3 inertia(0, 0, 0);
	if (mass != 0.0)
		sphere->calculateLocalInertia(mass, inertia);

	btMotionState* motion = new btDefaultMotionState(t);
	btRigidBody::btRigidBodyConstructionInfo info(mass, motion, sphere, inertia);
	info.m_restitution = 0.1f;
	info.m_friction = 1.5f;
	btRigidBody* body = new btRigidBody(info);
	world->addRigidBody(body);
	bodies.push_back(new bulletObject(body, 0, r, g, b));
	body->setUserPointer(bodies[bodies.size() - 1]);
	return body;
}

void Window::renderSphere(bulletObject *bobj)
{
	btRigidBody* sphere = bobj->body;
	if (sphere->getCollisionShape()->getShapeType() != SPHERE_SHAPE_PROXYTYPE)
		return;
	if (bobj->selected)
		glColor3f(0, 0, 1);
	else if (!bobj->hit)
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
	glutSolidSphere(r, 20, 20);
	glPopMatrix();
}

btRigidBody* Window::addBox(float width, float height, float depth, float x, float y, float z, float mass, float r, float g, float b)
{
	btTransform t;
	t.setIdentity();
	t.setOrigin(btVector3(x, y, z));
	btCollisionShape* box = new btBoxShape(btVector3(width / 2.0, height / 2.0, depth / 2.0));
	box->setMargin(0.001f);
	btVector3 inertia(0, 0, 0);
	if (mass != 0.0)
		box->calculateLocalInertia(mass, inertia);

	btMotionState* motion = new btDefaultMotionState(t);
	btRigidBody::btRigidBodyConstructionInfo info(mass, motion, box, inertia);
	btRigidBody* body = new btRigidBody(info);
	world->addRigidBody(body);
	bodies.push_back(new bulletObject(body, 3, r, g, b));
	body->setUserPointer(bodies[bodies.size() - 1]);
	return body;
}

void Window::renderBox(bulletObject* bobj)
{
	btRigidBody* box = bobj->body;
	if (box->getCollisionShape()->getShapeType() != BOX_SHAPE_PROXYTYPE)
		return;
	if (bobj->selected)
		glColor3f(0, 0, 1);
	else if (!bobj->hit)
		glColor3f(bobj->r, bobj->g, bobj->b);
	else
		glColor3f(1, 0, 0);
	btVector3 extent = ((btBoxShape*) box->getCollisionShape())->getHalfExtentsWithoutMargin();
	btTransform t;
	box->getMotionState()->getWorldTransform(t);
	float mat[16];
	t.getOpenGLMatrix(mat);
	glPushMatrix();
	glMultMatrixf(mat);     //translation,rotation

	
	glBegin(GL_QUADS);
	glNormal3f(-1.0, 0.0, 0.0);
	glVertex3f(-extent.x(), extent.y(), -extent.z());
	glVertex3f(-extent.x(), -extent.y(), -extent.z());
	glVertex3f(-extent.x(), -extent.y(), extent.z());
	glVertex3f(-extent.x(), extent.y(), extent.z());
	glNormal3f(1.0, 0.0, 0.0);
	glVertex3f(extent.x(), extent.y(), extent.z());
	glVertex3f(extent.x(), -extent.y(), extent.z());
	glVertex3f(extent.x(), -extent.y(), -extent.z());
	glVertex3f(extent.x(), extent.y(), -extent.z());
	glNormal3f(0.0, 0.0, 1.0);
	glVertex3f(-extent.x(), extent.y(), extent.z());
	glVertex3f(-extent.x(), -extent.y(), extent.z());
	glVertex3f(extent.x(), -extent.y(), extent.z());
	glVertex3f(extent.x(), extent.y(), extent.z());
	glNormal3f(0.0, 0.0, -1.0);
	glVertex3f(extent.x(), extent.y(), -extent.z());
	glVertex3f(extent.x(), -extent.y(), -extent.z());
	glVertex3f(-extent.x(), -extent.y(), -extent.z());
	glVertex3f(-extent.x(), extent.y(), -extent.z());
	glNormal3f(0.0, 1.0, 0.0);
	glVertex3f(-extent.x(), extent.y(), -extent.z());
	glVertex3f(-extent.x(), extent.y(), extent.z());
	glVertex3f(extent.x(), extent.y(), extent.z());
	glVertex3f(extent.x(), extent.y(), -extent.z());
	glNormal3f(0.0, -1.0, 0.0);
	glVertex3f(-extent.x(), -extent.y(), -extent.z());
	glVertex3f(extent.x(), -extent.y(), -extent.z());
	glVertex3f(extent.x(), -extent.y(), extent.z());
	glVertex3f(-extent.x(), -extent.y(), extent.z());
	glEnd();
	

	glPopMatrix();
}

void Window::renderPlane(bulletObject* bobj)
{
	btRigidBody* plane = bobj->body;
	if (plane->getCollisionShape()->getShapeType() != STATIC_PLANE_PROXYTYPE)
		return;
	glColor3f(bobj->r, bobj->g, bobj->b);
	btTransform t;
	plane->getMotionState()->getWorldTransform(t);
	float mat[16];
	t.getOpenGLMatrix(mat);
	glPushMatrix();
	glMultMatrixf(mat);     //translation,rotation
	glBegin(GL_QUADS);
	glNormal3f(0.0, 1.0, 0.0);
	glVertex3f(-1000, 0, 1000);
	glVertex3f(1000, 0, 1000);
	glVertex3f(1000, 0, -1000);
	glVertex3f(-1000, 0, -1000);
	glEnd();
	glPopMatrix();
}

void Window::renderSoftbody(btSoftBody* b)
{
	//faces
	glColor3f(1.0, 0.0, 1.0);
	glBegin(GL_TRIANGLES);
	for (int i = 0; i<b->m_faces.size(); i++)
	{
		for (int j = 0; j<3; j++)
			glVertex3f(b->m_faces[i].m_n[j]->m_x.x(),
			b->m_faces[i].m_n[j]->m_x.y(),
			b->m_faces[i].m_n[j]->m_x.z());

	}
	glEnd();
	//lines
	glColor3f(0.0, 0.0, 1.0);
	glBegin(GL_LINES);
	for (int i = 0; i<b->m_links.size(); i++)
	{
		for (int j = 0; j<2; j++)
			glVertex3f(b->m_links[i].m_n[j]->m_x.x(),
			b->m_links[i].m_n[j]->m_x.y(),
			b->m_links[i].m_n[j]->m_x.z());
	}
	glEnd();
}

void Window::init() {
	/*root = new MatrixTransform(Matrix4d());
	rotation = new MatrixTransform(Matrix4d());
	scaling = new MatrixTransform(Matrix4d());

	root->addChild(scaling);
	scaling->addChild(rotation);
	*/

	// Setting up physics world
	// Build the broadphase
	broadphase = new btDbvtBroadphase();
	
	// Set up the collision configuration and dispatcher
	collisionConfig = new btSoftBodyRigidBodyCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collisionConfig);
	
	// The actual physics solver
	solver = new btSequentialImpulseConstraintSolver();
	softbodySolver = new btDefaultSoftBodySolver();
	
	// The world
	world = new btSoftRigidDynamicsWorld(dispatcher, broadphase, solver, collisionConfig, softbodySolver);
	world->setGravity(btVector3(0, -10, 0));


	// Setting up ground
	btTransform t;
	t.setIdentity();
	t.setOrigin(btVector3(0, 0, 0));
	btCollisionShape* plane = new btStaticPlaneShape(btVector3(0, 1, 0), 1);
	btDefaultMotionState* motion = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, -1, 0)));
	btRigidBody::btRigidBodyConstructionInfo info(0.0, motion, plane, btVector3(0, 0, 0));
	//info.m_linearDamping = 0.2f;
	info.m_restitution = 1.0f;
	info.m_friction = 0.5f;
	btRigidBody* body = new btRigidBody(info);
	world->addRigidBody(body);
	bodies.push_back(new bulletObject(body, 4, 0.8, 0.8, 0.8));
	body->setUserPointer(bodies[bodies.size() - 1]);  


	// setting up wall
	double x_start = (double)wall_width / 2.0;
	x_start = -x_start * brick_width;
	double y_start = brick_height / 2.0;
	double z_start = 0.0;
	double mass = 10.0;

	for (int i = 0; i < wall_thickness; i++){
		for (int j = 0; j < wall_height; j++){
			for (int k = 0; k < wall_width; k++){
				if ((k % 2 == 0 && j % 2 == 1) || (k % 2 == 1 && j % 2 == 0))
					addBox(brick_width, brick_height, brick_depth, x_start, y_start, z_start, mass, 0.1, 0.1, 0.1);
				else
					addBox(brick_width, brick_height, brick_depth, x_start, y_start, z_start, mass, 0.7, 0.7, 0.7);
				x_start += brick_width;
			}
			x_start = -(double)wall_width / 2.0 * brick_width;
			y_start += brick_height;
		}
		y_start = brick_height / 2.0;
		z_start += brick_depth / 2.0;

	}


	btTransform t1;
	t1.setIdentity();
	t1.setOrigin(btVector3(0, 25, 10));
	btSphereShape* sphere = new btSphereShape(10);
	btVector3 inertia(0, 0, 0);
	sphere->calculateLocalInertia(30, inertia);
	btMotionState* motion1 = new btDefaultMotionState(t1);
	btRigidBody::btRigidBodyConstructionInfo info1(30, motion1, sphere, inertia);
	btRigidBody* body1 = new btRigidBody(info1);
	world->addRigidBody(body1);
	bodies.push_back(new bulletObject(body1, 0, 60.0 / 256.0, 20.0 / 256.0, 134.0 / 256.0));
	body1->setUserPointer(bodies[bodies.size() - 1]);

	btSoftBody* softBody = btSoftBodyHelpers::CreateRope(world->getWorldInfo(),
		btVector3(0, 150, 10),
		btVector3(0, 25, 10),
		50,
		1);
	softBody->m_cfg.viterations = 50;
	softBody->m_cfg.piterations = 50;
	softBody->setTotalMass(0.5);
	//softBody->getCollisionShape()->setMargin(0.01);
	softBody->appendAnchor(softBody->m_nodes.size() - 1, body1);
	world->addSoftBody(softBody);

	/*
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
	*/
}

//----------------------------------------------------------------------------
// Callback method called when system is idle.
void Window::idleCallback() {
	displayCallback();         // call display routine to show the cube

	if (fpsOn) {
		frames++;
		time = glutGet(GLUT_ELAPSED_TIME);
		if ((time - baseTime) > 1000.0) {
			fps = frames * 1000.0 / (time - baseTime);
			baseTime = time;
			frames = 0;
		}
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
	world->stepSimulation(1 / 60.0);

	/*
	// raytracing test
	if (movement == trackball::MOVEMENT::SELECTION) {
		//lastPoint = rayTracingComputePoint(rayTraceX, rayTraceY);
		Vector3d direction = (lastPoint - camera.getEye()) * scaler;;

		btCollisionWorld::ClosestRayResultCallback rayCallback(btVector3(camera.getEye()[0], camera.getEye()[1], camera.getEye()[2]), btVector3(direction[0], direction[1], direction[2]));
		world->rayTest(btVector3(camera.getEye()[0], camera.getEye()[1], camera.getEye()[2]), btVector3(direction[0], direction[1], direction[2]), rayCallback);
		if (rayCallback.hasHit()) {
			((bulletObject*)rayCallback.m_collisionObject->getUserPointer())->selected = true;

		}
	}
	else {
		//lastPoint = rayTracingComputePoint(rayTraceX, rayTraceY);
		Vector3d direction = (lastPoint - camera.getEye()) * scaler;;

		btCollisionWorld::ClosestRayResultCallback rayCallback(btVector3(camera.getEye()[0], camera.getEye()[1], camera.getEye()[2]), btVector3(direction[0], direction[1], direction[2]));
		world->rayTest(btVector3(camera.getEye()[0], camera.getEye()[1], camera.getEye()[2]), btVector3(direction[0], direction[1], direction[2]), rayCallback);
		if (rayCallback.hasHit()) {
			((bulletObject*)rayCallback.m_collisionObject->getUserPointer())->hit = true;
		}
	}
	*/
	/*double scaler = 1000.0;
	Vector3d direction = (camera.getLookAt() - camera.getEye()) * scaler;
	btCollisionWorld::ClosestRayResultCallback rayCallback(btVector3(camera.getEye()[0], camera.getEye()[1], camera.getEye()[2]), btVector3(direction[0], direction[1], direction[2]));
	world->rayTest(btVector3(camera.getEye()[0], camera.getEye()[1], camera.getEye()[2]), btVector3(direction[0], direction[1], direction[2]), rayCallback);
	if (rayCallback.hasHit()) {
		((bulletObject*) (rayCallback.m_collisionObject->getUserPointer()))->hit = true;
	}

	direction.print("direction: ");*/

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // clear color and depth buffers
	glMatrixMode(GL_MODELVIEW);  // make sure we're in Modelview mode
	glLoadIdentity();
	glLoadMatrixd(camera.getMatrix().getPointer());

	//root->draw(Matrix4d());
	for (int i = 0; i<bodies.size(); i++)
	{
		if (bodies[i]->body->getCollisionShape()->getShapeType() == STATIC_PLANE_PROXYTYPE){
			renderPlane(bodies[i]);
			//cerr << "rendering plane" << endl;
		}
		else if (bodies[i]->body->getCollisionShape()->getShapeType() == SPHERE_SHAPE_PROXYTYPE)
			renderSphere(bodies[i]);
		else if (bodies[i]->body->getCollisionShape()->getShapeType() == BOX_SHAPE_PROXYTYPE)
			renderBox(bodies[i]);
	}
	for (int i = 0; i<world->getSoftBodyArray().size(); i++)
		renderSoftbody(world->getSoftBodyArray()[i]);
	
	for (int i = 0; i < bodies.size(); i++) {
		bodies[i]->hit = false;
	}
	
	if (fpsOn) {
		cerr << "FPS: " << fps << endl;
	}
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
			camera.moveEye(UPWARD, motion_displacement);
			break;
		// down
		case 's':
			camera.moveEye(DOWNWARD, motion_displacement);
			break;
		// left
		case 'a':
			camera.moveEye(LEFTWARD, motion_displacement);
			break;
		// right
		case 'd':
			camera.moveEye(RIGHTWARD, motion_displacement);
			break;
		// reset
		case ' ':
			delete Window::world;
			delete Window::solver;
			delete Window::softbodySolver;
			delete Window::dispatcher;
			delete Window::collisionConfig;
			delete Window::broadphase;

			while (!bodies.empty()) {
				delete bodies.at(bodies.size() - 1);
				bodies.pop_back();
			}

			init();

			break;
		// shooting spheres
		case 'x':
		{
			Vector3d e = camera.getEye();
			Vector3d d = camera.getLookAt();
			btRigidBody* sphere = addSphere(5.0, e[0], e[1], e[2], 5.0, 32.0 / 256.0, 64.0 / 256.0, 20.0 / 256.0);
			Vector3d look = d - e;
			look.scale(1.1);
			sphere->setLinearVelocity(btVector3(look[0], look[1], look[2]));
		}
			
			break;
		case 27:
			exit(1);
		default:
			break;
	}
}

void Window::SpecialKeysCallBack(int key, int x, int y) {
	switch (key){
	case GLUT_KEY_UP:
		camera.move(UPWARD, motion_displacement);
		break;
	case GLUT_KEY_DOWN:
		camera.move(DOWNWARD, motion_displacement);
		break;
	case GLUT_KEY_LEFT:
		camera.move(LEFTWARD, motion_displacement);
		break;
	case GLUT_KEY_RIGHT:
		camera.move(RIGHTWARD, motion_displacement);
		break;
	}
}

void Window::MouseClickCallBack(int button, int state, int x, int y) {
	if (debugOn) {
		cerr << x << ", " << y << endl;
	}

	// zoom in/out
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
		movement = trackball::MOVEMENT::SCALING;
		prevX = x;
		prevY = y;
		lastPoint = trackBallMapping(x, y);
	}
	// object selection
	else if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		movement = trackball::MOVEMENT::SELECTION;
		rayTraceX = x;
		rayTraceY = y;
		lastPoint = rayTracingComputePoint(rayTraceX, rayTraceY);
		btVector3 btRayFrom(camera.getEye()[0], camera.getEye()[1], camera.getEye()[2]);
		Vector3d direction = (lastPoint - camera.getEye()) * scaler;
		btCollisionWorld::ClosestRayResultCallback rayCallback(btVector3(camera.getEye()[0], camera.getEye()[1], camera.getEye()[2]), btVector3(direction[0], direction[1], direction[2]));
		world->rayTest(btVector3(camera.getEye()[0], camera.getEye()[1], camera.getEye()[2]), btVector3(direction[0], direction[1], direction[2]), rayCallback);
		if (rayCallback.hasHit()){
			bulletObject* pPhysicsData = reinterpret_cast<bulletObject*>(rayCallback.m_collisionObject->getUserPointer());
			if (pPhysicsData)
			{
				btRigidBody * pBody = pPhysicsData->body;
				// Code for adding a constraint from Bullet Demo's DemoApplication.cpp
				if (!(pBody->isStaticObject() || pBody->isKinematicObject()))
				{
					m_pickedBody = pPhysicsData;
					m_pickedBody->selected = true;
					m_pickPos = rayCallback.m_hitPointWorld;
					pBody->setActivationState(DISABLE_DEACTIVATION);

					btVector3 localPivot = pBody->getCenterOfMassTransform().inverse() * m_pickPos;

					btTransform tr;
					tr.setIdentity();
					tr.setOrigin(localPivot);
					btGeneric6DofConstraint* dof6 = new btGeneric6DofConstraint(*pBody, tr, false);
					dof6->setLinearLowerLimit(btVector3(0, 0, 0));
					dof6->setLinearUpperLimit(btVector3(0, 0, 0));
					dof6->setAngularLowerLimit(btVector3(0, 0, 0));
					dof6->setAngularUpperLimit(btVector3(0, 0, 0));

					world->addConstraint(dof6);
					m_pickConstraint = dof6;

					dof6->setParam(BT_CONSTRAINT_STOP_CFM, 0.8f, 0);
					dof6->setParam(BT_CONSTRAINT_STOP_CFM, 0.8f, 1);
					dof6->setParam(BT_CONSTRAINT_STOP_CFM, 0.8f, 2);
					dof6->setParam(BT_CONSTRAINT_STOP_CFM, 0.8f, 3);
					dof6->setParam(BT_CONSTRAINT_STOP_CFM, 0.8f, 4);
					dof6->setParam(BT_CONSTRAINT_STOP_CFM, 0.8f, 5);

					dof6->setParam(BT_CONSTRAINT_STOP_ERP, 0.1f, 0);
					dof6->setParam(BT_CONSTRAINT_STOP_ERP, 0.1f, 1);
					dof6->setParam(BT_CONSTRAINT_STOP_ERP, 0.1f, 2);
					dof6->setParam(BT_CONSTRAINT_STOP_ERP, 0.1f, 3);
					dof6->setParam(BT_CONSTRAINT_STOP_ERP, 0.1f, 4);
					dof6->setParam(BT_CONSTRAINT_STOP_ERP, 0.1f, 5);

					//save mouse position for dragging
					m_pickDist = (m_pickPos - btRayFrom).length();
				}
			}
		}
	}
	// reset
	else {
		movement = trackball::MOVEMENT::NONE;
		if (m_pickConstraint != NULL){
			world->removeConstraint(m_pickConstraint);
			delete m_pickConstraint;
			m_pickConstraint = NULL;
			m_pickedBody->body->forceActivationState(ACTIVE_TAG);
			m_pickedBody->body->setDeactivationTime(0.f);
			m_pickedBody->selected = false;
			m_pickedBody = NULL;
		}
		
	}

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
			// zoom_factor = 1.0 + pixel_diff * ZOOM_SCALAR;
			zoom_factor = pixel_diff * ZOOM_SCALAR;
			cerr << pixel_diff << endl;
			if (zoom_factor > 0)
				camera.move(FORWARD, zoom_factor);
			else
				camera.move(BACKWARD, -zoom_factor);
			/*Matrix4d scailingUpdate;
			scailingUpdate.makeScale(zoom_factor, zoom_factor, zoom_factor);
			scaling->setMatrix(scailingUpdate * scaling->getMatrix());
			*/
			lastPoint = currentPoint;
		}
			break;
		case trackball::MOVEMENT::SELECTION:
		{
			rayTraceX = x;
			rayTraceY = y;
			
			Vector3d lastPoint= rayTracingComputePoint(rayTraceX, rayTraceY);

			btGeneric6DofConstraint* pickCon = static_cast<btGeneric6DofConstraint*>(m_pickConstraint);
			if (pickCon)
			{
				//keep it at the same picking distance

				btVector3 btRayTo = btVector3(lastPoint[0], lastPoint[1], lastPoint[2]);
				btVector3 btRayFrom = btVector3(camera.getEye()[0], camera.getEye()[1], camera.getEye()[2]);
				btVector3 oldPivotInB = pickCon->getFrameOffsetA().getOrigin();

				btVector3 newPivotB;

				btVector3 dir = btRayTo - btRayFrom;
				dir.normalize();
				dir *= m_pickDist;

				newPivotB = btRayFrom + dir;

				pickCon->getFrameOffsetA().setOrigin(newPivotB);
			}
			
		}
			break;
		case trackball::MOVEMENT::NONE:
			break;
		default:
			break;
	}


	
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

void Window::MousePassiveMotionCallBack(int x, int y) {
	rayTraceX = x;
	rayTraceY = y;
	lastPoint = rayTracingComputePoint(rayTraceX, rayTraceY);
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

Vector3d Window::rayTracingComputePoint(int x, int y) {
	double mvmatrix[16];
	double projmatrix[16];
	int viewport[4];
	double dX, dY, dZ;
	Vector3d mousePosition, direction;
	glGetIntegerv(GL_VIEWPORT, viewport);
	glGetDoublev(GL_MODELVIEW_MATRIX, mvmatrix);
	glGetDoublev(GL_PROJECTION_MATRIX, projmatrix);

	gluUnProject((double) x, (double) (viewport[3] - (double) y), 0.0, mvmatrix, projmatrix, viewport, &dX, &dY, &dZ);
	mousePosition = Vector3d(dX, dY, dZ);

	if (debugOn) {
		cerr << "x: " << dX << "; y: " << dY << "; z: " << dZ << endl;
		cerr << "cam: " << camera.getEye()[0] << ", " << camera.getEye()[1] << ", " << camera.getEye()[2] << endl;
	}

	return mousePosition;
}