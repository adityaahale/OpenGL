#include <GL/freeglut.h>

//global variable declaration
bool bFullscreen = false; //variable to toggle for fullscreen
int year = 0;
int day = 0;

int main(int argc, char** argv)
{
	//function prototypes
	void display(void);
	void resize(int, int);
	void keyboard(unsigned char, int, int);
	void mouse(int, int, int, int);
	//void spin(void);
	void initialize(void);
	void uninitialize(void);

	//code
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE |GLUT_DEPTH |GLUT_RGBA);

	glutInitWindowSize(800, 600); //to declare initial window size
	glutInitWindowPosition(300, 50); //to declare initial window position
	glutCreateWindow("OpenGL: REVOLUTION"); //open the window with "OpenGL First Window : Hello World" in the title bar

	initialize();

	glutDisplayFunc(display);
	glutReshapeFunc(resize);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	//glutIdleFunc(spin);
	glutCloseFunc(uninitialize);

	glutMainLoop();

	//	return(0); 
}

void display(void)
{

	//code

	//to clear all pixels
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Pyramid
	glLoadIdentity();
	gluLookAt(0.0f, 0.0f, 5.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	glPushMatrix();
	glutWireSphere(1.0f, 20.0f, 16.0f);
	glRotatef((GLfloat)year, 0.0f, 1.0f, 0.0f);
	glTranslatef(2.0f, 0.0f, 0.0f);
	glRotatef((GLfloat)day, 0.0f, 1.0f, 0.0f);
	glutWireSphere(0.2f, 10.0f, 8.0f);
	glPopMatrix();

	//to process buffered OpenGL Routines
	//glFlush();
	glutSwapBuffers();
}

void initialize(void)
{
	//code
	//to select clearing (background) clear
	glShadeModel(GL_FLAT);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f); //black 
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
}

void keyboard(unsigned char key, int x, int y)
{
	//code
	switch (key)
	{
	case 27: // Escape
		glutLeaveMainLoop();
		break;
	case 'Y':
		year = (year + 5) % 360;
		glutPostRedisplay();
		break;
	case 'y':
		year = (year - 5) % 360;
		glutPostRedisplay();
		break;
	case 'D':
		day = (day + 10) % 360;
		glutPostRedisplay();
		break;
	case 'd':
		day = (day - 10) % 360;
		glutPostRedisplay();
		break;
	case 'F':
	case 'f':
		if (bFullscreen == false)
		{
			glutFullScreen();
			bFullscreen = true;
		}
		else
		{
			glutLeaveFullScreen();
			bFullscreen = false;
		}
		break;
	default:
		break;
	}
}

void mouse(int button, int state, int x, int y)
{
	//code
	switch (button)
	{
	case GLUT_LEFT_BUTTON:
		break;
	default:
		break;
	}
}

void resize(int width, int height)
{
	// code
	if (height == 0)
		height = 1;
	if (width == 0)
		width = 1;
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	/*if (width < height)
	glOrtho(-100.0f, 100.0f, (-100.0f * (height/width)),(100.0f*(height/width)),-100.0f,100.0f);
	else
	glOrtho((-100.0f * (width/height)), (100.0f*(width/height)), -100.0f, 100.0f, -100.0f, 100.0f);
	*/
	gluPerspective(60.0f, (GLfloat)width / (GLfloat)height, 1.0f, 20.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void uninitialize(void)
{
	// code
}

