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
bool gbLighting = false;
bool gbAnimation = false;

GLuint VertexShaderObject;
GLuint FragmentShaderObject;
GLuint ShaderProgramObject;

GLuint Vao_Pyramid;
GLuint Vao_Cube;
GLuint Vbo_Position_Pyramid;
GLuint Vbo_Position_Cube;
GLuint Vbo_Color_Pyramid;
GLuint Vbo_Normals_Pyramid;
GLuint Vbo_Color_Cube;
GLuint MUniform;
GLuint VUniform;
GLuint PUniform;
GLuint materialShinyUniform;
GLuint light0PostionUniform;
GLuint light1PostionUniform;
GLuint lkeyPressedUniform;
GLuint L0a_uniform, L0d_uniform, L0s_uniform; 
GLuint L1a_uniform, L1d_uniform, L1s_uniform; 
GLuint Ka_uniform, Kd_uniform, Ks_uniform;

GLuint samplerUniform;

GLuint Texture_Kundali;
GLuint Texture_Stone;

mat4 PerspectiveProjectionMatrix;

GLfloat AnglePyramid = 0.0f; //angle of rotation of Trinagle
GLfloat AngleCube = 0.0f; //angle of rotation for Square

//lights
GLfloat light0Ambient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat light0Diffuse[] = { 1.0f,0.0f,0.0f,0.0f };
GLfloat light0Specular[] = { 1.0f, 0.0f, 0.0f, 0.0f };
GLfloat light0Position[] = { 2.0f, 2.0f, 0.0f, 1.0f };

GLfloat light1Ambient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat light1Diffuse[] = { 0.0f,0.0f,1.0f,0.0f };
GLfloat light1Specular[] = { 0.0f, 0.0f, 1.0f, 0.0f };
GLfloat light1Position[] = { -2.0f, 2.0f, 0.0f, 1.0f };

GLfloat material_ambient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat material_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat material_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat material_shininess = 50.0f;

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
						
						case XK_L:
						case XK_l:
							if (gbLighting == false)
							{
								
								gbLighting = true;
							}
							else
							{
								gbLighting = false;
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
		"in vec3 vNormal;" \
		"uniform mat4 u_model_matrix;" \
		"uniform mat4 u_view_matrix;" \
		"uniform mat4 u_projection_matrix;" \
		"uniform int u_lkeypress;" \
		"uniform vec3 u_L0a;" \
		"uniform vec3 u_L0d;" \
		"uniform vec3 u_L0s;" \
		"uniform vec4 u_light0_position;" \
		"uniform vec3 u_L1a;" \
		"uniform vec3 u_L1d;" \
		"uniform vec3 u_L1s;" \
		"uniform vec4 u_light1_position;" \
		"uniform vec3 u_Ka;" \
		"uniform vec3 u_Kd;" \
		"uniform vec3 u_Ks;" \
		"uniform float u_material_shininess;" \
		"out vec3 phong_ads_light;" \
		"void main(void)" \
		"{" \
		"if(u_lkeypress == 1)" \
		"{"\
		"vec4 eye_coordinates = u_view_matrix* u_model_matrix * vPosition;" \
		"vec3 transformed_normals = normalize(mat3(u_view_matrix*u_model_matrix) * vNormal);" \
		"vec3 light0_direction = normalize(vec3(u_light0_position) - eye_coordinates.xyz);" \
		"vec3 light1_direction = normalize(vec3(u_light1_position) - eye_coordinates.xyz);" \
		"float tn_dot_ld0 = max(dot(transformed_normals, light0_direction), 0.0);" \
		"float tn_dot_ld1 = max(dot(transformed_normals, light1_direction), 0.0);" \
		"vec3 ambient0 = u_L0a * u_Ka;" \
		"vec3 ambient1 = u_L1a * u_Ka;" \
		"vec3 diffuse0 = u_L0d * u_Kd * tn_dot_ld0;" \
		"vec3 diffuse1 = u_L1d * u_Kd * tn_dot_ld1;" \
		"vec3 reflection_vector0 = reflect(-light0_direction, transformed_normals);" \
		"vec3 reflection_vector1 = reflect(-light1_direction, transformed_normals);" \
		"vec3 viewer_vector = normalize(-eye_coordinates.xyz);" \
		"vec3 specular0 = u_L0s * u_Ks * pow(max(dot(reflection_vector0, viewer_vector),0.0), u_material_shininess);" \
		"vec3 specular1 = u_L1s * u_Ks * pow(max(dot(reflection_vector1, viewer_vector),0.0), u_material_shininess);" \
		"phong_ads_light = ambient0 + ambient1 + diffuse0 + diffuse1 + specular0 + specular1;"
		"}" \
		"else" \
		"{"
		"phong_ads_light = vec3(1.0, 1.0, 1.0);" \
		"}"\
		"gl_Position = u_projection_matrix * u_view_matrix * u_model_matrix * vPosition;" \
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
		"in vec3 phong_ads_light;" \
		"out vec4 FragColor;" \
		"void main(void)" \
		"{" \
		"FragColor = vec4(phong_ads_light,1.0);" \
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
	glBindAttribLocation(ShaderProgramObject, AMC_ATTRIBUTE_NORMAL, "vNormal");
	

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
	MUniform = glGetUniformLocation(ShaderProgramObject, "u_model_matrix");
	VUniform = glGetUniformLocation(ShaderProgramObject, "u_view_matrix");
	PUniform = glGetUniformLocation(ShaderProgramObject, "u_projection_matrix");
	lkeyPressedUniform = glGetUniformLocation(ShaderProgramObject, "u_lkeypress"); //L/1 key pressed or not
	L0a_uniform = glGetUniformLocation(ShaderProgramObject, "u_L0a");
	L0d_uniform = glGetUniformLocation(ShaderProgramObject, "u_L0d");
	L0s_uniform = glGetUniformLocation(ShaderProgramObject, "u_L0s");
	light0PostionUniform = glGetUniformLocation(ShaderProgramObject, "u_light0_position");

	//light1
	L1a_uniform = glGetUniformLocation(ShaderProgramObject, "u_L1a");
	L1d_uniform = glGetUniformLocation(ShaderProgramObject, "u_L1d");
	L1s_uniform = glGetUniformLocation(ShaderProgramObject, "u_L1s");
	light1PostionUniform = glGetUniformLocation(ShaderProgramObject, "u_light1_position");

	//material
	Ka_uniform = glGetUniformLocation(ShaderProgramObject, "u_Ka");
	Kd_uniform = glGetUniformLocation(ShaderProgramObject, "u_Kd");
	Ks_uniform = glGetUniformLocation(ShaderProgramObject, "u_Ks");
	materialShinyUniform = glGetUniformLocation(ShaderProgramObject, "u_material_shininess");

	const GLfloat pyramidVertices[] =
	{ // front
	0.0f, 1.0f, 0.0f,
	-1.0f, -1.0f, 1.0f,
	1.0f, -1.0f, 1.0f,

	// right
0.0f, 1.0f, 0.0f,
1.0f, -1.0f, 1.0f,
1.0f, -1.0f, -1.0f,

// back
0.0f, 1.0f, 0.0f,
1.0f, -1.0f, -1.0f,
-1.0f, -1.0f, -1.0f,

// left
0.0f, 1.0f, 0.0f,
-1.0f, -1.0f, -1.0f,
-1.0f, -1.0f, 1.0f
	};

	const GLfloat pyramidNormals[] = {
	0.0f, 0.447214f, 0.894427f,// front-top
		0.0f, 0.447214f, 0.894427f,// front-left
		0.0f, 0.447214f, 0.894427f,// front-right

		0.894427f, 0.447214f, 0.0f, // right-top
		0.894427f, 0.447214f, 0.0f, // right-left
		0.894427f, 0.447214f, 0.0f, // right-right

		0.0f, 0.447214f, -0.894427f, // back-top
		0.0f, 0.447214f, -0.894427f, // back-left
		0.0f, 0.447214f, -0.894427f, // back-right

		-0.894427f, 0.447214f, 0.0f, // left-top
		-0.894427f, 0.447214f, 0.0f, // left-left
		-0.894427f, 0.447214f, 0.0f // left-right
	};


	//For Triangle
	glGenVertexArrays(1, &Vao_Pyramid);
	glBindVertexArray(Vao_Pyramid);

	glGenBuffers(1, &Vbo_Position_Pyramid);
	glBindBuffer(GL_ARRAY_BUFFER, Vbo_Position_Pyramid);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidVertices), pyramidVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &Vbo_Normals_Pyramid);
	glBindBuffer(GL_ARRAY_BUFFER, Vbo_Normals_Pyramid);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidNormals), pyramidNormals, GL_STATIC_DRAW);

	glVertexAttribPointer(AMC_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(AMC_ATTRIBUTE_NORMAL);

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
	mat4 rotX, rotY, rotZ;
	mat4 translationMatrix;
	mat4 scaleMatrix;
	// OpenGL Drawing

	if (gbLighting == true) {
		glUniform1i(lkeyPressedUniform, 1);
		//setting light's properties
		//light0
		glUniform3fv(L0a_uniform, 1, light0Ambient);
		glUniform3fv(L0d_uniform, 1, light0Diffuse);
		glUniform3fv(L0s_uniform, 1, light0Specular);
		glUniform4fv(light0PostionUniform, 1, light0Position);

		glUniform3fv(L1a_uniform, 1, light1Ambient);
		glUniform3fv(L1d_uniform, 1, light1Diffuse);
		glUniform3fv(L1s_uniform, 1, light1Specular);
		glUniform4fv(light1PostionUniform, 1, light1Position);

		//set material properties
		glUniform3fv(Ka_uniform, 1, material_ambient);
		glUniform3fv(Kd_uniform, 1, material_diffuse);
		glUniform3fv(Ks_uniform, 1, material_specular);
		glUniform1f(materialShinyUniform, material_shininess);

		//setting materia properties
	}
	else {
		glUniform1i(lkeyPressedUniform, 0);
	}
	mat4 modelMatrix = mat4::identity();
	mat4 viewMatrix = mat4::identity();
	mat4 rotationMatrix = mat4::identity();

	modelMatrix = vmath::translate(0.0f, 0.0f, -5.0f);
	rotationMatrix = vmath::rotate(AnglePyramid, 0.0f, 1.0f, 0.0f);
	modelMatrix = modelMatrix * rotationMatrix;
	// Pass above matrices to shaders

	glUniformMatrix4fv(MUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(VUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(PUniform, 1, GL_FALSE, PerspectiveProjectionMatrix);

	glBindVertexArray(Vao_Pyramid);

	glDrawArrays(GL_TRIANGLES, 0, 12);

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
	if (Vbo_Color_Pyramid)
	{
		glDeleteBuffers(1, &Vbo_Color_Pyramid);
		Vbo_Color_Pyramid = 0;
	}
	if (Vbo_Normals_Pyramid)
	{
		glDeleteBuffers(1, &Vbo_Normals_Pyramid);
		Vbo_Normals_Pyramid = 0;
	}


	if (Vbo_Position_Pyramid)
	{
		glDeleteBuffers(1, &Vbo_Position_Pyramid);
		Vbo_Position_Pyramid = 0;
	}

	if (Vao_Pyramid)
	{
		glDeleteVertexArrays(1, &Vao_Pyramid);
		Vao_Pyramid = 0;
	}

	if (Vbo_Color_Pyramid)
	{
		glDeleteBuffers(1, &Vbo_Color_Pyramid);
		Vbo_Color_Pyramid = 0;
	}

	if (Vbo_Position_Cube)
	{
		glDeleteBuffers(1, &Vbo_Position_Cube);
		Vbo_Position_Cube = 0;
	}

	if (Vao_Cube)
	{
		glDeleteVertexArrays(1, &Vao_Cube);
		Vao_Cube = 0;
	}
	
	glUseProgram(0);

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
