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
	ATA_ATTRIBUTE_POSITION = 0,
	ATA_ATTRIBUTE_COLOR,
	ATA_ATTRIBUTE_NORMAL,
	ATA_ATTRIBUTE_TEXTURE0,
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
bool gbLight;

GLuint VertexShaderObject;
GLuint FragmentShaderObject;
GLuint ShaderProgramObject;

GLuint VaoAta;
GLuint Vbo_PositionAta;
GLuint Vbo_ColorAta;


GLuint MVPUniformAta;
GLuint samplerUniformAta; //for uniform(dynamic) texture data
GLuint textureMarbleStoneAta;
GLuint modelUniformAta, viewUniformAta, projectionUniformAta;
GLuint lightPositionUniformAta;
GLuint lKeyPressUniformAta;
GLuint La_uniform, Ld_uniform, Ls_uniform;
GLuint Ka_uniform, Kd_uniform, Ks_uniform;
GLuint material_shininess_uniform;

GLfloat lightAmbient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat lightDiffuse[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat lightSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat lightPosition[] = { 100.0f, 100.0f, 100.0f, 1.0f };

GLfloat material_ambient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat material_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat material_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat material_shininess = 50.0f;


mat4 PerspectiveProjectionMatrixAta;


GLfloat angleTriAta = 0.0f, angleSqAta = 0.0f;
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
							if (gbLight == false)
							{
								
								gbLight = true;
							}
							else
							{
								gbLight = false;
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

		//gluBuild2DMipmaps(GL_TEXTURE_2D, 3, width, height, GL_RGB,GL_UNSIGNED_BYTE,ImageData);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, ImageData);
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);
		
		SOIL_free_image_data(ImageData);
	
		return (bStatus);
}
		
	
	glDeleteTextures(1, texture);
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
	else // successfully created 4.1 context
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
		"in vec2 vTexture0_Coord;" \
		"out vec2 out_texture0_coord;" \
		"uniform mat4 u_model_matrix;" \
		"uniform mat4 u_view_matrix;" \
		"uniform mat4 u_projection_matrix;" \
		"uniform vec4 u_light_position;" \
		"uniform int u_lighting_enabled;" \
		"out vec3 transformed_normals;" \
		"out vec3 light_direction;" \
		"out vec3 viewer_vector;" \
		"in vec4 vColor;" \
		"out vec4 out_color;"
		"void main(void)" \
		"{" \
		"if(u_lighting_enabled == 1)" \
		"{" \
		"vec4 eye_coordinates = u_view_matrix* u_model_matrix * vPosition;" \
		"transformed_normals = mat3(u_view_matrix*u_model_matrix) * vNormal;" \
		"light_direction = vec3(u_light_position) - eye_coordinates.xyz;" \
		"viewer_vector = -eye_coordinates.xyz;" \
		"}" \
		"gl_Position = u_projection_matrix * u_view_matrix * u_model_matrix * vPosition;" \
		"out_color = vColor;" \
		"out_texture0_coord = vTexture0_Coord;" \
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
		"in vec3 transformed_normals;" \
		"in vec3 light_direction;" \
		"in vec3 viewer_vector;" \
		"in vec2 out_texture0_coord;" \
		"in vec4 out_color;"
		"uniform vec3 u_La;" \
		"uniform vec3 u_Ld;" \
		"uniform vec3 u_Ls;" \
		"uniform vec3 u_Ka;" \
		"uniform vec3 u_Kd;" \
		"uniform vec3 u_Ks;" \
		"uniform float u_material_shininess;" \
		"uniform int u_lighting_enabled;" \
		"uniform sampler2D u_texture0_sampler;" \
		"out vec4 FragColor;" \
		"void main(void)" \
		"{" \
		"vec3 phong_ads_color;"\
		"if(u_lighting_enabled == 1)" \
		"{" \
		"vec3 normalized_transformed_normals = normalize(transformed_normals);" \
		"vec3 normalized_light_direction = normalize(light_direction);" \
		"vec3 normalized_viewer_vector = normalize(viewer_vector);" \
		"float tn_dot_ld = max(dot(normalized_transformed_normals, normalized_light_direction), 0.0);" \
		"vec3 ambient = u_La * u_Ka;" \
		"vec3 diffuse = u_Ld * u_Kd * tn_dot_ld;" \
		"vec3 reflection_vector = reflect(-normalized_light_direction, normalized_transformed_normals);" \
		"vec3 specular = u_Ls * u_Ks * pow(max(dot(reflection_vector, normalized_viewer_vector),0.0), u_material_shininess);" \
		"phong_ads_color = ambient + diffuse + specular;" \
		"}" \
		"else" \
		"{" \
		"phong_ads_color = vec3(1.0, 1.0, 1.0);" \
		"}" \
		"FragColor = texture(u_texture0_sampler, out_texture0_coord) * out_color * vec4(phong_ads_color, 1.0);" \
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
	glBindAttribLocation(ShaderProgramObject, ATA_ATTRIBUTE_POSITION, "vPosition");
	glBindAttribLocation(ShaderProgramObject, ATA_ATTRIBUTE_TEXTURE0, "vTexture0_Coord");
	glBindAttribLocation(ShaderProgramObject, ATA_ATTRIBUTE_NORMAL, "vNormal");
	glBindAttribLocation(ShaderProgramObject, ATA_ATTRIBUTE_COLOR, "vColor");


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
	samplerUniformAta = glGetUniformLocation(ShaderProgramObject, "u_texture0_sampler");
	modelUniformAta = glGetUniformLocation(ShaderProgramObject, "u_model_matrix");
	viewUniformAta = glGetUniformLocation(ShaderProgramObject, "u_view_matrix");
	projectionUniformAta = glGetUniformLocation(ShaderProgramObject, "u_projection_matrix");
	lKeyPressUniformAta = glGetUniformLocation(ShaderProgramObject, "u_lighting_enabled"); //L/1 key pressed or not
	La_uniform = glGetUniformLocation(ShaderProgramObject, "u_La");
	Ld_uniform = glGetUniformLocation(ShaderProgramObject, "u_Ld");
	Ls_uniform = glGetUniformLocation(ShaderProgramObject, "u_Ls");
	lightPositionUniformAta = glGetUniformLocation(ShaderProgramObject, "u_light_position");

	//material ambient color intensity
	Ka_uniform = glGetUniformLocation(ShaderProgramObject, "u_Ka");
	Kd_uniform = glGetUniformLocation(ShaderProgramObject, "u_Kd");
	Ks_uniform = glGetUniformLocation(ShaderProgramObject, "u_Ks");
	//shininess of material
	material_shininess_uniform = glGetUniformLocation(ShaderProgramObject, "u_material_shininess");



	// *** vertices, colors, shader attribs, vbo, vao initializations ***
	const GLfloat cubeData[] = {
		// Top face
		1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		-1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		-1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,

		// Bottom face
		1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
		-1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		-1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
		1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,

		// Front face
		1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		-1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
		-1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
		1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,

		// Back face
		1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f,
		-1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		-1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
		1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f,

		// Right face
		1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f,
		1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,

		// Left face
		-1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
		-1.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
		-1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f
	};
	//For Vao
	glGenVertexArrays(1, &VaoAta);
	glBindVertexArray(VaoAta);
	//vbo for positions
	glGenBuffers(1, &Vbo_PositionAta);
	glBindBuffer(GL_ARRAY_BUFFER, Vbo_PositionAta);
	glBufferData(GL_ARRAY_BUFFER, 24*11*sizeof(float), cubeData, GL_STATIC_DRAW);

	glVertexAttribPointer(ATA_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 11*sizeof(float), (void*)(0*sizeof(float)));
	glEnableVertexAttribArray(ATA_ATTRIBUTE_POSITION);

	glVertexAttribPointer(ATA_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(ATA_ATTRIBUTE_NORMAL);

	glVertexAttribPointer(ATA_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(ATA_ATTRIBUTE_COLOR);

	glVertexAttribPointer(ATA_ATTRIBUTE_TEXTURE0, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(9 * sizeof(float)));
	glEnableVertexAttribArray(ATA_ATTRIBUTE_TEXTURE0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// ==================
	
	
	// set-up depth buffer
	glClearDepth(1.0f);
	// enable depth testing
	glEnable(GL_DEPTH_TEST);
	
	// We will always cull back faces for better performance
	glDisable(GL_CULL_FACE);

	// set background clearing color
	glClearColor(0.0f,0.0f,0.0f,0.0f); // black
	
	LoadGLTextures(&textureMarbleStoneAta,"marble.bmp");
	
	glEnable(GL_TEXTURE_2D);
	// set perspective matrix to identitu matrix
	PerspectiveProjectionMatrixAta = mat4::identity();	
	// resize
	resize(WIN_WIDTH, WIN_HEIGHT);
}

void resize(int width,int height)
{
    //code
	if(height==0)
		height=1;
		
	glViewport(0,0,(GLsizei)width,(GLsizei)height);

	PerspectiveProjectionMatrixAta = perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);

}

void display(void)
{
	//code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// start using OpenGL program object
	glUseProgram(ShaderProgramObject);
	if (gbLight == true)
	{
		glUniform1i(lKeyPressUniformAta, 1);
		//setting light's properties
		glUniform3fv(La_uniform, 1, lightAmbient);
		glUniform3fv(Ld_uniform, 1, lightDiffuse);
		glUniform3fv(Ls_uniform, 1, lightSpecular);

		glUniform4fv(lightPositionUniformAta, 1, lightPosition);

		//set material properties
		glUniform3fv(Ka_uniform, 1, material_ambient);
		glUniform3fv(Kd_uniform, 1, material_diffuse);
		glUniform3fv(Ks_uniform, 1, material_specular);
		glUniform1f(material_shininess_uniform, material_shininess);
	}
	else 
	{
		glUniform1i(lKeyPressUniformAta, 0);
	}
	//mat4 modelViewMatrixAta;
	mat4 modelMatrixAta;
	mat4 viewMatrixAta;
	mat4 modelViewProjectionMatrixAta;
	mat4 rotationMatrixAta;
	mat4 translationMatrixAta;

	// OpenGL Drawing

	modelMatrixAta = mat4::identity();
	viewMatrixAta = mat4::identity();
	modelViewProjectionMatrixAta = mat4::identity();
	rotationMatrixAta = mat4::identity();
	translationMatrixAta = mat4::identity();

	
	translationMatrixAta = translate(0.0f, 0.0f, -8.0f);
	rotationMatrixAta = rotate(angleSqAta, 1.0f, 0.0f, 0.0f);
	rotationMatrixAta *= rotate(angleSqAta, 0.0f, 1.0f, 0.0f);
	rotationMatrixAta *= rotate(angleSqAta, 0.0f, 0.0f, 1.0f);
	modelMatrixAta = modelMatrixAta * translationMatrixAta * rotationMatrixAta;
	
	//modelViewProjectionMatrixAta = PerspectiveProjectionMatrixAta * modelViewMatrixAta;

	glUniformMatrix4fv(modelUniformAta, 1, GL_FALSE, modelMatrixAta);
	glUniformMatrix4fv(viewUniformAta, 1, GL_FALSE, viewMatrixAta);
	glUniformMatrix4fv(projectionUniformAta, 1, GL_FALSE, PerspectiveProjectionMatrixAta);
	
	glBindVertexArray(VaoAta);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureMarbleStoneAta);
	glUniform1i(samplerUniformAta, 0);

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4); 
	glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 16, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 20, 4);

	glBindVertexArray(0);
	// stop using OpenGL program object
	glUseProgram(0);
	
	//to process buffered OpenGL Routines
	glXSwapBuffers(gpDisplay,gWindow);
}



void spin(void)
{
	angleTriAta = angleTriAta + 1.0f;
	if (angleTriAta >= 360)
	{
		angleTriAta = 0.0f;
	}

	angleSqAta = angleSqAta + 1.0f;
	if (angleSqAta >= 360)
	{
		angleSqAta = 0.0f;
	}
}

void uninitialize(void)
{
	//code
	// PYRAMID
	// destroy vao
	if (Vbo_PositionAta)
	{
		glDeleteBuffers(1, &Vbo_PositionAta);
		Vbo_PositionAta = 0;
	}

	if (Vbo_ColorAta)
	{
		glDeleteBuffers(1, &Vbo_ColorAta);
		Vbo_ColorAta = 0;
	}

	if (VaoAta)
	{
		glDeleteVertexArrays(1, &VaoAta);
		VaoAta = 0;
	}
	
	// detach vertex shader from shader program object
	glDetachShader(ShaderProgramObject, VertexShaderObject);
	// detach fragment  shader from shader program object
	glDetachShader(ShaderProgramObject, FragmentShaderObject);

	// delete vertex shader object
	glDeleteShader(VertexShaderObject);
	VertexShaderObject = 0;
	// delete fragment shader object
	glDeleteShader(FragmentShaderObject);
	FragmentShaderObject = 0;

	// delete shader program object
	glDeleteProgram(ShaderProgramObject);
	ShaderProgramObject = 0;

	// unlink shader program
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
