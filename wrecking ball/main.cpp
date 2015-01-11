#include "main.h"

int main(int argc, char *argv []) {
	float specular [] = { 1.0, 1.0, 1.0, 1.0 };
	float shininess [] = { 20.0 };
	float position [] = { 15.0, 20.0, 1.0, 0.0 };	// lightsource position

	glutInit(&argc, argv);      	      	      // initialize GLUT
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);   // open an OpenGL context with double buffering, RGB colors, and depth buffering
	glutInitWindowSize(Window::width, Window::height);      // set initial window size
	glutCreateWindow("OpenGL Cube");    	      // open window and set window title

	glEnable(GL_TEXTURE_2D);   // enable texture mapping
	glShadeModel(GL_SMOOTH);   // enable smooth shading
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);  // black background
	glClearDepth(1.0f);        // depth buffer setup
	glEnable(GL_DEPTH_TEST);   // enables depth testing
	glDepthFunc(GL_LEQUAL);    // configure depth testing
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);          // really nice perspective calculations

	// Enable Culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glShadeModel(GL_SMOOTH);             	      // set shading to smooth
	glMatrixMode(GL_PROJECTION);

	// Generate material properties:
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

	glEnable(GL_COLOR_MATERIAL);

	// Generate light source:
	//glLightfv(GL_LIGHT0, GL_POSITION, position);
	//glEnable(GL_LIGHTING);
	//glEnable(GL_LIGHT0);

	// Install callback functions:
	glutDisplayFunc(Window::displayCallback);
	glutReshapeFunc(Window::reshapeCallback);
	glutIdleFunc(Window::idleCallback);
	glutKeyboardFunc(Window::keyBoardCallBack);
	glutSpecialFunc(Window::SpecialKeysCallBack);
	glutMouseFunc(Window::MouseClickCallBack);
	glutMotionFunc(Window::MouseMotionCallBack);
	glutPassiveMotionFunc(Window::MousePassiveMotionCallBack);

	// Initialize robot matrix:
	Window::init();
	glutMainLoop();
	return 0;
}

