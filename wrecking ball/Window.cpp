#define _CRT_SECURE_NO_WARNINGS

#include "Window.h"

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
Camera Window::camera(Vector3d(0.0, 10.0, 50.0), Vector3d(0.0, 0.0, 0.0), Vector3d(0.0, 1.0, 0.0));

void Window::init() {
	root = new MatrixTransform(Matrix4d());
	rotation = new MatrixTransform(Matrix4d());
	scaling = new MatrixTransform(Matrix4d());

	root->addChild(scaling);
	scaling->addChild(rotation);

	GLdouble wall_horizontal_offset = 5;
	GLdouble wall_vertical_offset = 5;
	
	for (int i = 0; i < 10; ++i) {
		for (int j = -5; j < 5; ++j) {
			Matrix4d temp;
			temp.makeTranslate((GLdouble) j * wall_horizontal_offset + 2.5, (GLdouble) i * wall_vertical_offset + 2.5, 0.0);
			MatrixTransform* transformation = new MatrixTransform(temp);
			rotation->addChild(transformation);
			transformation->addChild(new Cube(5.0, Vector3d(1.0, 1.0, 1.0), draw::mode::SOLID));

			if (i % 2 == 1) {
				wall_horizontal_offset += 2.5;
			}
			else {
				wall_horizontal_offset -= 2.5;
			}
		}
	}
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
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // clear color and depth buffers
	glMatrixMode(GL_MODELVIEW);  // make sure we're in Modelview mode
	glLoadIdentity();
	//glLoadMatrixd(camera.getMatrix().getPointer());

	root->draw(camera.getMatrix());

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