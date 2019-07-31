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
#define CHECK_IMAGE_WIDTH 64
#define CHECK_IMAGE_HEIGHT 64


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


//textures

GLubyte checkImage[CHECK_IMAGE_WIDTH][CHECK_IMAGE_HEIGHT][4];
GLuint texImage;

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
	char buffer[31];
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
					//XLookupString(&event.xkey, buffer, 30, &keysym, NULL);
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

					default:
					
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
		GLX_RGBA,GLX_RED_SIZE,1,GLX_GREEN_SIZE,1,GLX_BLUE_SIZE,1,GLX_ALPHA_SIZE,1,None
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

	XStoreName(gpDisplay,gWindow,"Checker Board");
	
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
	void display(void);
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
	display();
	glFlush();
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);	//change for animation

	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -3.5f);
	
	glBegin(GL_QUADS);
	glBindTexture(GL_TEXTURE_2D, 0);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-2.0f, -1.0f, 0.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-2.0f, 1.0f, 0.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.0f, 1.0f, 0.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.0f, -1.0f, 0.0f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(1.0f, -1.0f, 0.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(1.0f, 1.0f, 0.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(2.41421f, 1.0f, -1.41421f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(2.41421f, -1.0f, -1.41421f);
	glEnd();

	//glFlush();
	//glXSwapBuffers(gpDisplay,gWindow);			//change for animation
}

void spin(void)
{
	
	tri=tri+2.0f;

	if(tri>=360.0f)
	{
		tri=0.0f;
	}

	rect=rect+2.0f;

	if(rect>=360.0f)
	{
		rect=0.0f;
	}
}



void LoadGLTextures(void)
{
	void MakeCheckImage(void);
	MakeCheckImage();
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &texImage);
	glBindTexture(GL_TEXTURE_2D, texImage);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, CHECK_IMAGE_WIDTH, CHECK_IMAGE_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, &checkImage);
	//gluBuild2DMipmaps(GL_TEXTURE_2D, 3, bmp.bmWidth, bmp.bmHeight, GL_BGR_EXT, GL_UNSIGNED_BYTE, bmp.bmBits);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	//glDeleteTextures(1, &texImage);
}

void MakeCheckImage(void)
{
	int i, j, c;
	for (i = 0; i < CHECK_IMAGE_HEIGHT; i++) {
		for (j = 0; j < CHECK_IMAGE_WIDTH; j++) {
			c = ((((i & 0x8) == 0) ^ ((j & 0x8)) == 0)) * 255;
			checkImage[i][j][0] = (GLubyte)c;
			checkImage[i][j][1] = (GLubyte)c;
			checkImage[i][j][2] = (GLubyte)c;
			checkImage[i][j][3] = 255;
		}
	}
}

void initialize()
{
	void resize(int, int);
	void LoadGLTextures(void);

	gGLXContext=glXCreateContext(gpDisplay,gpXVisualInfo,NULL,GL_TRUE);
	
	glXMakeCurrent(gpDisplay,gWindow,gGLXContext);
	
	
	//change for 3d animation
	glShadeModel(GL_SMOOTH);
	glClearColor(0.0f,0.0f,0.0f,0.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	LoadGLTextures();
	
	glEnable(GL_TEXTURE_2D);
	

	glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);
	

	
	resize(giWindowWidth,giWindowHeight);


}

