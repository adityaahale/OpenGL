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
#include<SOIL/SOIL.h>	//for bmp loading 

using namespace std;



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

//lights
bool bLight=false;
GLfloat LightAmbient[]={0.5f,0.5f,0.5f,1.0f};
GLfloat LightDiffuse[]={1.0f,1.0f,1.0f,1.0f};
GLfloat LightPosition[]={0.0f,0.0f,2.0f,1.0f};

//textures
GLuint Texture_kundali;
GLuint Texture_Stone;


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

	XStoreName(gpDisplay,gWindow,"Texture");
	
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
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);	//change for animation

	glLoadIdentity();
	glTranslatef(-2.0f,0.0f,-7.0f);
	glRotatef(tri,0.0f,1.0f,0.0f);
	glBindTexture(GL_TEXTURE_2D,Texture_Stone);
	glBegin(GL_TRIANGLES);
	//triangle/pyramid code
	glTexCoord2f(0.5f, 1.0f);
	glVertex3f(0.0f, 1.0f, 0.0f); //apex of triangle

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f); //left-bottom tip of triangle

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.0f, -1.0f, 1.0f); //right-bottom tip of triangle

								   //****RIGHT FACE****
	glTexCoord2f(0.5f, 1.0f);
	glVertex3f(0.0f, 1.0f, 0.0f); //apex of triangle

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.0f, -1.0f, 1.0f); //left-bottom tip of triangle

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(1.0f, -1.0f, -1.0f); //right-bottom tip of triangle

									//****BACK FACE****
	glTexCoord2f(0.5f, 1.0f);
	glVertex3f(0.0f, 1.0f, 0.0f); //apex of triangle

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.0f, -1.0f, -1.0f); //left-bottom tip of triangle

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-1.0f, -1.0f, -1.0f); //right-bottom tip of triangle

									 //****LEFT FACE****
	glTexCoord2f(0.5f, 1.0f);
	glVertex3f(0.0f, 1.0f, 0.0f); //apex of triangle

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-1.0f, -1.0f, -1.0f); //left-bottom tip of triangle

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f); //right-bottom tip of triangle
	

	glEnd();
	//square
	glLoadIdentity();
	glTranslatef(2.0f,0.0f,-7.0f);
	glScalef(0.75f,0.75f,0.75f);
	glRotatef(rect,1.0f,1.0f,1.0f);
	glBindTexture(GL_TEXTURE_2D,Texture_kundali);

	glBegin(GL_QUADS);
	
	//top
	glTexCoord2f(0.0f,1.0f);
	glVertex3f(1.0f,1.0f,-1.0f);
	glTexCoord2f(0.0f,0.0f);
	glVertex3f(-1.0f,1.0f,-1.0f);
	glTexCoord2f(1.0f,0.0f);
	glVertex3f(-1.0f,1.0f,1.0f);
	glTexCoord2f(1.0f,1.0f);
	glVertex3f(1.0f,1.0f,1.0f);

	//bottom
	glTexCoord2f(1.0f,1.0f);
	glVertex3f(1.0f,-1.0f,-1.0f);
	glTexCoord2f(0.0f,1.0f);
	glVertex3f(-1.0f,-1.0f,-1.0f);
	glTexCoord2f(0.0f,0.0f);
	glVertex3f(-1.0f,-1.0f,1.0f);
	glTexCoord2f(1.0f,0.0f);
	glVertex3f(1.0f,-1.0f,1.0f);

	//front
	glTexCoord2f(0.0f,0.0f);
	glVertex3f(1.0f,1.0f,1.0f);
	glTexCoord2f(1.0f,0.0f);
	glVertex3f(-1.0f,1.0f,1.0f);
	glTexCoord2f(1.0f,1.0f);
	glVertex3f(-1.0f,-1.0f,1.0f);
	glTexCoord2f(0.0f,1.0f);
	glVertex3f(1.0f,-1.0f,1.0f);

	//back
	glTexCoord2f(1.0f,0.0f);
	glVertex3f(1.0f,1.0f,-1.0f);
	glTexCoord2f(1.0f,1.0f);
	glVertex3f(-1.0f,1.0f,-1.0f);
	glTexCoord2f(0.0f,1.0f);
	glVertex3f(-1.0f,-1.0f,-1.0f);
	glTexCoord2f(0.0f,0.0f);
	glVertex3f(1.0f,-1.0f,-1.0f);

	//left
	glTexCoord2f(0.0f,0.0f);
	glVertex3f(-1.0f,1.0f,1.0f);
	glTexCoord2f(1.0f,0.0f);
	glVertex3f(-1.0f,1.0f,-1.0f);
	glTexCoord2f(1.0f,1.0f);
	glVertex3f(-1.0f,-1.0f,-1.0f);
	glTexCoord2f(0.0f,1.0f);
	glVertex3f(-1.0f,-1.0f,1.0f);

	//right 
	glTexCoord2f(1.0f,0.0f);
	glVertex3f(1.0f,1.0f,-1.0f);
	glTexCoord2f(1.0f,1.0f);
	glVertex3f(1.0f,1.0f,1.0f);
	glTexCoord2f(0.0f,1.0f);
	glVertex3f(1.0f,-1.0f,1.0f);
	glTexCoord2f(0.0f,0.0f);
	glVertex3f(1.0f,-1.0f,-1.0f);
	glEnd();


	glXSwapBuffers(gpDisplay,gWindow);			//change for animation
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

bool LoadGLTextures(GLuint *texture, const char *path)
{
	/*texture = SOIL_load_OGL_texture("Smiley.bmp",
                                     SOIL_LOAD_AUTO,
                                     SOIL_CREATE_NEW_ID,
                                     SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_MULTIPLY_ALPHA
                                    );
    /*if(*texture == NULL){
        printf("[Texture loader] \"%s\" failed to load!\n", filename);
}*/
	bool bStatus = false;
	int width,height;
	unsigned char *ImageData=NULL;

	//glGenTextures(1, texture);
	ImageData=SOIL_load_image(path,&width,&height,0,SOIL_LOAD_RGB);
	if (ImageData==NULL)
	{
		bStatus = false;
		return bStatus;
	}
	else
	{
		bStatus=true;
		
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
		glGenTextures(1, texture);
		glBindTexture(GL_TEXTURE_2D, *texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, width, height, GL_RGB,GL_UNSIGNED_BYTE,ImageData);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, ImageData);

		SOIL_free_image_data(ImageData);
	
		return (bStatus);
}
		
	
	glDeleteTextures(1, texture);
}

void initialize()
{

	gGLXContext=glXCreateContext(gpDisplay,gpXVisualInfo,NULL,GL_TRUE);
	
	glXMakeCurrent(gpDisplay,gWindow,gGLXContext);
	
	
	//change for 3d animation
	glShadeModel(GL_SMOOTH);
	glClearColor(0.0f,0.0f,0.0f,0.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);
	
	//lights
	glLightfv(GL_LIGHT1,GL_AMBIENT,LightAmbient);
	glLightfv(GL_LIGHT1,GL_DIFFUSE,LightDiffuse);
	glLightfv(GL_LIGHT1,GL_POSITION,LightPosition);
	glEnable(GL_LIGHT1);
	
	//textures using SOIL
	//Texture_kundali=SOIL_load_OGL_texture("Vijay_Kundali.bmp",SOIL_LOAD_AUTO,SOIL_CREATE_NEW_ID,SOIL_FLAG_MIPMAPS);
	//Texture_Stone  =SOIL_load_OGL_texture("Stone.bmp",SOIL_LOAD_AUTO,SOIL_CREATE_NEW_ID,SOIL_FLAG_MIPMAPS);
	LoadGLTextures(&Texture_kundali,"Vijay_Kundali.bmp");
	LoadGLTextures(&Texture_Stone,"Stone.bmp");
	
	glEnable(GL_TEXTURE_2D);
	
	resize(giWindowWidth,giWindowHeight);


}
