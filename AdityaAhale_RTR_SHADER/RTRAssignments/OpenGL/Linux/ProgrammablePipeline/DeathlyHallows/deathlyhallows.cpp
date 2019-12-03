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
#define PI 3.14285714286

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

GLuint Vao_VerLine;
GLuint Vao_Circle;
GLuint Vao_Triangle;

GLuint Vbo_Position_VerLine;
GLuint Vbo_Position_Circle;
GLuint Vbo_Position_Triangle;

GLuint Vbo_Color_VerLine;
GLuint Vbo_Color_Circle;
GLuint Vbo_Color_Triangle;

GLuint MVPUniform;
GLuint samplerUniform;

GLuint Texture_Kundali;
GLuint Texture_Stone;

mat4 PerspectiveProjectionMatrix;

GLfloat AnglePyramid = 0.0f; //angle of rotation of Trinagle
GLfloat AngleCube = 0.0f; //angle of rotation for Square
GLfloat rot = 0.0f, xtrans = -3.0f, ytrans = -3.0f, xctrans = 3.0, yctrans = -3.0, yltrans = 3.0;
float radius;			//1,-1	0,1  -1,-1 
float area;
float a, b, c, tx, ty;
float x1 = 0.0f, x2 = -0.7f, x3 = 0.7f, yone = 0.7f, y2 = -0.7f, y3 = -0.7f;
float s, summation;
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
		"in vec4 out_color;"
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


	
	const GLfloat verticalVertices[] =
	{
		0.0f,0.7f,0.0f,
		0.0f,-0.7f,0.0f
	};
	
	

	const GLfloat verticalColor[] =
	{
		0.0f,1.0f,0.0f,
		0.0f,1.0f,0.0f
	};

	
	//INCIRCLE CALCULATION
	c = sqrtf((x2 - x1)*(x2 - x1) + (y2 - yone)*(y2 - yone)); //These are opposite lengths/sides of vertices
	a = sqrtf((x3 - x2)*(x3 - x2) + (y3 - y2)*(y3 - y2));
	b = sqrtf((x1 - x3)*(x1 - x3) + (yone - y3)*(yone - y3));
	summation = a + b + c;
	s = (float)(a + b + c) / 2.0f;
	
	area = sqrtf(s*(s - a)*(s - b)*(s - c));
	radius = area / s;


	
	GLfloat circleVertices[3000];
	//GLfloat numberOfVertices = 1000 + 1;

	GLfloat doublePi = 2.0f * PI;

	GLfloat circleVerticesX[3000];
	GLfloat circleVerticesY[3000];
	GLfloat circleVerticesZ[3000];	
		

	for (int i = 0; i < 3000; i++)
	{
		circleVerticesX[i] = (radius * cos(i * doublePi / 1000));
		circleVerticesY[i] = (radius * sin(i * doublePi / 1000));
		circleVerticesZ[i] = 0.0;
	}

	GLfloat allCircleVertices[3000 * 3];

	for (int i = 0; i < 3000; i++)
	{
		allCircleVertices[i * 3] = circleVerticesX[i];
		allCircleVertices[(i * 3) + 1] = circleVerticesY[i];
		allCircleVertices[(i * 3) + 2] = circleVerticesZ[i];
	}


	GLfloat circleColor[9000];
	for (int j = 0; j < 3000; j++)
	{
		circleColor[j * 3] = 1.0f;
		circleColor[(j * 3)+1] = 1.0f;
		circleColor[(j * 3)+2] = 1.0f;
	}
	
	GLfloat triangleVertices[] = {

	0.0f,0.7f,
	-0.7f,-0.7f,
	0.7f,-0.7f
	};

	

	//Circle
	glGenVertexArrays(1, &Vao_Circle);
	glBindVertexArray(Vao_Circle);

	glGenBuffers(1, &Vbo_Position_Circle);
	glBindBuffer(GL_ARRAY_BUFFER, Vbo_Position_Circle);
	glBufferData(GL_ARRAY_BUFFER, sizeof(allCircleVertices), allCircleVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &Vbo_Color_Circle);
	glBindBuffer(GL_ARRAY_BUFFER, Vbo_Color_Circle);
	glBufferData(GL_ARRAY_BUFFER, sizeof(circleColor), circleColor, GL_STATIC_DRAW);

	glVertexAttribPointer(AMC_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	//glVertexAttrib3f(AMC_ATTRIBUTE_COLOR, 1.0f, 0.0f, 1.0f);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR);

	glBindBuffer(GL_ARRAY_BUFFER, 0);


	glBindVertexArray(0);


	//Triangle
	glGenVertexArrays(1, &Vao_Triangle);
	glBindVertexArray(Vao_Triangle);

	glGenBuffers(1, &Vbo_Position_Triangle);
	glBindBuffer(GL_ARRAY_BUFFER, Vbo_Position_Triangle);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 2, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
	
	glVertexAttrib3f(AMC_ATTRIBUTE_COLOR, 1.0f, 1.0f, 1.0f);

	glBindVertexArray(0);


	//For Vertical Line
	glGenVertexArrays(1, &Vao_VerLine);
	glBindVertexArray(Vao_VerLine);

	glGenBuffers(1, &Vbo_Position_VerLine);
	glBindBuffer(GL_ARRAY_BUFFER, Vbo_Position_VerLine);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticalVertices), verticalVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

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
	mat4 modelViewMatrix;
	mat4 modelViewProjectionMatrix;
	mat4 translationMatrix;
	mat4 rotationMatrix;

	// OpenGL Drawing
	glLineWidth(1.0f);
	
	glBindVertexArray(Vao_Circle);
	modelViewMatrix = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();
	rotationMatrix = mat4::identity();
	translationMatrix = mat4::identity();
	
	tx = ((a*x1) + (b*x2) + (c*x3)) / summation;
	ty = ((a*yone) + (b*y2) + (c*y3)) / summation;

	translationMatrix = translate(tx+xctrans, ty-yctrans, -3.0f);
	rotationMatrix = rotate(AnglePyramid, 0.0f, 1.0f, 0.0f);
	modelViewMatrix = modelViewMatrix * translationMatrix * rotationMatrix;
	modelViewProjectionMatrix = PerspectiveProjectionMatrix * modelViewMatrix;

	glUniformMatrix4fv(MVPUniform, 1, GL_FALSE, modelViewProjectionMatrix);

	glDrawArrays(GL_LINE_LOOP, 0, 3000);

	glBindVertexArray(0);

	//Draw Triangle
	glBindVertexArray(Vao_Triangle);
	//glLineWidth(3.0f);
	modelViewMatrix = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();
	rotationMatrix = mat4::identity();
	translationMatrix = mat4::identity();

	translationMatrix = translate(xtrans, ytrans, -3.0f);
	rotationMatrix = rotate(AnglePyramid, 0.0f, 1.0f, 0.0f);
	modelViewMatrix = modelViewMatrix * translationMatrix * rotationMatrix;
	modelViewProjectionMatrix = PerspectiveProjectionMatrix * modelViewMatrix;

	glUniformMatrix4fv(MVPUniform, 1, GL_FALSE, modelViewProjectionMatrix);

	glDrawArrays(GL_LINE_LOOP, 0, 3);


	glBindVertexArray(0);

	//Draw Vertical Line
	glBindVertexArray(Vao_VerLine);
	modelViewMatrix = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();
	rotationMatrix = mat4::identity();
	translationMatrix = mat4::identity();

	translationMatrix = translate(0.0f, yltrans, -3.0f);
	modelViewMatrix = modelViewMatrix * translationMatrix;
	modelViewProjectionMatrix = PerspectiveProjectionMatrix * modelViewMatrix;

	glUniformMatrix4fv(MVPUniform, 1, GL_FALSE, modelViewProjectionMatrix);

	glDrawArrays(GL_LINES, 0, 3);
	
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

void uninitialize(void)
{
	//code

	// destroy vao
	if (Vao_Circle)
	{
		glDeleteVertexArrays(1, &Vao_Circle);
		Vao_Circle = 0;
	}

	if (Vbo_Position_Circle)
	{
		glDeleteBuffers(1, &Vbo_Position_Circle);
		Vbo_Position_Circle = 0;
	}


	if (Vbo_Position_VerLine)
	{
		glDeleteBuffers(1, &Vbo_Position_VerLine);
		Vbo_Position_VerLine = 0;
	}

	if (Vbo_Position_Triangle)
	{
		glDeleteBuffers(1, &Vbo_Position_Triangle);
		Vbo_Position_Triangle = 0;
	}


	if (Vbo_Color_Circle)
	{
		glDeleteBuffers(1, &Vbo_Color_Circle);
		Vbo_Color_Circle = 0;
	}

	if (Vbo_Color_VerLine)
	{
		glDeleteBuffers(1, &Vbo_Color_VerLine);
		Vbo_Color_VerLine = 0;
	}

	if (Vbo_Color_Triangle)
	{
		glDeleteBuffers(1, &Vbo_Color_Circle);
		Vbo_Color_Circle = 0;
	}

	if (Vao_Triangle)
	{
		glDeleteVertexArrays(1, &Vao_Triangle);
		Vao_Triangle = 0;
	}

	if (Vao_VerLine)
	{
		glDeleteVertexArrays(1, &Vao_VerLine);
		Vao_VerLine = 0;
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