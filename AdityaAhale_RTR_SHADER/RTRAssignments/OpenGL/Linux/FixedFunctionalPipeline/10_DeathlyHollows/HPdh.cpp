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
GLfloat rot = 0.0f,xtrans=-3.0f,ytrans=-3.0f,xctrans=3.0,yctrans=-3.0,yltrans=3.0;


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
		printf("ERROR:Unable to open XDisplay.\nEXITING..");
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

	XStoreName(gpDisplay,gWindow,"Deathly Hollows");
	
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
	//ToggleFullscreen();
	resize(giWindowWidth,giWindowHeight);


}




void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);	//change for animation
	
	float radius;			//1,-1	0,1  -1,-1 
	float area;
	float a, b, c, tx, ty;
	float x1 = -1.0f, x2 = 1.0f, x3 = 0.0f, yone = -1.0f, y2 = -1.0f, y3 = 1.0f;
	float s, summation;
	c = sqrtf((x2 - x1)*(x2 - x1) + (y2 - yone)*(y2 - yone)); //These are opposite lengths/sides of vertices
	a = sqrtf((x3 - x2)*(x3 - x2) + (y3 - y2)*(y3 - y2));
	b = sqrtf((x1 + x3)*(x1 - x3) + (yone - y3)*(yone - y3));
	summation = a + b + c;
	s = (float)(a + b + c) / 2.0f;
	tx = ((a*x1) + (b*x2) + (c*x3)) / summation;
	ty = ((a*yone) + (b*y2) + (c*y3)) / summation;
	area = sqrtf(s*(s - a)*(s - b)*(s - c));
	radius = area / s;
	
	glLoadIdentity();
	glTranslatef(xtrans, ytrans, -4.0f);
	//glTranslatef(0.0f, 0.0f, -4.0f);
	

	
	glRotatef(rot, 0.0f, 1.0f, 0.0f);
	glBegin(GL_LINES);
	glVertex2f(x1, yone);
	glVertex2f(x2, y2);
	glVertex2f(x2, y2);
	glVertex2f(x3, y3);
	glVertex2f(x3, y3);
	glVertex2f(x1, yone);
	glEnd();


	glLoadIdentity();
	//glTranslatef(xctrans, yctrans, -4.0f);
	glTranslatef(tx+xctrans, ty-xctrans, -4.0f);
	
	glRotatef(rot, 0.0f, 1.0f, 0.0f);
	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < numPoints; i++)
	{
		GLfloat angle = 2.0*PI*i / numPoints;
		//glColor3f(colourArray[coun][0], colourArray[coun][1], colourArray[coun][2]);
		glVertex3f(cos(angle)*radius, sin(angle)*radius, 0.0f);

	}
	glEnd();

	glLoadIdentity();
	glTranslatef(0.0f, yltrans, -4.0f);
	//glTranslatef(0.0f, 0.0f, -4.0f);



	glRotatef(rot, 0.0f, 1.0f, 0.0f);
	glBegin(GL_LINES);
	glVertex2f(0.0f, 1.0);
	glVertex2f(0.0f, -1.0f);
	glEnd();

	glXSwapBuffers(gpDisplay,gWindow);			//change for animation
}

void update(void)
{
	rot = rot + 0.9f;
	if (rot >= 360)
	{
		rot = 0.0f;
	}

	if (xtrans <= 0.0)
	{
		xtrans = xtrans + 0.001;
		ytrans = ytrans + 0.001;
	}
	
	if (xtrans >= 0.0)
	{
		if (xctrans > 0.0)
		{
			xctrans = xctrans - 0.001;
			yctrans = yctrans + 0.001;
			
		}

		
	}

	if (xtrans > 0.0)
	{
		if (xctrans < 0.0)
		{
			if (yltrans >= 0.0)
			{
				yltrans = yltrans - 0.001;
			}
		}
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




