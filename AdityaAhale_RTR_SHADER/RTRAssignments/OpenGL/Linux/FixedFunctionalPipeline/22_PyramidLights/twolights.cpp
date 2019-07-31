#include<iostream>
#include<stdlib.h>
#include<stdio.h>
#include<memory.h>

#include<X11/Xlib.h>
#include<X11/Xutil.h>
#include<X11/XKBlib.h>
#include<X11/keysym.h>

#include<GL/gl.h>
#include<GL/glx.h>
#include<GL/glu.h>

using namespace std;

#define LENGTH 4

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
GLUquadric *quadric = NULL;

struct MyLight
{
	GLfloat LightAmbient[LENGTH];
	GLfloat LightDiffuse[LENGTH];
	GLfloat LightSpecular[LENGTH];
	GLfloat LightPosition[LENGTH];

};

struct MyLight mylights[2];

bool bLight = false;
GLfloat LightAmbientzero[] = { 0.5f,0.5f,0.5f,1.0f };
GLfloat LightDiffusezero[] = { 1.0f,0.0f,0.0f,1.0f };
GLfloat LightPositionzero[] = { -2.0f,0.0f,0.0f,1.0f };
GLfloat LightSpecularzero[] = { 1.0f,0.0f,0.0f,1.0f };

GLfloat LightAmbientone[] = { 0.5f,0.5f,0.5f,1.0f };
GLfloat LightDiffuseone[] = { 0.0f,0.0f,1.0f,1.0f };
GLfloat LightPositionone[] = { 2.0f,0.0f,0.0f,1.0f };
GLfloat LightSpecularone[] = { 0.0f,0.0f,1.0f,1.0f };


//mylights[1].LightAmbient=LightAmbientzero;

GLfloat MaterialAmbient[] = { 0.0f,0.0f,0.0f,1.0f };
GLfloat MaterialDiffuse[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat MaterialShininess[] = { 128.0f };
GLfloat MaterialSpecular[] = { 1.0f,1.0f,1.0f,1.0f };



GLXContext gGLXContext;

int main(void)
{
	void CreateWindow(void);
	void ToggleFullscreen(void);
	void uninitialize(void);
	
	//change for OGL
	void initialize(void);
	void display(void);
	void resize(int,int);
	void spin(void);		//change for animation
	
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
						if(bFullScreen==false)
						{
							ToggleFullscreen();
							bFullScreen=true;
						}
						else
						{
							ToggleFullscreen();
							bFullScreen=false;
						}
						break;
					case XK_L:
					case XK_l:
						if(bLight==false)
						{
							bLight=true;
							glEnable(GL_LIGHTING);
						}
						else
						{
							bLight=false;
							glDisable(GL_LIGHTING);
						}
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
		display();
		spin();
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

	XStoreName(gpDisplay,gWindow,"Pyramid With two Lights");
	
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

void display(void)
{
	//code
	//to clear all pixels
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -7.0f);
	glRotatef(tri, 0.0f, 1.0f, 0.0f);

	glBegin(GL_TRIANGLES);

	glNormal3f(0.0f, 0.447214f, 0.894427f);
	glVertex3f(0.0f, 1.0f, 0.0f);

	glNormal3f(0.0f, 0.447214f, 0.894427f);
	glVertex3f(-1.0f, -1.0f, 1.0f);

	glNormal3f(0.0f, 0.447214f, 0.894427f);
	glVertex3f(1.0f, -1.0f, 1.0f);

	glNormal3f(0.894427f, 0.447214f, 0.0f);
	glVertex3f(0.0f, 1.0f, 0.0f);

	glNormal3f(0.894427f, 0.447214f, 0.0f);
	glVertex3f(1.0f, -1.0f, 1.0f);

	glNormal3f(0.894427f, 0.447214f, 0.0f);
	glVertex3f(1.0f, -1.0f, -1.0f);

	glNormal3f(0.0f, 0.447214f, -0.894427f);
	glVertex3f(0.0f, 1.0f, 0.0f);

	glNormal3f(0.0f, 0.447214f, -0.894427f);
	glVertex3f(1.0f, -1.0f, -1.0f);

	glNormal3f(0.0f, 0.447214f, -0.894427f);
	glVertex3f(-1.0f, -1.0f, -1.0f);

	glNormal3f(-0.894427f, 0.447214f, 0.0f);
	glVertex3f(0.0f, 1.0f, 0.0f);

	glNormal3f(-0.894427f, 0.447214f, 0.0f);
	glVertex3f(-1.0f, -1.0f, -1.0f);

	glNormal3f(-0.894427f, 0.447214f, 0.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f);

	glEnd();
	glXSwapBuffers(gpDisplay,gWindow);			//change for animation
}

void spin(void)
{
	
	tri=tri+1.0f;

	if(tri>=360.0f)
	{
		tri=0.0f;
	}

	rect=rect+0.1f;

	if(rect>=360.0f)
	{
		rect=0.0f;
	}
	
}

void initialize()
{

	gGLXContext=glXCreateContext(gpDisplay,gpXVisualInfo,NULL,GL_TRUE);
	
	glXMakeCurrent(gpDisplay,gWindow,gGLXContext);
	
	
	//change for 3d animation
	glShadeModel(GL_SMOOTH);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	for (int i = 0; i < LENGTH; i++) {
		mylights[0].LightAmbient[i] = LightAmbientzero[i];
		mylights[0].LightDiffuse[i] = LightDiffusezero[i];
		mylights[0].LightPosition[i] = LightPositionzero[i];
		mylights[0].LightSpecular[i] = LightSpecularzero[i];
		mylights[1].LightAmbient[i] = LightAmbientone[i];
		mylights[1].LightDiffuse[i] = LightDiffuseone[i];
		mylights[1].LightPosition[i] = LightPositionone[i];
		mylights[1].LightSpecular[i] = LightSpecularone[i];
	}


	glLightfv(GL_LIGHT0, GL_AMBIENT, mylights[0].LightAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, mylights[0].LightDiffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, mylights[0].LightPosition);
	glLightfv(GL_LIGHT0, GL_SPECULAR, mylights[0].LightSpecular);

	glLightfv(GL_LIGHT1, GL_AMBIENT, mylights[1].LightAmbient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, mylights[1].LightDiffuse);
	glLightfv(GL_LIGHT1, GL_POSITION, mylights[1].LightPosition);
	glLightfv(GL_LIGHT1, GL_SPECULAR, mylights[1].LightSpecular);


	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, MaterialShininess);
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	resize(giWindowWidth,giWindowHeight);


}


