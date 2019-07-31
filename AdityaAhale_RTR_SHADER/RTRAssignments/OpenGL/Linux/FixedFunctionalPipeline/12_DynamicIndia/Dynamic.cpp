#include<iostream>
#include<stdlib.h>
#include<stdio.h>
#include<memory.h>
#include<math.h>

#include<X11/Xlib.h>
#include<X11/Xutil.h>
#include<X11/XKBlib.h>
#include<X11/keysym.h>

#include<GL/gl.h>
#include<GL/glx.h>
#include<GL/glu.h>

using namespace std;

#define PI 3.141592653

int giWindowWidth=800;
int giWindowHeight=600;
Display *gpDisplay=NULL;
bool bFullScreen=false;
XVisualInfo *gpXVisualInfo=NULL;
Colormap gcolormap;
Window gWindow;

//for animation
GLfloat tri=0.0f;
GLfloat rect=0.0f;
const int numPoints = 1000;
GLfloat rot = 0.0f, itrans = -8.0f, ntrans = 6.5f, iitrans = -6.5f, atrans = 8.0;
GLfloat xOrColor = 0.0f, yOrColor = 0.0f, zOrColor = 0.0f, xGrColor = 0.0f, yGrColor = 0.0f, zGrColor = 0.0f;
GLfloat xTritrans = -6.5f, Topangle = 0.0f, Bottomangle = 0.0f, xIAFtrans = -8.3f,
SOrange[3] = { 1.0f, 0.6f, 0.2f },
EOrange[3] = { 1.0f, 0.6f, 0.2f },
SWhite[3] = { 1.0f,1.0f,1.0f },
EWhite[3] = { 1.0f,1.0f,1.0f },
SGreen[3] = { 0.07058f, 0.54f, 0.0274f },
EGreen[3] = { 0.07058f, 0.54f, 0.0274f },
xPlanetrans = -8.3f, xPlane1trans = -8.3f, xPlane2trans = -8.3f, yPlane1trans = 2.0, yPlane2trans = -2.0;
int gbTristable = 0;

float colourArray[100][3] = { 1.0f,0.0f,0.0f,
							0.0f,1.0f,0.0f,
							0.0f,0.0f,1.0f,
							1.0f,1.0f,0.0f,
							0.0f,1.0f,1.0f,
							1.0f,0.0f,1.0f,
							0.5f,0.5f,0.5f,
							1.0f,1.0f,1.0f,
							1.0f,0.5f,0.0f,
							0.25f,0.75f,0.5f };

GLXContext gGLXContext;
	//void translatemy(void);
	void DrawI(void);
	void DrawN(void);
	void DrawD(void);
	void DrawA(void);
	void TriLine(void);
	void PlaneBody(void);
	void PlaneWings(void);
	void smoke(void);
	void SmokeAnimation(void);
	void IAF(void);

int main(void)
{
	void CreateWindow(void);
	void ToggleFullscreen(void);
	void uninitialize(void);
	
	//change for OGL
	void initialize(void);
	void display(void);
	void resize(int,int);
	void update(void);		//change for animation
	

	int WinWidth=giWindowWidth;
	int WinHeight=giWindowHeight;
	bool bDone=false;
	
	//code
	CreateWindow();
	initialize();			//change for OGL
	
	//game loop
	XEvent event;
	KeySym keysym;
	
	
	while(bDone==false)
	{
		while(XPending(gpDisplay))	//game loop
		{
		
			XNextEvent(gpDisplay,&event);	
			switch(event.type)
			{
				case MapNotify:
					break;
				case KeyPress:
					keysym=XkbKeycodeToKeysym(gpDisplay,event.xkey.keycode,0,0);
					switch(keysym)
					{
					case XK_Escape:
						bDone=true;
						uninitialize();
						exit(0);
						break;
					case XK_F:
					case XK_f:
						
						break;
					}
					break;
				case ButtonPress:
					switch(event.xbutton.button)
					{
						case 1:
							break;
						case 2:
							break;
						case 3:
							break;
						default:
							break;
					}
					break;
				case MotionNotify:
					break;
				case ConfigureNotify:
					WinWidth=event.xconfigure.width;
					WinHeight=event.xconfigure.height;
					resize(WinWidth,WinHeight);
					break;
				case Expose:
					break;
				case DestroyNotify:
					break;
				case 33:
					uninitialize();
					exit(0);
					break;
				default:
					break;
			}
		}
		update();
		display();
		
	}
	uninitialize();
	return 0;
}

void CreateWindow(void)
{
	void uninitialize(void);
	
	
	XSetWindowAttributes winAttribs;
	int defaultScreen;
	int defaultDepth;
	int styleMask;
	
	//change for animation
	static int frameBufferAttributes[]=
	{
		GLX_RGBA,GLX_DOUBLEBUFFER,True,GLX_RED_SIZE,8,GLX_GREEN_SIZE,8,GLX_BLUE_SIZE,8,GLX_ALPHA_SIZE,8,GLX_DEPTH_SIZE,24,None
	};
	
	
	gpDisplay=XOpenDisplay(NULL);
	if(gpDisplay==NULL)
	{
		printf("ERROR:Unable o open XDisplay.\nEXITING..");
		uninitialize();
		exit(1);
	}
	defaultScreen=XDefaultScreen(gpDisplay);
	
	gpXVisualInfo=glXChooseVisual(gpDisplay,defaultScreen,frameBufferAttributes);	// change for OGL
	
	winAttribs.border_pixel=0;
	winAttribs.background_pixmap=0;
	winAttribs.colormap=XCreateColormap(gpDisplay,RootWindow(gpDisplay,gpXVisualInfo->screen),gpXVisualInfo->visual,AllocNone);
	gcolormap=winAttribs.colormap;
	winAttribs.background_pixel=BlackPixel(gpDisplay,defaultScreen);
	winAttribs.event_mask=ExposureMask|VisibilityChangeMask|ButtonPressMask|KeyPressMask|PointerMotionMask|StructureNotifyMask;
	styleMask=CWBorderPixel|CWBackPixel|CWEventMask|CWColormap;
	
	gWindow=XCreateWindow(gpDisplay,RootWindow(gpDisplay,gpXVisualInfo->screen),0,0,giWindowWidth,giWindowHeight,0,gpXVisualInfo->depth,InputOutput,gpXVisualInfo->visual,styleMask,&winAttribs);
	
	if(!gWindow)
	{
		printf("ERROR:Failed to create main window..exiting...\n");
		uninitialize();
		exit(1);
	}

	XStoreName(gpDisplay,gWindow,"Dynamic India");
	
	Atom WindowManagerDelete=XInternAtom(gpDisplay,"WM_DELETE_WINDOW",True);
	XSetWMProtocols(gpDisplay,gWindow,&WindowManagerDelete,1);
	XMapWindow(gpDisplay,gWindow);
}

void ToggleFullscreen()
{
	Atom wm_state;
	Atom fullscreen;
	XEvent xev={0};
	
	
	wm_state=XInternAtom(gpDisplay,"_NET_WM_STATE",False);
	memset(&xev,0,sizeof(xev));
	
	xev.type=ClientMessage;
	xev.xclient.window=gWindow;
	xev.xclient.message_type=wm_state;
	xev.xclient.format=32;
	xev.xclient.data.l[0]=bFullScreen?0:1;
	
	fullscreen=XInternAtom(gpDisplay,"_NET_WM_STATE_FULLSCREEN",False);
	xev.xclient.data.l[1]=fullscreen;
	
	XSendEvent(gpDisplay,RootWindow(gpDisplay,gpXVisualInfo->screen),False,StructureNotifyMask,&xev);
}

void initialize()
{

	void resize(int, int);
	void ToggleFullscreen(void);

	gGLXContext=glXCreateContext(gpDisplay,gpXVisualInfo,NULL,GL_TRUE);
	
	glXMakeCurrent(gpDisplay,gWindow,gGLXContext);
	
	glClearColor(0.0f,0.0f,0.0f,0.0f);
	ToggleFullscreen();
	//resize(giWindowWidth,giWindowHeight);


}




void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);	//change for animation
	
	//smoke
	glLoadIdentity();
	glTranslatef(xTritrans, 0.0f, -6.0f);//x=2.0f
	glLineWidth(3.0f);
	smoke();

	//IIIIIIIIIIIIII
	glLoadIdentity();
	glTranslatef(itrans, 0.0f, -6.0f);
	//glTranslatef(-2.5f, 0.0f, -6.0f);
	glLineWidth(4.0f);
	DrawI();

	//AAAAAAAAAAAAAA
	glLoadIdentity();
	glTranslatef(atrans, 0.0f, -6.0f);
	//glTranslatef(2.0f, 0.0f, -6.0f);
	glLineWidth(4.0f);
	DrawA();

	//NNNNNNNNNNNNNNNNNNN
	glLoadIdentity();
	glTranslatef(-1.5f, ntrans, -6.0f);
	//glTranslatef(-1.5f, 0.0f, -6.0f);
	glLineWidth(4.0f);
	DrawN();


	//2nd IIIIIIIIIIIIIIIIIII
	glLoadIdentity();
	glTranslatef(1.0f, iitrans, -6.0f);
	glLineWidth(4.0f);
	//glBegin(GL_LINES);
	DrawI();

	//DDDDDDDDDDDDDDDDDD

	glLoadIdentity();
	glTranslatef(0.2f, 0.0f, -6.0f);
	glLineWidth(4.0f);
	//glBegin(GL_LINES);
	DrawD();


	//TriFlag
	glLoadIdentity();
	glTranslatef(xTritrans, 0.0f, -6.0f);//x=2.0f
	glLineWidth(3.0f);
	TriLine();

	//glEnd();
	//Plane
	glLoadIdentity();
	glTranslatef(xPlanetrans, 0.078f, -8.0f);
	PlaneBody();
	glLoadIdentity();
	glTranslatef(xPlanetrans, 0.078f, -8.0f);
	PlaneWings();
	glLoadIdentity();
	glTranslatef(xIAFtrans, 0.078, -8.0f);
	glLineWidth(1.0f);
	IAF();


	//TopPlane
	glLoadIdentity();
	glTranslatef(xPlane1trans, yPlane1trans, -8.0f);
	glRotatef(Topangle, 1.0f, 1.0f, 1.0f);
	PlaneBody();
	glLoadIdentity();
	glTranslatef(xPlane1trans, yPlane1trans, -8.0f);
	glRotatef(Topangle, 1.0f, 1.0f, 1.0f);
	PlaneWings();
	glLoadIdentity();
	glTranslatef(xPlane1trans, yPlane1trans, -8.0f);
	glRotatef(Topangle, 1.0f, 1.0f, 1.0f);
	glLineWidth(1.0f);
	IAF();

	//BottomPlane
	glLoadIdentity();
	glTranslatef(xPlane2trans, yPlane2trans, -8.0f);
	glRotatef(Bottomangle, 1.0f, 1.0f, 1.0f);
	PlaneBody();
	glLoadIdentity();
	glTranslatef(xPlane2trans, yPlane2trans, -8.0f);
	glRotatef(Bottomangle, 1.0f, 1.0f, 1.0f);
	PlaneWings();
	glLoadIdentity();
	glTranslatef(xPlane2trans, yPlane2trans, -8.0f);
	glRotatef(Bottomangle, 1.0f, 1.0f, 1.0f);
	glLineWidth(1.0f);
	IAF();


	glXSwapBuffers(gpDisplay,gWindow);			//change for animation
}

void update(void)
{
	
	if (itrans <= -2.5)
	{
		//itrans = itrans + 0.08;
		itrans = itrans + 0.005;
		//ytrans = ytrans + 0.001;
	}

	if (itrans >= -2.5)
	{
		if (atrans > 2.0)
		{
			//atrans = atrans - 0.08;
			atrans = atrans - 0.005;
			//yctrans = yctrans + 0.001;

		}


	}
	
	if (atrans < 2.0)
	{
		if (ntrans >= 0.0)
		{
			//ntrans = ntrans - 0.08;
			ntrans = ntrans - 0.005;
		}
	}

	if (ntrans < 0.0)
	{
		if (iitrans <= 0.0)
		{
			//iitrans = iitrans + 0.08;
			iitrans = iitrans + 0.005;
		}
	}

	if (iitrans > 0.0)
	{
		//1.0,0.6,0.2--orange		0.07058,0.54,0.0274--green
		if (xOrColor < 1.0)
		{
			//xOrColor = xOrColor + 0.08;
			xOrColor = xOrColor + 0.002;
		}
		if (yOrColor < 0.6)
		{
			//yOrColor = yOrColor + 0.08;
			yOrColor = yOrColor + 0.002;
		}
		if (zOrColor < 0.2)
		{
			zOrColor = zOrColor + 0.002;
			//zOrColor = zOrColor + 0.08;
		}

		if (xGrColor < 0.07058)
		{
			//xGrColor = xGrColor + 0.08;
			xGrColor = xGrColor + 0.002;
		}

		if (yGrColor < 0.54)
		{
			//yGrColor = yGrColor + 0.08;
			yGrColor = yGrColor + 0.002;
		}

		if (zGrColor < 0.0274)
		{
			//zGrColor = zGrColor + 0.08;
			zGrColor = zGrColor + 0.002;
		}
	}

	if (xOrColor >= 1.0)
	{

		if (xTritrans < 2.0)
		{
			xTritrans = xTritrans + 0.007;

		}
		xPlanetrans = xPlanetrans + 0.009;
		xPlane1trans = xPlane1trans + 0.009;
		xPlane2trans = xPlane2trans + 0.009;
		xIAFtrans = xIAFtrans + 0.009;
		//Topangle = Topangle + 0.008;
		//Bottomangle = Bottomangle - 0.008;

		if (yPlane1trans > 0.078)
		{
			yPlane1trans = yPlane1trans - 0.004;

		}
		if (yPlane2trans < 0.078)
		{
			yPlane2trans = yPlane2trans + 0.004;
		}

		if (xPlanetrans > 3.0)
		{
			xPlane1trans = xPlane1trans + 0.0008;
			yPlane1trans = yPlane1trans - 0.006;//0.007

			Topangle = Topangle - 0.08;//0.08
			if (Topangle < -45.0)
			{
				Topangle = -45.0;
			}
		}

		if (xPlanetrans > 3.0)
		{
			xPlane2trans = xPlane2trans + 0.0008;
			yPlane2trans = yPlane2trans + 0.006;

			Bottomangle = Bottomangle + 0.08;
			if (Bottomangle > 45.0)
			{
				Bottomangle = 45.0;
			}
		}

		//}
		/*if (Bottomangle < 0.0)
		{

		}*/

	}
	SmokeAnimation();

}

void SmokeAnimation(void)
{
	if (xTritrans > 1.5)
	{
		EOrange[0] = EOrange[0] - 0.006;
		EOrange[1] = EOrange[1] - 0.006;
		EOrange[2] = EOrange[2] - 0.006;
		if (EOrange[2] < 0.0)
		{
			SOrange[0] = SOrange[0] - 0.007;
			SOrange[1] = SOrange[1] - 0.007;
			SOrange[2] = SOrange[2] - 0.007;

		}
	}

	if (xTritrans > 1.5)
	{
		EWhite[0] = EWhite[0] - 0.0065;
		EWhite[1] = EWhite[1] - 0.0065;
		EWhite[2] = EWhite[2] - 0.0065;
		if (EWhite[2] < 0.0)
		{
			SWhite[0] = SWhite[0] - 0.007;
			SWhite[1] = SWhite[1] - 0.007;
			SWhite[2] = SWhite[2] - 0.007;

		}
	}

	if (xTritrans > 1.5)
	{
		EGreen[0] = EGreen[0] - 0.006;
		EGreen[1] = EGreen[1] - 0.006;
		EGreen[2] = EGreen[2] - 0.006;
		if (EGreen[2] < 0.0)
		{
			SGreen[0] = SGreen[0] - 0.007;
			SGreen[1] = SGreen[1] - 0.007;
			SGreen[2] = SGreen[2] - 0.007;

		}
	}
}

void DrawI(void)
{
	glBegin(GL_LINES);
	glColor3f(1.0f, 0.6f, 0.2f);
	glVertex2f(0.0f, 1.0f);
	glColor3f(0.07058f, 0.54f, 0.0274f);
	glVertex2f(0.0f, -1.0f);
	glEnd();
}
void DrawN(void) {
	glBegin(GL_LINES);
	glColor3f(1.0f, 0.6f, 0.2f);
	glVertex2f(-0.5f, 1.0f);
	glColor3f(0.07058f, 0.54f, 0.0274f);
	glVertex2f(-0.5f, -1.0f);
	glColor3f(1.0f, 0.6f, 0.2f);
	glVertex2f(-0.5f, 1.0f);
	glColor3f(0.07058f, 0.54f, 0.0274f);
	glVertex2f(0.5f, -1.0f);
	glColor3f(0.07058f, 0.54f, 0.0274f);
	glVertex2f(0.5f, -1.0f);
	glColor3f(1.0f, 0.6f, 0.2f);
	glVertex2f(0.5f, 1.0f);
	glEnd();
}
void DrawD(void) {
	glBegin(GL_LINES);
	glColor3f(xOrColor, yOrColor, zOrColor);
	glVertex2f(-0.5f, 1.0f);
	glColor3f(xGrColor, yGrColor, zGrColor);
	glVertex2f(-0.5f, -1.0f);
	glColor3f(xOrColor, yOrColor, zOrColor);
	glVertex2f(-0.7f, 1.0f);
	glVertex2f(0.3f, 1.0f);
	glColor3f(xGrColor, yGrColor, zGrColor);
	//glColor3f(0.07058f, 0.54f, 0.0274f);
	glVertex2f(-0.7f, -1.0f);
	glVertex2f(0.3, -1.0f);
	glColor3f(xOrColor, yOrColor, zOrColor);
	glVertex2f(0.3f, 1.0f);
	glColor3f(xGrColor, yGrColor, zGrColor);
	glVertex2f(0.3f, -1.0f);
	glEnd();
}
void DrawA(void) {
	glBegin(GL_LINES);
	glColor3f(1.0f, 0.6f, 0.2f);
	glVertex2f(0.0f, 1.0f);
	glColor3f(0.07058f, 0.54f, 0.0274f);
	glVertex2f(-0.5f, -1.0f);
	glColor3f(1.0f, 0.6f, 0.2f);
	glVertex2f(0.0f, 1.0f);
	glColor3f(0.07058f, 0.54f, 0.0274f);
	glVertex2f(0.5f, -1.0f);
	glEnd();

}

void TriLine(void)
{
	glBegin(GL_LINES);
	glColor3f(1.0f, 0.6f, 0.2f);
	glVertex2f(-0.23, 0.08f);
	glVertex2f(0.23, 0.08f);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(-0.23, 0.05f);
	glVertex2f(0.23, 0.05f);
	glColor3f(0.07058f, 0.54f, 0.0274f);
	glVertex2f(-0.23, 0.02f);
	glVertex2f(0.23, 0.02f);
	glEnd();
}

void smoke(void)
{
	glBegin(GL_LINES);
	glColor3f(SOrange[0], SOrange[1], SOrange[2]);
	glVertex2f(-0.23, 0.08f);

	glColor3f(EOrange[0], EOrange[1], EOrange[2]);
	glVertex2f(-4.0, 0.08f);

	glColor3f(SWhite[0], SWhite[1], SWhite[2]);
	glVertex2f(-0.23f, 0.05f);

	glColor3f(EWhite[0], EWhite[1], EWhite[2]);
	glVertex2f(-4.0f, 0.05f);

	glColor3f(SGreen[0], SGreen[1], SGreen[2]);
	glVertex2f(-0.23f, 0.02f);

	glColor3f(EGreen[0], EGreen[1], EGreen[2]);
	glVertex2f(-4.0f, 0.02f);

	glEnd();
}

void PlaneBody(void) {

	glBegin(GL_QUADS);
	glColor3f((float)186 / 255, (float)226 / 255, (float)238 / 255);
	glVertex2f(-0.2, 0.05f);
	glVertex2f(-0.2, -0.05f);
	glVertex2f(0.4, -0.05f);
	glVertex2f(0.4, 0.05f);
	glEnd();
}
void PlaneWings(void)
{
	glBegin(GL_TRIANGLES);

	glVertex2f(0.5f, 0.0f);
	glVertex2f(0.4f, 0.05f);
	glVertex2f(0.4f, -0.05f);


	glEnd();

	glBegin(GL_TRIANGLES);

	glVertex2f(0.2f, 0.05f);
	glVertex2f(-0.3f, 0.4f);
	glVertex2f(0.0f, 0.05f);


	glEnd();

	glBegin(GL_TRIANGLES);

	glVertex2f(0.2f, -0.05f);
	glVertex2f(-0.3f, -0.4f);
	glVertex2f(0.0f, -0.05f);


	glEnd();

	glBegin(GL_TRIANGLES);

	glVertex2f(-0.1f, 0.05f);
	glVertex2f(-0.4f, 0.3f);
	glVertex2f(-0.2f, 0.05f);


	glEnd();

	glBegin(GL_TRIANGLES);

	glVertex2f(-0.1f, -0.05f);
	glVertex2f(-0.4f, -0.3f);
	glVertex2f(-0.2f, -0.05f);


	glEnd();
}

void IAF(void)
{
	glBegin(GL_LINES);
	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex2f(0.01f, 0.04f);
	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex2f(0.01f, -0.04f);
	glEnd();

	//AAA
	glBegin(GL_LINES);
	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex2f(0.05f, 0.04f);
	glVertex2f(0.03f, -0.04f);
	glVertex2f(0.05f, 0.04f);
	glVertex2f(0.07f, -0.04f);
	glEnd();

	glBegin(GL_LINES);
	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex2f(0.09f, 0.04f);
	glVertex2f(0.09f, -0.04f);
	glVertex2f(0.09f, 0.04f);
	glVertex2f(0.12f, 0.04);
	glVertex2f(0.09f, 0.01f);
	glVertex2f(0.12f, 0.01f);
	glEnd();
}

void resize(int width,int height)
{
	if(height==0)
	{
		height=1;
	}

	glViewport(0,0,(GLsizei)width,(GLsizei)height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,100.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void uninitialize()
{
	GLXContext ctx;
	ctx=glXGetCurrentContext();
	
	//OGL changes
	if(ctx!=NULL && ctx==gGLXContext)
	{
		glXMakeCurrent(gpDisplay,0,0);
	}
	
	if(ctx)
	{
		glXDestroyContext(gpDisplay,ctx);
	}
	
	
	if(gWindow)
	{
		XDestroyWindow(gpDisplay,gWindow);
	}
	
	if(gcolormap)
	{
		XFreeColormap(gpDisplay,gcolormap);
	}
	
	if(gpXVisualInfo)
	{
		free(gpXVisualInfo);
		gpXVisualInfo=NULL;
	}
	
	if(gpDisplay)
	{
		XCloseDisplay(gpDisplay);
		gpDisplay=NULL;
	}
}



