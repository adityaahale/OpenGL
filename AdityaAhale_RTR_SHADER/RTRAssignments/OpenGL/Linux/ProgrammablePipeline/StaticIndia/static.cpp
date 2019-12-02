//headers
#include <iostream>
#include <stdio.h> //for printf()
#include <stdlib.h> //for exit()
#include <memory.h> //for memset()

//headers for XServer
#include <X11/Xlib.h> //analogous to windows.h
#include <X11/Xutil.h> //for visuals
#include <X11/XKBlib.h> //XkbKeycodeToKeysym()
#include <X11/keysym.h> //for 'Keysym'

#include <GL/glew.h> // for GLSL extensions IMPORTANT : This Line Should Be Before #include<gl\gl.h>

#include <GL/gl.h>
#include <GL/glx.h> //for 'glx' functions
#include<SOIL/SOIL.h>

#include "vmath.h"

using namespace vmath;

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

enum
{
	AMC_ATTRIBUTE_POSITION = 0,
	AMC_ATTRIBUTE_COLOR,
	AMC_ATTRIBUTE_NORMAL,
	AMC_ATTRIBUTE_TEXTURE0,
};

//global variable declarations
FILE *gpFile = NULL;

Display *gpDisplay=NULL;
XVisualInfo *gpXVisualInfo=NULL;
Colormap gColormap;
Window gWindow;
typedef GLXContext (*glXCreateContextAttribsARBProc)(Display*, GLXFBConfig, GLXContext, Bool, const int*);
glXCreateContextAttribsARBProc glXCreateContextAttribsARB=NULL;
GLXFBConfig gGLXFBConfig;
GLXContext gGLXContext; //parallel to HGLRC

bool gbFullscreen = false;

GLuint VertexShaderObject;
GLuint FragmentShaderObject;
GLuint ShaderProgramObject;

GLuint Vao_I;
GLuint Vao_N;
GLuint Vao_D;
GLuint Vao_SecondI;
GLuint Vao_A;
GLuint Vao_OrangeL;
GLuint Vao_TriLine;
GLuint Vao_Plane;
GLuint Vao_IAF;
GLuint Vao_Smoke;
//GLuint Vao_OrangeL;

GLuint Vbo_Position_I;
GLuint Vbo_Position_D;
GLuint Vbo_Position_SecondI;
GLuint Vbo_Position_A;
GLuint Vbo_Position_N;
GLuint Vbo_Position_orangeLine;
GLuint Vbo_Position_triLine;
GLuint Vbo_Position;

GLuint Vbo_Color_I;
GLuint Vbo_Color_N;
GLuint Vbo_Color_D;
GLuint Vbo_Color_SecondI;
GLuint Vbo_Color_A;
GLuint Vbo_Color_orangeLine;
GLuint Vbo_Color_TriLine;
GLuint Vbo_Color;

GLuint alphaUniform;

GLuint MVPUniform;
GLuint samplerUniform;

GLuint Texture_Kundali;
GLuint Texture_Stone;

mat4 PerspectiveProjectionMatrix;

GLfloat AnglePyramid = 0.0f; //angle of rotation of Trinagle
GLfloat AngleCube = 0.0f; //angle of rotation for Square
GLfloat angleTri = 0.0f, angleSq = 0.0f;
GLfloat rot = 0.0f, itrans = -8.0f, ntrans = 7.0f, iitrans = -7.0, atrans = 8.0;
//GLfloat xOrColor = 0.0f, yOrColor = 0.0f, zOrColor = 0.0f, xGrColor = 0.0f, yGrColor = 0.0f, zGrColor = 0.0f;
GLfloat xTritrans = -6.5f,xFlagtrans = -6.5f, Topangle = 0.0f, Bottomangle = 0.0f, xIAFtrans = -8.3f,
xPlanetrans = -8.3f, xPlane1trans = -8.3f, xPlane2trans = -8.3f, yPlane1trans = 2.0, yPlane2trans = -2.0;

GLfloat alphaBlend = 0.0f;

//entry-point function
int main(int argc, char *argv[])
{
	//function prototype
	void CreateWindow(void);
	void ToggleFullscreen(void);
	void initialize(void);
	void resize(int,int);
	void display(void);
	void spin(void);
	void uninitialize(void);
	void translatemy(void);
	//code
	// create log file
	gpFile=fopen("Log.txt", "w");
	if (gpFile==NULL)
	{
		printf("Log File Can Not Be Created. EXitting Now ...\n");
		exit(0);
	}
	else
	{
		fprintf(gpFile, "Log File Is Successfully Opened.\n");
	}
	
	// create the window
	CreateWindow();
	
	//initialize()
	initialize();
	
	//Message Loop

	//variable declarations
	XEvent event; //parallel to 'MSG' structure
	KeySym keySym;
	int winWidth;
	int winHeight;
	bool bDone=false;
	
	while(bDone==false)
	{
		while(XPending(gpDisplay))
		{
			XNextEvent(gpDisplay,&event); //parallel to GetMessage()
			switch(event.type) //parallel to 'iMsg'
			{
				case MapNotify: //parallel to WM_CREATE
					break;
				case KeyPress: //parallel to WM_KEYDOWN
					keySym=XkbKeycodeToKeysym(gpDisplay,event.xkey.keycode,0,0);
					switch(keySym)
					{
						case XK_Escape:
							bDone=true;
							break;
						case XK_F:
						case XK_f:
							if(gbFullscreen==false)
							{
								ToggleFullscreen();
								gbFullscreen=true;
							}
							else
							{
								ToggleFullscreen();
								gbFullscreen=false;
							}
							break;
						default:
							break;
					}
					break;
				case ButtonPress:
					switch(event.xbutton.button)
					{
						case 1: //Left Button
							break;
						case 2: //Middle Button
							break;
						case 3: //Right Button
							break;
						default: 
							break;
					}
					break;
				case MotionNotify: //parallel to WM_MOUSEMOVE
					break;
				case ConfigureNotify: //parallel to WM_SIZE
					winWidth=event.xconfigure.width;
					winHeight=event.xconfigure.height;
					resize(winWidth,winHeight);
					break;
				case Expose: //parallel to WM_PAINT
					break;
				case DestroyNotify:
					break;
				case 33: //close button, system menu -> close
					bDone=true;
					break;
				default:
					break;
			}
		}
		
		display();
		spin();
		
	}
	
	uninitialize();
	return(0);
}

void CreateWindow(void)
{
	//function prototype
	void uninitialize(void);
	
	//variable declarations
	XSetWindowAttributes winAttribs;
	GLXFBConfig *pGLXFBConfigs=NULL;
	GLXFBConfig bestGLXFBConfig;
	XVisualInfo *pTempXVisualInfo=NULL;
	int iNumFBConfigs=0;
	int styleMask;
	int i;
	
	static int frameBufferAttributes[]={
		GLX_X_RENDERABLE,True,
		GLX_DRAWABLE_TYPE,GLX_WINDOW_BIT,
		GLX_RENDER_TYPE,GLX_RGBA_BIT,
		GLX_X_VISUAL_TYPE,GLX_TRUE_COLOR,
		GLX_RED_SIZE,8,
		GLX_GREEN_SIZE,8,
		GLX_BLUE_SIZE,8,
		GLX_ALPHA_SIZE,8,
		GLX_DEPTH_SIZE,24,
		GLX_STENCIL_SIZE,8,
		GLX_DOUBLEBUFFER,True,
		//GLX_SAMPLE_BUFFERS,1,
		//GLX_SAMPLES,4,
		None}; // array must be terminated by 0
	
	//code
	gpDisplay=XOpenDisplay(NULL);
	if(gpDisplay==NULL)
	{
		printf("ERROR : Unable To Obtain X Display.\n");
		uninitialize();
		exit(1);
	}
	
	// get a new framebuffer config that meets our attrib requirements
	pGLXFBConfigs=glXChooseFBConfig(gpDisplay,DefaultScreen(gpDisplay),frameBufferAttributes,&iNumFBConfigs);
	if(pGLXFBConfigs==NULL)
	{
		printf( "Failed To Get Valid Framebuffer Config. Exitting Now ...\n");
		uninitialize();
		exit(1);
	}
	printf("%d Matching FB Configs Found.\n",iNumFBConfigs);
	
	// pick that FB config/visual with the most samples per pixel
	int bestFramebufferconfig=-1,worstFramebufferConfig=-1,bestNumberOfSamples=-1,worstNumberOfSamples=999;
	for(i=0;i<iNumFBConfigs;i++)
	{
		pTempXVisualInfo=glXGetVisualFromFBConfig(gpDisplay,pGLXFBConfigs[i]);
		if(pTempXVisualInfo)
		{
			int sampleBuffer,samples;
			glXGetFBConfigAttrib(gpDisplay,pGLXFBConfigs[i],GLX_SAMPLE_BUFFERS,&sampleBuffer);
			glXGetFBConfigAttrib(gpDisplay,pGLXFBConfigs[i],GLX_SAMPLES,&samples);
			printf("Matching Framebuffer Config=%d : Visual ID=0x%lu : SAMPLE_BUFFERS=%d : SAMPLES=%d\n",i,pTempXVisualInfo->visualid,sampleBuffer,samples);
			if(bestFramebufferconfig < 0 || sampleBuffer && samples > bestNumberOfSamples)
			{
				bestFramebufferconfig=i;
				bestNumberOfSamples=samples;
			}
			if( worstFramebufferConfig < 0 || !sampleBuffer || samples < worstNumberOfSamples)
			{
				worstFramebufferConfig=i;
			    worstNumberOfSamples=samples;
			}
		}
		XFree(pTempXVisualInfo);
	}
	bestGLXFBConfig = pGLXFBConfigs[bestFramebufferconfig];
	// set global GLXFBConfig
	gGLXFBConfig=bestGLXFBConfig;
	
	// be sure to free FBConfig list allocated by glXChooseFBConfig()
	XFree(pGLXFBConfigs);
	
	gpXVisualInfo=glXGetVisualFromFBConfig(gpDisplay,bestGLXFBConfig);
	printf("Chosen Visual ID=0x%lu\n",gpXVisualInfo->visualid );
	
	//setting window's attributes
	winAttribs.border_pixel=0;
	winAttribs.background_pixmap=0;
	winAttribs.colormap=XCreateColormap(gpDisplay,
										RootWindow(gpDisplay,gpXVisualInfo->screen), //you can give defaultScreen as well
										gpXVisualInfo->visual,
										AllocNone); //for 'movable' memory allocation
										
	winAttribs.event_mask=StructureNotifyMask | KeyPressMask | ButtonPressMask |
						  ExposureMask | VisibilityChangeMask | PointerMotionMask;
	
	styleMask=CWBorderPixel | CWEventMask | CWColormap;
	gColormap=winAttribs.colormap;										           
	
	gWindow=XCreateWindow(gpDisplay,
						  RootWindow(gpDisplay,gpXVisualInfo->screen),
						  0,
						  0,
						  WIN_WIDTH,
						  WIN_HEIGHT,
						  0, //border width
						  gpXVisualInfo->depth, //depth of visual (depth for Colormap)          
						  InputOutput, //class(type) of your window
						  gpXVisualInfo->visual,
						  styleMask,
						  &winAttribs);
	if(!gWindow)
	{
		printf("Failure In Window Creation.\n");
		uninitialize();
		exit(1);
	}
	
	XStoreName(gpDisplay,gWindow,"OpenGL Programmable Pipeline Window");
	
	Atom windowManagerDelete=XInternAtom(gpDisplay,"WM_WINDOW_DELETE",True);
	XSetWMProtocols(gpDisplay,gWindow,&windowManagerDelete,1);
	
	XMapWindow(gpDisplay,gWindow);
}

void ToggleFullscreen(void)
{
	//code
	Atom wm_state=XInternAtom(gpDisplay,"_NET_WM_STATE",False); //normal window state
	
	XEvent event;
	memset(&event,0,sizeof(XEvent));
	
	event.type=ClientMessage;
	event.xclient.window=gWindow;
	event.xclient.message_type=wm_state;
	event.xclient.format=32; //32-bit
	event.xclient.data.l[0]=gbFullscreen ? 0 : 1;

	Atom fullscreen=XInternAtom(gpDisplay,"_NET_WM_STATE_FULLSCREEN",False);	
	event.xclient.data.l[1]=fullscreen;
	
	//parallel to SendMessage()
	XSendEvent(gpDisplay,
			   RootWindow(gpDisplay,gpXVisualInfo->screen),
			   False, //do not send this message to Sibling windows
			   StructureNotifyMask, //resizing mask (event_mask)
			   &event);	
}


void initialize(void)
{
	// function declarations
	void uninitialize(void);
	void resize(int,int);
	//bool LoadGLTextures(unsigned int*, char const*)
	//code
	// create a new GL context 4.5 for rendering
	glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc)glXGetProcAddressARB((GLubyte *)"glXCreateContextAttribsARB");
	
	GLint attribs[] = {
		GLX_CONTEXT_MAJOR_VERSION_ARB,4,
		GLX_CONTEXT_MINOR_VERSION_ARB,5,
		GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
		0 }; // array must be terminated by 0
		
	gGLXContext = glXCreateContextAttribsARB(gpDisplay,gGLXFBConfig,0,True,attribs);

	if(!gGLXContext) // fallback to safe old style 2.x context
	{
		// When a context version below 3.0 is requested, implementations will return 
		// the newest context version compatible with OpenGL versions less than version 3.0.
		GLint attribs[] = {
			GLX_CONTEXT_MAJOR_VERSION_ARB,1,
			GLX_CONTEXT_MINOR_VERSION_ARB,0,
			0 }; // array must be terminated by 0
		printf("Failed To Create GLX 4.5 context. Hence Using Old-Style GLX Context\n");
		gGLXContext = glXCreateContextAttribsARB(gpDisplay,gGLXFBConfig,0,True,attribs);
	}
	else // successfully created 4.5 context
	{
		printf("OpenGL Context 4.5 Is Created.\n");
	}
	
	// verifying that context is a direct context
	if(!glXIsDirect(gpDisplay,gGLXContext))
	{
		printf("Indirect GLX Rendering Context Obtained\n");
	}
	else
	{
		printf("Direct GLX Rendering Context Obtained\n" );
	}
	
	glXMakeCurrent(gpDisplay,gWindow,gGLXContext);
	
	//code
	// GLEW Initialization Code For GLSL ( IMPORTANT : It Must Be Here. Means After Creating OpenGL Context But Before Using Any OpenGL Function )
	GLenum glew_error = glewInit();
	if (glew_error != GLEW_OK)
	{
		printf("Failure To Initialize GLEW. Exitting Now ...\n");
		uninitialize();
		exit(1);
	}

	// *** VERTEX SHADER ***
	// create shader
	VertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

	// provide source code to shader
	const GLchar *vertexShaderSourceCode =
		"#version 450 core" \
		"\n" \
		"in vec4 vPosition;" \
		"in vec4 vColor;" \
		"uniform mat4 u_mvp_matrix;" \
		"out vec4 out_color;"
		"void main(void)" \
		"{" \
		"gl_Position = u_mvp_matrix * vPosition;" \
		"out_color = vColor;"
		"}";
	glShaderSource(VertexShaderObject, 1, (const GLchar **)&vertexShaderSourceCode, NULL);

	// compile shader
	glCompileShader(VertexShaderObject);
	GLint iInfoLogLength = 0;
	GLint iShaderCompiledStatus = 0;
	char *szInfoLog = NULL;
	glGetShaderiv(VertexShaderObject, GL_COMPILE_STATUS, &iShaderCompiledStatus);
	if (iShaderCompiledStatus == GL_FALSE)
	{
		glGetShaderiv(VertexShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (char *)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(VertexShaderObject, iInfoLogLength, &written, szInfoLog);
				fprintf(gpFile, "Vertex Shader Compilation Log : %s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				exit(0);
			}
		}
	}

	// *** FRAGMENT SHADER ***
	// create shader
	FragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);

	// provide source code to shader
	const GLchar *fragmentShaderSourceCode =
		"#version 450 core" \
		"\n" \
		"in vec4 out_color;" \
		"out vec4 FragColor;" \
		"void main(void)" \
		"{" \
		"FragColor = out_color;" \
		"}";
	glShaderSource(FragmentShaderObject, 1, (const GLchar **)&fragmentShaderSourceCode, NULL);

	// compile shader
	glCompileShader(FragmentShaderObject);
	glGetShaderiv(FragmentShaderObject, GL_COMPILE_STATUS, &iShaderCompiledStatus);
	if (iShaderCompiledStatus == GL_FALSE)
	{
		glGetShaderiv(FragmentShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (char *)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(FragmentShaderObject, iInfoLogLength, &written, szInfoLog);
				fprintf(gpFile, "Fragment Shader Compilation Log : %s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				exit(0);
			}
		}
	}

	// *** SHADER PROGRAM ***
	// create
	ShaderProgramObject = glCreateProgram();

	// attach vertex shader to shader program
	glAttachShader(ShaderProgramObject, VertexShaderObject);

	// attach fragment shader to shader program
	glAttachShader(ShaderProgramObject, FragmentShaderObject);

	// pre-link binding of shader program object with vertex shader position attribute
	glBindAttribLocation(ShaderProgramObject, AMC_ATTRIBUTE_POSITION, "vPosition");
	glBindAttribLocation(ShaderProgramObject, AMC_ATTRIBUTE_COLOR, "vColor");

	// pre-link binding of shader program object with vertex shader color attribute
//	glBindAttribLocation(ShaderProgramObject, AMC_ATTRIBUTE_COLOR, "vColor");
	
	// link shader
	glLinkProgram(ShaderProgramObject);
	GLint iShaderProgramLinkStatus = 0;
	glGetProgramiv(ShaderProgramObject, GL_LINK_STATUS, &iShaderProgramLinkStatus);
	if (iShaderProgramLinkStatus == GL_FALSE)
	{
		glGetProgramiv(ShaderProgramObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength>0)
		{
			szInfoLog = (char *)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(ShaderProgramObject, iInfoLogLength, &written, szInfoLog);
				fprintf(gpFile, "Shader Program Link Log : %s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				exit(0);
			}
		}
	}

	// get MVP uniform location
	MVPUniform = glGetUniformLocation(ShaderProgramObject, "u_mvp_matrix");
	//samplerUniform = glGetUniformLocation(ShaderProgramObject, "u_sampler");


	// *** vertices, colors, shader attribs, vbo, vao initializations ***
	const GLfloat I[] = {
		0.0f,1.0f,0.0f,
		0.0f,-1.0f,0.0f
	};

	const GLfloat N[] = {
		-0.5f, 1.0f,0.0f,
	-0.5f, -1.0f,0.0f,
	-0.5f, 1.0f,0.0f,
	0.5f, -1.0f,0.0f,
	0.5f, -1.0f,0.0f,
	0.5f, 1.0f,0.0f,
	0.5f,1.0f,0.0f
	};

	const GLfloat D[] = {
		-0.5f, 1.0f,0.0f,
	-0.5f, -1.0f,0.0f,
	-0.7f, 1.0f,0.0f,
	0.3f, 1.0f,0.0f,
	-0.7f, -1.0f,0.0f,
	0.3, -1.0f,0.0f,
	0.3f, 1.0f,0.0f,
	0.3f, -1.0f,0.0f };

	const GLfloat A[] = {
	0.0f, 1.0f,0.0f,
	-0.5f, -1.0f,0.0f,
	0.0f, 1.0f,0.0f,
	0.0f, 1.0f,0.0f,
	0.5f, -1.0f,0.0f,
	0.0f, 1.0f,0.0f
	};

	const GLfloat greenLine[]{
		0.0f
	};

	const GLfloat TriColor[] = {
		1.0f, 0.6f, 0.2f,
		0.07058f, 0.54f, 0.0274f,
		1.0f, 0.6f, 0.2f,
		1.0f, 0.6f, 0.2f,
		0.07058f, 0.54f, 0.0274f,
		0.07058f, 0.54f, 0.0274f,
		1.0f, 0.6f, 0.2f,
		0.07058f, 0.54f, 0.0274f
	};

	const GLfloat AColor[] = {
			1.0f, 0.6f, 0.2f,
	0.07058f, 0.54f, 0.0274f,
	1.0f, 0.6f, 0.2f,
	0.07058f, 0.54f, 0.0274f,
	0.07058f, 0.54f, 0.0274f,
	1.0f, 0.6f, 0.2f,
	0.07058f, 0.54f, 0.0274f
	};

	const GLfloat flagColor[] = {
		1.0f, 0.6f, 0.2f,
	1.0f, 0.6f, 0.2f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	0.07058f, 0.54f, 0.0274f,
	0.07058f, 0.54f, 0.0274f

	};

	GLfloat NColor[] = {
	1.0f, 0.6f, 0.2f,
	0.07058f, 0.54f, 0.0274f,
	1.0f, 0.6f, 0.2f,
	0.07058f, 0.54f, 0.0274f,
	0.07058f, 0.54f, 0.0274f,
	1.0f, 0.6f, 0.2f
	};

	GLfloat greenVertices[] = {
		-0.7f,0.7f,0.0f,
		-0.7f,-0.7f,0.0f,
		0.7f,-0.7f,0.0f,
		0.7f,0.7f,0.0f
	};

	GLfloat orangeVertices[] = {
		1.0f,1.0f,0.0f,
		1.0f,1.0f,0.0f,
		1.0f,1.0f,0.0f,
		1.0f,1.0f,0.0f,
		1.0f,1.0f,0.0f,
		1.0f,1.0f,0.0f,
		1.0f,1.0f,0.0f,
		1.0f,1.0f,0.0f,
		1.0f,1.0f,0.0f,
		1.0f,1.0f,0.0f,
		1.0f,1.0f,0.0f,
		1.0f,1.0f,0.0f,
		1.0f,1.0f,0.0f,
		1.0f,1.0f,0.0f,
		1.0f,1.0f,0.0f,
		1.0f,1.0f,0.0f,
	};

	const GLfloat triVertices[]{
	-0.2f, 0.2f,0.0f,
0.2f, 0.2f,0.0f,
-0.2f, 0.15f,0.0f,
0.2f, 0.15f,0.0f,
-0.2f, 0.1f,0.0f,
0.2f, 0.1f,0.0f
	};
	
	//White Line
	glGenVertexArrays(1, &Vao_TriLine);
	glBindVertexArray(Vao_TriLine);

	glGenBuffers(1, &Vbo_Position_triLine);
	glBindBuffer(GL_ARRAY_BUFFER, Vbo_Position_triLine);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triVertices), triVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &Vbo_Color_TriLine);
	glBindBuffer(GL_ARRAY_BUFFER, Vbo_Color_TriLine);
	glBufferData(GL_ARRAY_BUFFER, sizeof(flagColor), flagColor, GL_STATIC_DRAW);

	glVertexAttribPointer(AMC_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	//glVertexAttrib3f(AMC_ATTRIBUTE_COLOR, 1.0f, 0.0f, 1.0f);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR);

	glBindBuffer(GL_ARRAY_BUFFER, 0);


	glBindVertexArray(0);


	//2nd I
	glGenVertexArrays(1, &Vao_SecondI);
	glBindVertexArray(Vao_SecondI);

	glGenBuffers(1, &Vbo_Position_SecondI);
	glBindBuffer(GL_ARRAY_BUFFER, Vbo_Position_SecondI);
	glBufferData(GL_ARRAY_BUFFER, sizeof(I), I, GL_STATIC_DRAW);

	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &Vbo_Color_SecondI);
	glBindBuffer(GL_ARRAY_BUFFER, Vbo_Color_SecondI);
	glBufferData(GL_ARRAY_BUFFER, sizeof(TriColor), TriColor, GL_STATIC_DRAW);

	glVertexAttribPointer(AMC_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	//glVertexAttrib3f(AMC_ATTRIBUTE_COLOR, 1.0f, 0.0f, 1.0f);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR);

	glBindBuffer(GL_ARRAY_BUFFER, 0);


	glBindVertexArray(0);


	//A
	glGenVertexArrays(1, &Vao_A);
	glBindVertexArray(Vao_A);

	glGenBuffers(1, &Vbo_Position_A);
	glBindBuffer(GL_ARRAY_BUFFER, Vbo_Position_A);
	glBufferData(GL_ARRAY_BUFFER, sizeof(A), A, GL_STATIC_DRAW);

	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &Vbo_Color_A);
	glBindBuffer(GL_ARRAY_BUFFER, Vbo_Color_A);
	glBufferData(GL_ARRAY_BUFFER, sizeof(AColor), AColor, GL_STATIC_DRAW);

	glVertexAttribPointer(AMC_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	//glVertexAttrib3f(AMC_ATTRIBUTE_COLOR, 1.0f, 0.0f, 1.0f);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR);

	glBindBuffer(GL_ARRAY_BUFFER, 0);


	glBindVertexArray(0);

	//For D
	glGenVertexArrays(1, &Vao_D);
	glBindVertexArray(Vao_D);

	glGenBuffers(1, &Vbo_Position_D);
	glBindBuffer(GL_ARRAY_BUFFER, Vbo_Position_D);
	glBufferData(GL_ARRAY_BUFFER, sizeof(D), D, GL_STATIC_DRAW);

	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &Vbo_Color_D);
	glBindBuffer(GL_ARRAY_BUFFER, Vbo_Color_D);
	glBufferData(GL_ARRAY_BUFFER, sizeof(TriColor), TriColor, GL_STATIC_DRAW);

	glVertexAttribPointer(AMC_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	//glVertexAttrib3f(AMC_ATTRIBUTE_COLOR, 0.0f, 0.0f, 1.0f);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//I
	glGenVertexArrays(1, &Vao_I);
	glBindVertexArray(Vao_I);

	glGenBuffers(1, &Vbo_Position_I);
	glBindBuffer(GL_ARRAY_BUFFER, Vbo_Position_I);
	glBufferData(GL_ARRAY_BUFFER, sizeof(I), I, GL_STATIC_DRAW);

	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &Vbo_Color_I);
	glBindBuffer(GL_ARRAY_BUFFER, Vbo_Color_I);
	glBufferData(GL_ARRAY_BUFFER, sizeof(TriColor), TriColor, GL_STATIC_DRAW);

	glVertexAttribPointer(AMC_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	//glVertexAttrib3f(AMC_ATTRIBUTE_COLOR, 1.0f, 1.0f, 0.0f);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	//N
	glGenVertexArrays(1, &Vao_N);
	glBindVertexArray(Vao_N);

	glGenBuffers(1, &Vbo_Position_N);
	glBindBuffer(GL_ARRAY_BUFFER, Vbo_Position_N);
	glBufferData(GL_ARRAY_BUFFER, sizeof(N), N, GL_STATIC_DRAW);

	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &Vbo_Color_N);
	glBindBuffer(GL_ARRAY_BUFFER, Vbo_Color_N);
	glBufferData(GL_ARRAY_BUFFER, sizeof(NColor), NColor, GL_STATIC_DRAW);

	glVertexAttribPointer(AMC_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	//glVertexAttrib3f(AMC_ATTRIBUTE_COLOR, 1.0f, 1.0f, 0.0f);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
	
	// ==================
	glClearDepth(1.0f);
	
	glEnable(GL_DEPTH_TEST);
	
	glDisable(GL_CULL_FACE);

	
	glClearColor(0.0f,0.0f,0.0f,0.0f); // black
	
	PerspectiveProjectionMatrix = mat4::identity();	
	// resize
	resize(WIN_WIDTH, WIN_HEIGHT);

}

void resize(int width,int height)
{
    //code
	if(height==0)
		height=1;
		
	glViewport(0,0,(GLsizei)width,(GLsizei)height);

	PerspectiveProjectionMatrix = perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);

}

void display(void)
{
	//code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// start using OpenGL program object
	glUseProgram(ShaderProgramObject);
	
	glLineWidth(3.0f);


	mat4 modelViewMatrix;
	mat4 modelViewProjectionMatrix;
	mat4 rotationMatrix;
	mat4 translationMatrix;

	// OpenGL Drawing

	glBindVertexArray(Vao_SecondI);
	modelViewMatrix = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();
	rotationMatrix = mat4::identity();
	translationMatrix = mat4::identity();
	
	translationMatrix = translate(1.0f, 0.0f, -6.0f);
	modelViewMatrix = modelViewMatrix * translationMatrix;
	modelViewProjectionMatrix = PerspectiveProjectionMatrix * modelViewMatrix;

	glUniformMatrix4fv(MVPUniform, 1, GL_FALSE, modelViewProjectionMatrix);

	glDrawArrays(GL_LINES, 0,2 );

	glBindVertexArray(0);

	glBindVertexArray(Vao_TriLine);
	modelViewMatrix = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();
	rotationMatrix = mat4::identity();
	translationMatrix = mat4::identity();

	translationMatrix = translate(2.0f, 0.0f, -6.0f);
	modelViewMatrix = modelViewMatrix * translationMatrix;
	modelViewProjectionMatrix = PerspectiveProjectionMatrix * modelViewMatrix;

	glUniformMatrix4fv(MVPUniform, 1, GL_FALSE, modelViewProjectionMatrix);

	glDrawArrays(GL_LINES, 0, 6);
	//glDrawArrays(GL_LINES, 2, 6);
	//glDrawArrays(GL_LINES, 4, 6);

	glBindVertexArray(0);



	
	glBindVertexArray(Vao_A);
	//glLineWidth(3.0f);
	modelViewMatrix = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();
	rotationMatrix = mat4::identity();
	translationMatrix = mat4::identity();


	translationMatrix = translate(2.0f, 0.0f, -6.0f);
	modelViewMatrix = modelViewMatrix * translationMatrix;
	modelViewProjectionMatrix = PerspectiveProjectionMatrix * modelViewMatrix;

	glUniformMatrix4fv(MVPUniform, 1, GL_FALSE, modelViewProjectionMatrix);

	glDrawArrays(GL_LINES, 0, 6);
	//glDrawArrays(GL_LINE_LOOP, 3, 6);
	//glDrawArrays(GL_LINES, 0, 3);

	glBindVertexArray(0);

	glBindVertexArray(Vao_I);
	//glLineWidth(3.0f);
	modelViewMatrix = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();
	rotationMatrix = mat4::identity();
	translationMatrix = mat4::identity();


	translationMatrix = translate(-2.5f, 0.0f, -6.0f);
	modelViewMatrix = modelViewMatrix * translationMatrix;
	modelViewProjectionMatrix = PerspectiveProjectionMatrix * modelViewMatrix;

	glUniformMatrix4fv(MVPUniform, 1, GL_FALSE, modelViewProjectionMatrix);

	glDrawArrays(GL_LINES, 0, 3);

	glBindVertexArray(0);


	glBindVertexArray(Vao_N);
	modelViewMatrix = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();
	rotationMatrix = mat4::identity();
	translationMatrix = mat4::identity();


	translationMatrix = translate(-1.5f, 0.0f, -6.0f);
	modelViewMatrix = modelViewMatrix * translationMatrix;
	modelViewProjectionMatrix = PerspectiveProjectionMatrix * modelViewMatrix;

	glUniformMatrix4fv(MVPUniform, 1, GL_FALSE, modelViewProjectionMatrix);

	glDrawArrays(GL_LINE_STRIP, 0, 7);

	glBindVertexArray(0);


	glBindVertexArray(Vao_D);
	//glLineWidth(1.0f);
	modelViewMatrix = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();
	rotationMatrix = mat4::identity();
	translationMatrix = mat4::identity();
	//MultiplyMatrices
	modelViewMatrix = translate(0.0f, 0.0f, 0.0f);
	translationMatrix = translate(0.2f, 0.0f, -6.0f);
	modelViewMatrix = modelViewMatrix * translationMatrix;
	modelViewProjectionMatrix = PerspectiveProjectionMatrix * modelViewMatrix;

	glUniformMatrix4fv(MVPUniform, 1, GL_FALSE, modelViewProjectionMatrix);



	glDrawArrays(GL_LINES, 0, 2);
	glDrawArrays(GL_LINES, 2, 4);
	glDrawArrays(GL_LINES, 4, 8);

	// *** unbind vao ***
	glBindVertexArray(0);

	//====================
	// stop using OpenGL program object
	glUseProgram(0);
	
	//to process buffered OpenGL Routines
	glXSwapBuffers(gpDisplay,gWindow);
}



void spin(void)
{
	// code
		
	AnglePyramid = AnglePyramid + 1.0f;
	if (AnglePyramid >= 360.0f)
		AnglePyramid = AnglePyramid - 360.0f;

	AngleCube = AngleCube + 1.0f;
	if (AngleCube >= 360.0f)
		AngleCube = AngleCube - 360.0f;
}

void uninitialize(void)
{
	//code

	// destroy vao
	if (Vao_SecondI)
	{
		glDeleteVertexArrays(1, &Vao_SecondI);
		Vao_SecondI = 0;
	}

	if (Vao_D)
	{
		glDeleteVertexArrays(1, &Vao_D);
		Vao_D = 0;
	}

	if (Vbo_Position_A)
	{
		glDeleteBuffers(1, &Vbo_Position_A);
		Vbo_Position_A = 0;
	}

	if (Vbo_Position_SecondI)
	{
		glDeleteBuffers(1, &Vbo_Position_SecondI);
		Vbo_Position_SecondI = 0;
	}

	if (Vbo_Color_N)
	{
		glDeleteBuffers(1, &Vbo_Color_N);
		Vbo_Color_N = 0;
	}

	if (Vao_I)
	{
		glDeleteVertexArrays(1, &Vao_I);
		Vao_I = 0;
	}

	if (Vbo_Position_I)
	{
		glDeleteBuffers(1, &Vbo_Position_I);
		Vbo_Position_I = 0;
	}

	if (Vbo_Position_D)
	{
		glDeleteBuffers(1, &Vbo_Position_D);
		Vbo_Position_D = 0;
	}

	if (Vbo_Position_orangeLine)
	{
		glDeleteBuffers(1, &Vbo_Position_orangeLine);
		Vbo_Position_orangeLine = 0;
	}


	if (Vbo_Color_I)
	{
		glDeleteBuffers(1, &Vbo_Color_I);
		Vbo_Color_I = 0;

	}

	if (Vbo_Color_TriLine)
	{
		glDeleteBuffers(1, &Vbo_Color_TriLine);
		Vbo_Color_TriLine = 0;
	}

	if (Vbo_Color_orangeLine)
	{
		glDeleteBuffers(1, &Vbo_Color_orangeLine);
		Vbo_Color_orangeLine = 0;
	}

	if (Vbo_Color)
	{
		glDeleteBuffers(1, &Vbo_Color);
		Vbo_Color = 0;
	}

	if (Vao_A)
	{
		glDeleteVertexArrays(1, &Vao_A);
		Vao_A = 0;
	}

	if (Vao_N)
	{
		glDeleteVertexArrays(1, &Vao_N);
		Vao_N = 0;
	}

	// Releasing OpenGL related and XWindow related objects 	
	GLXContext currentContext=glXGetCurrentContext();
	if(currentContext!=NULL && currentContext==gGLXContext)
	{
		glXMakeCurrent(gpDisplay,0,0);
	}
	
	if(gGLXContext)
	{
		glXDestroyContext(gpDisplay,gGLXContext);
	}
	
	if(gWindow)
	{
		XDestroyWindow(gpDisplay,gWindow);
	}
	
	if(gColormap)
	{
		XFreeColormap(gpDisplay,gColormap);
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

	if (gpFile)
	{
		fprintf(gpFile, "Log File Is Successfully Closed.\n");
		fclose(gpFile);
		gpFile = NULL;
	}
}
