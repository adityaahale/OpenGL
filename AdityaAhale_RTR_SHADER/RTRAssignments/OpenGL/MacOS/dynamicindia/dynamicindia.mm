#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>

#import <QuartzCore/CVDisplayLink.h>
#import <OpenGL/gl3.h>
#import <OpenGL/gl3ext.h>
#import "vmath.h"

using namespace vmath;
enum{
    ATA_ATTRIBUTE_POSITION = 0,
    ATA_ATTRIBUTE_COLOR,
    ATA_ATTRIBUTE_NORMAL,
    ATA_ATTRIBUTE_TEXTURE0
};

// 'C' style global function declarations
CVReturn MyDisplayLinkCallback(CVDisplayLinkRef, const CVTimeStamp*, const CVTimeStamp*,
                               CVOptionFlags, CVOptionFlags *, void *);
//global variables
FILE *gpAtaFile =NULL;

//interface declarations
@interface AppDelegate : NSObject <NSApplicationDelegate, NSWindowDelegate>
@end

@interface GLView : NSOpenGLView
@end

//Entry-point function

int main(int argc, const char *argv[])
{
	NSAutoreleasePool *pPool_ata = [[NSAutoreleasePool alloc]init];
    NSApp = [NSApplication sharedApplication];
    [NSApp setDelegate:[[AppDelegate alloc]init]];
    [NSApp run];
    
    [pPool_ata release];
    printf("In main");    

    return (0);
}


//interface implementations
@implementation AppDelegate
{
@private
    NSWindow *window;
    GLView *view_ata;
}


- (void) applicationDidFinishLaunching:(NSNotification *)aNotification
{
    //Code for log file
    NSBundle *mainBundleAta = [NSBundle mainBundle];
    NSString *appDirNameAta = [mainBundleAta bundlePath];
    NSString *parentDirPathAta = [appDirNameAta stringByDeletingLastPathComponent];
    NSString *logFileNameWithPathAta = [NSString stringWithFormat:@"%@/Log.txt",parentDirPathAta];
    const char *pszLogFileNameWithPathAta = [logFileNameWithPathAta cStringUsingEncoding:NSASCIIStringEncoding];
    
    gpAtaFile=fopen("Log.txt","w");
    if(gpAtaFile==NULL)
    {
        printf("Can not create log file. \n Exiting...\n");
        [self release];
        [NSApp terminate:self];
    }
    //file created successfully
    fprintf(gpAtaFile, "Program is started successfully");
    
    //window
    NSRect win_rect;
    win_rect = NSMakeRect(0.0,0.0,800.0,600.0);
    
        //create simple window
    window = [[NSWindow alloc] initWithContentRect:win_rect
                                         styleMask:NSWindowStyleMaskTitled|NSWindowStyleMaskClosable|
              NSWindowStyleMaskMiniaturizable|NSWindowStyleMaskResizable
                                           backing:NSBackingStoreBuffered defer:NO];
    [window setTitle:@"DynamicIndia"];
    [window center];
    view_ata=[[GLView alloc]initWithFrame:win_rect];
    
    [window setContentView:view_ata];
    [window setDelegate:self];
    [window makeKeyAndOrderFront:self];
    
}

- (void)applicationWillTerminate:(NSNotification *)notification
{
    fprintf(gpAtaFile, "Program terminated successfully.");
    if(gpAtaFile)
    {
        fclose(gpAtaFile);
        gpAtaFile=NULL;
    }
    
}


- (void)windowWillClose:(NSNotification *)notification
{
        //code
    [NSApp terminate:self];
}

-(void) dealloc
{
       //code
    [view_ata release];
    [window release];
    [super dealloc];
}

@end

@implementation GLView
{
@private
    CVDisplayLinkRef displayLink;
    
    GLuint vertexShaderObject_ata;
    GLuint fragmentShaderObject_ata;
    GLuint shaderProgramObject_ata;
    
    GLuint vao_horizontal_ata;
    GLuint vao_vertical_ata;
    GLuint vao_triline_ata;
    GLuint vao_N;
    GLuint vao_A;
    GLuint vao_D;
    GLuint vao_I;
    GLuint vao_plane;
    GLuint vao_iaf;
    GLuint vao_smoke;
    	
    GLuint vbo_position;
    GLuint vbo_position_line;
    GLuint vbo_position_horizontal;
    GLuint vbo_position_i;
    GLuint vbo_position_d;
    GLuint vbo_position_n;
    GLuint vbo_position_a;
	GLuint vbo_position_plane;
	GLuint vbo_position_smoke;
	GLuint vbo_position_iaf;
	
    GLuint vbo_color;
    GLuint vbo_color_triline;
    GLuint vbo_color_horizontal;
    GLuint vbo_color_i;
    GLuint vbo_color_d;
    GLuint vbo_color_n;
    GLuint vbo_color_a;
	GLuint vbo_color_smoke;
	GLuint vbo_color_iaf;
	
    GLuint mvpUniform_ata;
    GLuint alphaUniform;
    
    GLfloat angleTriangleAta;
    GLfloat angleSquareAta;
    GLfloat angleDHAta;
    GLfloat rot , itrans, ntrans, iitrans, atrans;
    GLfloat xtrans, ytrans, xctrans, yctrans, yltrans;
    GLfloat xTritrans, xFlagtrans, Topangle, Bottomangle, xIAFtrans,
    xPlanetrans, xPlane1trans, xPlane2trans, yPlane1trans, yPlane2trans;

    GLfloat alphaBlend;
    
    float radius;			//1,-1	0,1  -1,-1 
    float area;
    float a, b, c, tx, ty;
    float x1, x2, x3, yone, y2, y3;
    float s, summation;

    vmath::mat4 perspectiveProjectionMatrix_ata;
}


-(id)initWithFrame:(NSRect)frame;
{
    //code

    
	self=[super initWithFrame:frame];

    xTritrans = -6.5f;
    xFlagtrans = -6.5f;
    Topangle = 0.0f;
    Bottomangle = 0.0f;
    xIAFtrans = -8.3f;
    xPlanetrans = -8.3f;
    xPlane1trans = -8.3f;
    xPlane2trans = -8.3f;
    yPlane1trans = 2.0f;
    yPlane2trans = -2.0f;
    rot = 0.0f;
    itrans = -8.0f;
    ntrans = 7.0f;
    iitrans = -7.0f;
    atrans = 8.0f;
    alphaBlend = 0.0f;
    
    
    if(self)
    {
        [[self window] setContentView:self];
        
    }
    
    NSOpenGLPixelFormatAttribute attrs[]=
    {
        NSOpenGLPFAOpenGLProfile,
        NSOpenGLProfileVersion4_1Core,
        //specify about displayid
        NSOpenGLPFAScreenMask, CGDisplayIDToOpenGLDisplayMask(kCGDirectMainDisplay),
        NSOpenGLPFANoRecovery,
        NSOpenGLPFAAccelerated,
        NSOpenGLPFAColorSize, 24,
        NSOpenGLPFADepthSize, 24,
        NSOpenGLPFAAlphaSize, 8,
        NSOpenGLPFADoubleBuffer,
        0}; //end of array elements
    
    NSOpenGLPixelFormat *pixelFormat = [[[NSOpenGLPixelFormat alloc]initWithAttributes:attrs]autorelease];
    if(pixelFormat == nil)
    {

        fprintf(gpAtaFile, "No valid OpenGL pixel format is available. Exit...");
        [self release];
        [NSApp terminate:self];
    }
    
    NSOpenGLContext *glContext = [[[NSOpenGLContext alloc]initWithFormat:pixelFormat shareContext:nil]autorelease];
    
    [self setPixelFormat:pixelFormat];
    [self setOpenGLContext:glContext];
    return (self);
}

-(void)prepareOpenGL
{
    //OpenGL info
    fprintf(gpAtaFile, "OpenGL version: %s\n", glGetString(GL_VERSION));
    fprintf(gpAtaFile, "GLSL version  : %s\n",glGetString(GL_SHADING_LANGUAGE_VERSION));
    GLint swapInt = 1;
    [[self openGLContext]setValues:&swapInt forParameter:NSOpenGLCPSwapInterval];
    //-----VERTEX SHADER----
    //Create vertex shader. Vertex shader specialist
    vertexShaderObject_ata = glCreateShader(GL_VERTEX_SHADER);
    
    const GLchar *vertexShaderSourceCodeAta =         
    "#version 410 core" \
    "\n" \
    "in vec4 vPosition;" \
    "in vec4 vColor;" \
    "out vec4 out_color;" \
    "uniform mat4 u_mvp_matrix;" \
    "void main(void)" \
    "{" \
    "gl_Position = u_mvp_matrix * vPosition;" \
    "out_color = vColor;" \
    "}";
    
    glShaderSource(vertexShaderObject_ata, 1, (const GLchar**)&vertexShaderSourceCodeAta, NULL); //NULL is for NULL terminated source code string
    
    //compile vertex shader
    glCompileShader(vertexShaderObject_ata);
    
    GLint iInfoLogLengthAta = 0;
    GLint iShaderCompiledStatusAta = 0;
    char* szInfoLogAta = NULL;
    glGetShaderiv(vertexShaderObject_ata, GL_COMPILE_STATUS, &iShaderCompiledStatusAta);
    if (iShaderCompiledStatusAta == GL_FALSE) {
        glGetShaderiv(vertexShaderObject_ata, GL_INFO_LOG_LENGTH, &iInfoLogLengthAta);
        if (iInfoLogLengthAta > 0) {
            szInfoLogAta = (char*)malloc(iInfoLogLengthAta);
            if (szInfoLogAta != NULL) {
                GLsizei writtenAta;
                glGetShaderInfoLog(vertexShaderObject_ata, iInfoLogLengthAta, &writtenAta, szInfoLogAta);
                fprintf(gpAtaFile, "Vertex shader compilation log:%s",
                        szInfoLogAta);
                free(szInfoLogAta);
                [self release];
                [NSApp terminate:self];
            }
        }
    }
    //-----FRAGMENT SHADER----
    //Create fragment shader. Fragment shader specialist
    fragmentShaderObject_ata = glCreateShader(GL_FRAGMENT_SHADER);
    
    //source code of fragment shader
    const GLchar *fragmentShaderSourceCodeAta =      
    "#version 410 core" \
    "\n" \
    "in vec4 out_color;"
    "uniform float alpha; " \
    "out vec4 FragColor;" \
    "void main(void)" \
    "{" \
    "FragColor = out_color;" \
    "FragColor.a= alpha;" \
    "}";
    glShaderSource(fragmentShaderObject_ata, 1, (const GLchar**)&fragmentShaderSourceCodeAta, NULL);
    
    //compile fragment shader
    glCompileShader(fragmentShaderObject_ata);
    
    glGetShaderiv(fragmentShaderObject_ata, GL_COMPILE_STATUS, &iShaderCompiledStatusAta);
    if (iShaderCompiledStatusAta == GL_FALSE) {
        glGetShaderiv(fragmentShaderObject_ata, GL_INFO_LOG_LENGTH, &iInfoLogLengthAta);
        if (iInfoLogLengthAta > 0) {
            szInfoLogAta = (char*)malloc(iInfoLogLengthAta);
            if (szInfoLogAta != NULL) {
                GLsizei writtenAta = 0;
                glGetShaderInfoLog(fragmentShaderObject_ata, iInfoLogLengthAta, &writtenAta, szInfoLogAta);
                fprintf(gpAtaFile, "Fragment shader compilation log:%s",
                        szInfoLogAta);
                free(szInfoLogAta);
                [self release];
                [NSApp terminate:self];
        }
    }
    }
    
    //Create shader program
    shaderProgramObject_ata = glCreateProgram();
    
    //attach shaders to the program
    glAttachShader(shaderProgramObject_ata, vertexShaderObject_ata);
    
    glAttachShader(shaderProgramObject_ata, fragmentShaderObject_ata);
    
    glBindAttribLocation(shaderProgramObject_ata, ATA_ATTRIBUTE_POSITION, "vPosition");
    glBindAttribLocation(shaderProgramObject_ata, ATA_ATTRIBUTE_COLOR, "vColor");
    
    //Link shader program
    glLinkProgram(shaderProgramObject_ata);
   
    GLint iShaderProgramLinkStatus = 0;
    glGetProgramiv(shaderProgramObject_ata, GL_LINK_STATUS, &iShaderProgramLinkStatus);
    if (iShaderProgramLinkStatus == GL_FALSE) {
        glGetProgramiv(shaderProgramObject_ata, GL_INFO_LOG_LENGTH, &iInfoLogLengthAta);
        if (iInfoLogLengthAta > 0) {
            szInfoLogAta = (char*)malloc(iInfoLogLengthAta);
            if (szInfoLogAta != NULL) {
                GLsizei writtenAta;
                glGetProgramInfoLog(shaderProgramObject_ata, iInfoLogLengthAta, &writtenAta, szInfoLogAta);
                fprintf(gpAtaFile, "Shader program linking log:%s", szInfoLogAta);
                [self release];
                [NSApp terminate:self];
            }
        }
    }
	
    mvpUniform_ata = glGetUniformLocation(shaderProgramObject_ata, "u_mvp_matrix");
    alphaUniform = glGetUniformLocation(shaderProgramObject_ata, "alpha");
    
    // Vertices, colors, shader attribs, vbo, vao initializations
	const GLfloat triVertices[] ={
		-0.2f, 0.2f,0.0f,
	0.2f, 0.2f,0.0f,
	-0.2f, 0.15f,0.0f,
	0.2f, 0.15f,0.0f,
	-0.2f, 0.1f,0.0f,
	0.2f, 0.1f,0.0f
	};

	//TriLine
	glGenVertexArrays(1, &vao_triline_ata);
	glBindVertexArray(vao_triline_ata);

	glGenBuffers(1, &vbo_position_line);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position_line);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triVertices), triVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(ATA_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(ATA_ATTRIBUTE_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	const GLfloat flagColor[] = {
		1.0f, 0.6f, 0.2f,
	1.0f, 0.6f, 0.2f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	0.07058f, 0.54f, 0.0274f,
	0.07058f, 0.54f, 0.0274f

	};
	
	glGenBuffers(1, &vbo_color_triline);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_color_triline);
    glBufferData(GL_ARRAY_BUFFER, sizeof(flagColor), flagColor, GL_STATIC_DRAW);

    glVertexAttribPointer(ATA_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(ATA_ATTRIBUTE_COLOR);
	
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0); 
	

	glBindVertexArray(0);// done with triflag lines
	
	
    const GLfloat horLineVerticesAta[] = {
			-1.0f,0.0f,0.0f,
		1.0f,0.0f,0.0f
    };
    
    //create a vao
    glGenVertexArrays(1, &vao_horizontal_ata);
    glBindVertexArray(vao_horizontal_ata);
    
    glGenBuffers(1, &vbo_position_horizontal);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_position_horizontal);
    glBufferData(GL_ARRAY_BUFFER, sizeof(horLineVerticesAta), horLineVerticesAta, GL_STATIC_DRAW);
    
    glVertexAttribPointer(ATA_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(ATA_ATTRIBUTE_POSITION);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    //vbo for horizontal colors
    const GLfloat horLineColorsAta[] = {
		1.0f,0.0f,0.0f,
		1.0f,0.0f,0.0f
	};

    glGenBuffers(1, &vbo_color_horizontal);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_color_horizontal);
    glBufferData(GL_ARRAY_BUFFER, sizeof(horLineColorsAta), horLineColorsAta, GL_STATIC_DRAW);

    glVertexAttribPointer(ATA_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(ATA_ATTRIBUTE_COLOR);
	
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);  //done with horizontal vao
    
    
    //square
    const GLfloat verLineVerticesAta[] = {
		0.0f,1.0f,0.0f,
		0.0f,-1.0f,0.0f
	};

	const GLfloat verColor[] = {
		0.0f,1.0f,0.0f,
		0.0f,1.0f,0.0f
	};
	
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glGenVertexArrays(1, &vao_vertical_ata);
    glBindVertexArray(vao_vertical_ata);

    glGenBuffers(1, &vbo_position);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verLineVerticesAta), verLineVerticesAta, GL_STATIC_DRAW);

    glVertexAttribPointer(ATA_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(ATA_ATTRIBUTE_POSITION);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	glGenBuffers(1, &vbo_color);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_color);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verColor), verColor, GL_STATIC_DRAW);

	glVertexAttribPointer(ATA_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(ATA_ATTRIBUTE_COLOR);
	
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0); //done with vertical vao
	
	//D vao
	const GLfloat D[] = { 
		-0.5f, 1.0f,0.0f,
	- 0.5f, -1.0f,0.0f,
	- 0.7f, 1.0f,0.0f,
	0.3f, 1.0f,0.0f,
	- 0.7f, -1.0f,0.0f,
	0.3, -1.0f,0.0f,
	0.3f, 1.0f,0.0f,
	0.3f, -1.0f,0.0f 
	};

	
	GLfloat triColor[] = {
	1.0f, 0.6f, 0.2f,
		0.07058f, 0.54f, 0.0274f,
		1.0f, 0.6f, 0.2f,
		1.0f, 0.6f, 0.2f,
		0.07058f, 0.54f, 0.0274f,
		0.07058f, 0.54f, 0.0274f,
		1.0f, 0.6f, 0.2f,
		0.07058f, 0.54f, 0.0274f
	};
	
	//create D  vao
	glGenVertexArrays(1, &vao_D);
	glBindVertexArray(vao_D);

	glGenBuffers(1, &vbo_position_d);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position_d);
	glBufferData(GL_ARRAY_BUFFER, sizeof(D), D, GL_STATIC_DRAW);

	glVertexAttribPointer(ATA_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(ATA_ATTRIBUTE_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &vbo_color_d);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_color_d);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triColor), triColor, GL_STATIC_DRAW);

	glVertexAttribPointer(ATA_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(ATA_ATTRIBUTE_COLOR);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
	
	const GLfloat I[] = {
		0.0f,1.0f,0.0f,
		0.0f,-1.0f,0.0f
	};
	
	//I 
	glGenVertexArrays(1, &vao_I);
	glBindVertexArray(vao_I);

	glGenBuffers(1, &vbo_position_i);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position_i);
	glBufferData(GL_ARRAY_BUFFER, sizeof(I), I, GL_STATIC_DRAW);

	glVertexAttribPointer(ATA_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(ATA_ATTRIBUTE_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &vbo_color_i);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_color_i);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triColor), triColor, GL_STATIC_DRAW);

	glVertexAttribPointer(ATA_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(ATA_ATTRIBUTE_COLOR);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0); //done with circle vao
	
	//N
	
	const GLfloat N[] = {
		-0.5f, 1.0f,0.0f,
	-0.5f, -1.0f,0.0f,
	-0.5f, 1.0f,0.0f,
	0.5f, -1.0f,0.0f,
	0.5f, -1.0f,0.0f,
	0.5f, 1.0f,0.0f,
	0.5f,1.0f,0.0f
	};
	
	GLfloat NColor[] = {
	1.0f, 0.6f, 0.2f,
	0.07058f, 0.54f, 0.0274f,
	1.0f, 0.6f, 0.2f,
	0.07058f, 0.54f, 0.0274f,
	0.07058f, 0.54f, 0.0274f,
	1.0f, 0.6f, 0.2f
	};
	
	//N vao
	glGenVertexArrays(1, &vao_N);
	glBindVertexArray(vao_N);

	glGenBuffers(1, &vbo_position_n);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position_n);
	glBufferData(GL_ARRAY_BUFFER, sizeof(N), N, GL_STATIC_DRAW);

	glVertexAttribPointer(ATA_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(ATA_ATTRIBUTE_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &vbo_color_n);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_color_n);
	glBufferData(GL_ARRAY_BUFFER, sizeof(NColor), NColor, GL_STATIC_DRAW);

	glVertexAttribPointer(ATA_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(ATA_ATTRIBUTE_COLOR);

	glBindBuffer(GL_ARRAY_BUFFER, 0);


	glBindVertexArray(0);//done with N vao

	
	
	GLfloat A[] = {
		0.0f, 1.0f,0.0f,
		-0.5f, -1.0f,0.0f,
		0.0f, 1.0f,0.0f,
		0.0f, 1.0f,0.0f,
		0.5f, -1.0f,0.0f,
		0.0f, 1.0f,0.0f
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
	
	//A vao
	glGenVertexArrays(1, &vao_A);
	glBindVertexArray(vao_A);

	glGenBuffers(1, &vbo_position_a);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position_a);
	glBufferData(GL_ARRAY_BUFFER, sizeof(A), A, GL_STATIC_DRAW);

	glVertexAttribPointer(ATA_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(ATA_ATTRIBUTE_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &vbo_color_a);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_color_a);
	glBufferData(GL_ARRAY_BUFFER, sizeof(AColor), AColor, GL_STATIC_DRAW);

	glVertexAttribPointer(ATA_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(ATA_ATTRIBUTE_COLOR);

	glBindBuffer(GL_ARRAY_BUFFER, 0);


	glBindVertexArray(0); //done with square vao
	
	const GLfloat plane[] = {

		-0.2, 0.05f,
		-0.2, -0.05f,
		0.4, -0.05f,
		0.4, 0.05f,
				-0.1f, 0.05f,
			-0.4f, 0.3f,
			-0.2f, 0.05f,


			-0.1f, -0.05f,
			-0.4f, -0.3f,
			-0.2f, -0.05f,


			0.5f, 0.0f,
			0.4f, 0.05f,
			0.4f, -0.05f,

			0.2f, 0.05f,
			-0.3f, 0.4f,
			0.0f, 0.05f,

			0.0f, -0.05f,
			-0.3f, -0.4f,
			0.2f, -0.05f
	
	};

	//plane 

	glGenVertexArrays(1, &vao_plane);
	glBindVertexArray(vao_plane);

	glGenBuffers(1, &vbo_position_plane);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position_plane);
	glBufferData(GL_ARRAY_BUFFER, sizeof(plane), plane, GL_STATIC_DRAW);

	glVertexAttribPointer(ATA_ATTRIBUTE_POSITION, 2, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(ATA_ATTRIBUTE_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//glGenBuffers(1, &Vbo_Color);
	//glBindBuffer(GL_ARRAY_BUFFER, Vbo_Color);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(planeColor), planeColor, GL_STATIC_DRAW);

	//glVertexAttribPointer(ATA_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glVertexAttrib3f(ATA_ATTRIBUTE_COLOR, (float)186 / 255, (float)226 / 255, (float)238 / 255);

	//glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR);

	glBindBuffer(GL_ARRAY_BUFFER, 0);


	glBindVertexArray(0);

	const GLfloat IAF[] = {
		0.01f, 0.04f,
	0.01f, -0.04f,

	0.05f, 0.04f,
	0.03f, -0.04f,
	0.05f, 0.04f,
	0.07f, -0.04f,


	0.09f, 0.04f,
	0.09f, -0.04f,
	0.09f, 0.04f,
	0.12f, 0.04,
	0.09f, 0.01f,
	0.12f, 0.01f,

	};

	const GLfloat IAFColor[] = {
		0.0f,0.0f,0.0f,
		0.0f,0.0f,0.0f,
		0.0f,0.0f,0.0f,
		0.0f,0.0f,0.0f,
		0.0f,0.0f,0.0f,
		0.0f,0.0f,0.0f,
		0.0f,0.0f,0.0f,
		0.0f,0.0f,0.0f,
		0.0f,0.0f,0.0f,
		0.0f,0.0f,0.0f,
		0.0f,0.0f,0.0f,
		0.0f,0.0f,0.0f,
		0.0f,0.0f,0.0f,
		0.0f,0.0f,0.0f,
		0.0f,0.0f,0.0f,
		0.0f,0.0f,0.0f,
		0.0f,0.0f,0.0f,
		0.0f,0.0f,0.0f,
		0.0f,0.0f,0.0f,
		0.0f,0.0f,0.0f,
		0.0f,0.0f,0.0f,
		0.0f,0.0f,0.0f,
		0.0f,0.0f,0.0f,
		0.0f,0.0f,0.0f,
		0.0f,0.0f,0.0f,
		0.0f,0.0f,0.0f,
		0.0f,0.0f,0.0f,
		0.0f,0.0f,0.0f,
		0.0f,0.0f,0.0f,
		0.0f,0.0f,0.0f,
		0.0f,0.0f,0.0f,
	};

	GLfloat smoke[] = {
		-0.23, 0.08f,
		-4.0, 0.08f,
		-0.23f, 0.05f,
		-4.0f, 0.05f,
		-0.23f, 0.02f,
		-4.0f, 0.02f
	};
	
	//Smoke
	glGenVertexArrays(1, &vao_smoke);
	glBindVertexArray(vao_smoke);

	glGenBuffers(1, &vbo_position_smoke);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position_smoke);
	glBufferData(GL_ARRAY_BUFFER, sizeof(smoke), smoke, GL_STATIC_DRAW);

	glVertexAttribPointer(ATA_ATTRIBUTE_POSITION, 2, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(ATA_ATTRIBUTE_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &vbo_color_smoke);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_color_smoke);
	glBufferData(GL_ARRAY_BUFFER, sizeof(flagColor), flagColor, GL_STATIC_DRAW);

	glVertexAttribPointer(ATA_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	//glVertexAttrib3f(ATA_ATTRIBUTE_COLOR, 1.0f, 0.0f, 1.0f);
	glEnableVertexAttribArray(ATA_ATTRIBUTE_COLOR);

	glBindBuffer(GL_ARRAY_BUFFER, 0);



	//IAF
	glGenVertexArrays(1, &vao_iaf);
	glBindVertexArray(vao_iaf);

	glGenBuffers(1, &vbo_position_iaf);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position_iaf);
	glBufferData(GL_ARRAY_BUFFER, sizeof(IAF), IAF, GL_STATIC_DRAW);

	glVertexAttribPointer(ATA_ATTRIBUTE_POSITION, 2, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(ATA_ATTRIBUTE_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &vbo_color_iaf);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_color_iaf);
	glBufferData(GL_ARRAY_BUFFER, sizeof(IAFColor), IAFColor, GL_STATIC_DRAW);

	glVertexAttribPointer(ATA_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	//glVertexAttrib3f(ATA_ATTRIBUTE_COLOR, (float)186 / 255, (float)226 / 255, (float)238 / 255);
	glEnableVertexAttribArray(ATA_ATTRIBUTE_COLOR);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
    // set-up depth buffer
    glClearDepth(1.0f);
    // enable depth testing
    glEnable(GL_DEPTH_TEST);
    // depth test to do
    glDepthFunc(GL_LEQUAL);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    // We will always cull back faces for better performance
    glDisable(GL_CULL_FACE);
    
    // set background color 
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // blue
    
    perspectiveProjectionMatrix_ata = vmath::mat4::identity();

    CVDisplayLinkCreateWithActiveCGDisplays(&displayLink);
    
    CVDisplayLinkSetOutputCallback(displayLink, &MyDisplayLinkCallback, self);
    CGLContextObj cglContext=(CGLContextObj)[[self openGLContext]CGLContextObj];
    CGLPixelFormatObj cglPixelFormat=(CGLPixelFormatObj)[[self pixelFormat]CGLPixelFormatObj];
    CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext(displayLink, cglContext, cglPixelFormat);
    CVDisplayLinkStart(displayLink);
    
}

-(void)drawRect:(NSRect)dirtyRect
{
    [self drawView];
}

-(void)drawView
{
    [[self openGLContext]makeCurrentContext];
    
    CGLLockContext((CGLContextObj)[[self openGLContext]CGLContextObj]);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    
    //Start using shader program object
    glUseProgram(shaderProgramObject_ata); //run shaders
    
    //OpenGL drawing
    
    //set modleview and projection matrices to identity matrix
    
    vmath::mat4 modelViewMatrixAta = vmath::mat4::identity();
    vmath::mat4 modelViewProjectionMatrixAta = vmath::mat4::identity();
    vmath::mat4 translationMatrixAta = mat4::identity();
    vmath::mat4 rotationMatrixAta = mat4::identity();
	
	//bind vao I
    glBindVertexArray(vao_I);
	glLineWidth(3.0f);
	translationMatrixAta = vmath::translate(itrans, 0.0f, -6.0f);
    modelViewMatrixAta = modelViewMatrixAta * translationMatrixAta;

    modelViewProjectionMatrixAta = perspectiveProjectionMatrix_ata * modelViewMatrixAta; //mind the order
    
    // Pass above model view matrix projection matrix to vertex shader 
    
    glUniformMatrix4fv(mvpUniform_ata, 1, GL_FALSE, modelViewProjectionMatrixAta);
    glUniform1f(alphaUniform, 1.0f);
    
    // Draw either by glDrawTriangles() or glDrawArrays() or glDrawElements()
    glDrawArrays(GL_LINES, 0, 3); //3 is no of positions
    
    glBindVertexArray(0);
	
	//D vao
	glBindVertexArray(vao_D);
	modelViewMatrixAta = vmath::mat4::identity();
    modelViewProjectionMatrixAta = vmath::mat4::identity();
    translationMatrixAta = mat4::identity();
    rotationMatrixAta = mat4::identity();

    translationMatrixAta = vmath::translate(0.2f, 0.0f, -6.0f);
    modelViewMatrixAta = modelViewMatrixAta * translationMatrixAta;
    modelViewProjectionMatrixAta = perspectiveProjectionMatrix_ata * modelViewMatrixAta; //mind the order
    glUniformMatrix4fv(mvpUniform_ata, 1, GL_FALSE, modelViewProjectionMatrixAta);
    glUniform1f(alphaUniform, alphaBlend);
    // Draw either by glDrawTriangles() or glDrawArrays() or glDrawElements()
	glDrawArrays(GL_LINES, 0, 2);
	glDrawArrays(GL_LINES, 2, 4);
	glDrawArrays(GL_LINES, 4, 8);
	
    glBindVertexArray(0);
	
	//N
	glBindVertexArray(vao_N);
	modelViewMatrixAta = vmath::mat4::identity();
    modelViewProjectionMatrixAta = vmath::mat4::identity();
    translationMatrixAta = mat4::identity();
    rotationMatrixAta = mat4::identity();

    translationMatrixAta = vmath::translate(-1.5f, ntrans, -6.0f);
    modelViewMatrixAta = modelViewMatrixAta * translationMatrixAta;
    modelViewProjectionMatrixAta = perspectiveProjectionMatrix_ata * modelViewMatrixAta; //mind the order
    glUniformMatrix4fv(mvpUniform_ata, 1, GL_FALSE, modelViewProjectionMatrixAta);
    glUniform1f(alphaUniform, 1.0f);
    // Draw either by glDrawTriangles() or glDrawArrays() or glDrawElements()
	glDrawArrays(GL_LINE_STRIP, 0, 7); //3 is no of positions

    glBindVertexArray(0);
	
	//Second I
	modelViewMatrixAta = vmath::mat4::identity();
    modelViewProjectionMatrixAta = vmath::mat4::identity();
    translationMatrixAta = mat4::identity();
    rotationMatrixAta = mat4::identity();
	glBindVertexArray(vao_I);
	//glLineWidth(3.0f);
	translationMatrixAta = vmath::translate(1.0f, iitrans, -6.0f);
    modelViewMatrixAta = modelViewMatrixAta * translationMatrixAta;

    modelViewProjectionMatrixAta = perspectiveProjectionMatrix_ata * modelViewMatrixAta; //mind the order
    
    // Pass above model view matrix projection matrix to vertex shader 
    
    glUniformMatrix4fv(mvpUniform_ata, 1, GL_FALSE, modelViewProjectionMatrixAta);
    glUniform1f(alphaUniform, 1.0f);
    
    // Draw either by glDrawTriangles() or glDrawArrays() or glDrawElements()
    glDrawArrays(GL_LINES, 0, 3); //3 is no of positions
    
    glBindVertexArray(0);
	
	
	//A vao
	glBindVertexArray(vao_A);
	modelViewMatrixAta = vmath::mat4::identity();
    modelViewProjectionMatrixAta = vmath::mat4::identity();
    translationMatrixAta = mat4::identity();
    rotationMatrixAta = mat4::identity();

    translationMatrixAta = vmath::translate(atrans, 0.0f, -6.0f);
    modelViewMatrixAta = modelViewMatrixAta * translationMatrixAta;
    modelViewProjectionMatrixAta = perspectiveProjectionMatrix_ata * modelViewMatrixAta; //mind the order
    glUniformMatrix4fv(mvpUniform_ata, 1, GL_FALSE, modelViewProjectionMatrixAta);
    glUniform1f(alphaUniform, 1.0f);
	
    // Draw either by glDrawTriangles() or glDrawArrays() or glDrawElements()
    glDrawArrays(GL_LINES, 0, 6); //3 is no of positions

    glBindVertexArray(0);
	
	
	//bind lines vao
    glBindVertexArray(vao_triline_ata);
    glLineWidth(1.0f);
	
    modelViewMatrixAta = vmath::mat4::identity();
    modelViewProjectionMatrixAta = vmath::mat4::identity();
    translationMatrixAta = mat4::identity();
    rotationMatrixAta = mat4::identity();

    translationMatrixAta = vmath::translate(xFlagtrans, 0.0f, -6.0f);
    modelViewMatrixAta = modelViewMatrixAta * translationMatrixAta;
    modelViewProjectionMatrixAta = perspectiveProjectionMatrix_ata * modelViewMatrixAta; //mind the order

	glUniformMatrix4fv(mvpUniform_ata, 1, GL_FALSE, modelViewProjectionMatrixAta);
    glUniform1f(alphaUniform, 1.0f);
    // Draw either by glDrawTriangles() or glDrawArrays() or glDrawElements()
    glDrawArrays(GL_LINES, 0, 6); //3 is no of positions

    glBindVertexArray(0);
	
	//smoke
	glBindVertexArray(vao_smoke);
    
    modelViewMatrixAta = vmath::mat4::identity();
    modelViewProjectionMatrixAta = vmath::mat4::identity();
    translationMatrixAta = mat4::identity();
    rotationMatrixAta = mat4::identity();

    translationMatrixAta = vmath::translate(xTritrans, 0.0f, -6.0f);
    modelViewMatrixAta = modelViewMatrixAta * translationMatrixAta;
    modelViewProjectionMatrixAta = perspectiveProjectionMatrix_ata * modelViewMatrixAta; //mind the order
    glUniformMatrix4fv(mvpUniform_ata, 1, GL_FALSE, modelViewProjectionMatrixAta);
    glUniform1f(alphaUniform, 1.0f);
    // Draw either by glDrawTriangles() or glDrawArrays() or glDrawElements()
    glDrawArrays(GL_LINES, 0, 6); //3 is no of positions

    glBindVertexArray(0);
	
	//plane1
    glBindVertexArray(vao_plane);
	modelViewMatrixAta = vmath::mat4::identity();
    modelViewProjectionMatrixAta = vmath::mat4::identity();
    translationMatrixAta = mat4::identity();
    rotationMatrixAta = mat4::identity();
	
    translationMatrixAta = vmath::translate(xPlanetrans, 0.078f, -8.0f);
    modelViewMatrixAta = modelViewMatrixAta * translationMatrixAta;

    modelViewProjectionMatrixAta = perspectiveProjectionMatrix_ata * modelViewMatrixAta; //mind the order
    
    // Pass above model view matrix projection matrix to vertex shader 
    
    glUniformMatrix4fv(mvpUniform_ata, 1, GL_FALSE, modelViewProjectionMatrixAta);
    glUniform1f(alphaUniform, 1.0f);
   
   // Draw either by glDrawTriangles() or glDrawArrays() or glDrawElements()
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glDrawArrays(GL_TRIANGLES, 4, 21);
	
    glBindVertexArray(0);
	
	//topplane2
	glBindVertexArray(vao_plane);
	modelViewMatrixAta = vmath::mat4::identity();
    modelViewProjectionMatrixAta = vmath::mat4::identity();
    translationMatrixAta = mat4::identity();
    rotationMatrixAta = mat4::identity();
	
    translationMatrixAta = vmath::translate(xPlane1trans, yPlane1trans, -8.0f);
    modelViewMatrixAta = modelViewMatrixAta * translationMatrixAta;

    modelViewProjectionMatrixAta = perspectiveProjectionMatrix_ata * modelViewMatrixAta; //mind the order
    
    // Pass above model view matrix projection matrix to vertex shader 
    
    glUniformMatrix4fv(mvpUniform_ata, 1, GL_FALSE, modelViewProjectionMatrixAta);
    glUniform1f(alphaUniform, 1.0f);
    
	// Draw either by glDrawTriangles() or glDrawArrays() or glDrawElements()
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glDrawArrays(GL_TRIANGLES, 4, 21);
	
    glBindVertexArray(0);
	
	//bottomplane3
	glBindVertexArray(vao_plane);
	modelViewMatrixAta = vmath::mat4::identity();
    modelViewProjectionMatrixAta = vmath::mat4::identity();
    translationMatrixAta = mat4::identity();
    rotationMatrixAta = mat4::identity();
	
    translationMatrixAta = vmath::translate(xPlane2trans, yPlane2trans, -8.0f);
    modelViewMatrixAta = modelViewMatrixAta * translationMatrixAta;

    modelViewProjectionMatrixAta = perspectiveProjectionMatrix_ata * modelViewMatrixAta; //mind the order
    
    // Pass above model view matrix projection matrix to vertex shader 
    
    glUniformMatrix4fv(mvpUniform_ata, 1, GL_FALSE, modelViewProjectionMatrixAta);
    glUniform1f(alphaUniform, 1.0f);
    
	// Draw either by glDrawTriangles() or glDrawArrays() or glDrawElements()
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glDrawArrays(GL_TRIANGLES, 4, 21);
	
    glBindVertexArray(0);
	
    //bind iaf plane1 vao
    glBindVertexArray(vao_iaf);
    glLineWidth(1.0f);
    modelViewMatrixAta = vmath::mat4::identity();
    modelViewProjectionMatrixAta = vmath::mat4::identity();
    translationMatrixAta = mat4::identity();
    rotationMatrixAta = mat4::identity();

    translationMatrixAta = vmath::translate(xIAFtrans, 0.0f, -8.0f);
    modelViewMatrixAta = modelViewMatrixAta * translationMatrixAta;
    modelViewProjectionMatrixAta = perspectiveProjectionMatrix_ata * modelViewMatrixAta; //mind the order
    
	glUniformMatrix4fv(mvpUniform_ata, 1, GL_FALSE, modelViewProjectionMatrixAta);
    glUniform1f(alphaUniform, 1.0f);
   
   // Draw either by glDrawTriangles() or glDrawArrays() or glDrawElements()
    glDrawArrays(GL_LINES, 0, 12); //3 is no of positions

    glBindVertexArray(0);
	
	//iaf2
	//bind iaf plane1 vao
    glBindVertexArray(vao_iaf);
    glLineWidth(1.0f);
    modelViewMatrixAta = vmath::mat4::identity();
    modelViewProjectionMatrixAta = vmath::mat4::identity();
    translationMatrixAta = mat4::identity();
    rotationMatrixAta = mat4::identity();

    translationMatrixAta = vmath::translate(xPlane1trans, yPlane1trans, -8.0f);
    modelViewMatrixAta = modelViewMatrixAta * translationMatrixAta;
    modelViewProjectionMatrixAta = perspectiveProjectionMatrix_ata * modelViewMatrixAta; //mind the order
    
	glUniformMatrix4fv(mvpUniform_ata, 1, GL_FALSE, modelViewProjectionMatrixAta);
    glUniform1f(alphaUniform, 1.0f);
   
   // Draw either by glDrawTriangles() or glDrawArrays() or glDrawElements()
    glDrawArrays(GL_LINES, 0, 12); //3 is no of positions

    glBindVertexArray(0);
	
	//iaf3
	//bind iaf plane1 vao
    glBindVertexArray(vao_iaf);
    glLineWidth(1.0f);
    modelViewMatrixAta = vmath::mat4::identity();
    modelViewProjectionMatrixAta = vmath::mat4::identity();
    translationMatrixAta = mat4::identity();
    rotationMatrixAta = mat4::identity();

    translationMatrixAta = vmath::translate(xPlane2trans, yPlane2trans, -8.0f);
    modelViewMatrixAta = modelViewMatrixAta * translationMatrixAta;
    modelViewProjectionMatrixAta = perspectiveProjectionMatrix_ata * modelViewMatrixAta; //mind the order
    
	glUniformMatrix4fv(mvpUniform_ata, 1, GL_FALSE, modelViewProjectionMatrixAta);
    glUniform1f(alphaUniform, 1.0f);
   
   // Draw either by glDrawTriangles() or glDrawArrays() or glDrawElements()
    glDrawArrays(GL_LINES, 0, 12); //3 is no of positions

    glBindVertexArray(0);
	
    [self update];
    //stop using shaders
    glUseProgram(0);
    
    CGLFlushDrawable((CGLContextObj)[[self openGLContext]CGLContextObj]);
    CGLUnlockContext((CGLContextObj)[[self openGLContext]CGLContextObj]);
}

-(void)update
{
	if (angleSquareAta > 360.0f) {
		angleSquareAta = 0.0f;
	}
	angleSquareAta = angleSquareAta + 1.0f;

	if (angleTriangleAta > 360.0f) {
		angleTriangleAta = 0.0f;
	}
	angleTriangleAta = angleTriangleAta + 1.0f;

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
		alphaBlend = alphaBlend + 0.01;
	}

	if (alphaBlend > 1.0)
	{

		if (xTritrans < 10.0)
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

			Bottomangle = Bottomangle + 0.0008;
			if (Bottomangle > 45.0)
			{
				Bottomangle = 45.0;
			}
		}

		if (xTritrans > 1.5)
		{
			xFlagtrans = 2.0f;
		}

		//}
		/*if (Bottomangle < 0.0)
		{

		}*/
	}

}
-(void)reshape
{
    CGLLockContext((CGLContextObj)[[self openGLContext]CGLContextObj]);
    NSRect rect=[self bounds];
    
    GLfloat widthAta=rect.size.width;
    GLfloat heightAta=rect.size.height;
    
    if(heightAta == 0)
        heightAta=1;
    
    glViewport(0,0 , (GLsizei)widthAta, (GLsizei)heightAta);
    perspectiveProjectionMatrix_ata = vmath::perspective(45.0f, ((GLfloat)widthAta / (GLfloat)heightAta),0.1f,100.0f);
    CGLUnlockContext((CGLContextObj)[[self openGLContext]CGLContextObj]);
    
}
-(CVReturn)getFrameForTime:(const CVTimeStamp *)pOutputTime
{
    NSAutoreleasePool *pool=[[NSAutoreleasePool alloc]init];
    [self drawView];
    
    [pool release];
    return (kCVReturnSuccess);
}

-(BOOL)acceptsFirstResponder
{
    //code
    NSLog(@"In acceptFirstResponder");
    [[self window]makeFirstResponder:self];
    
    return(YES);
    
}

-(void)keyDown:(NSEvent *)theEvent
{
    int key=(int)[[theEvent characters]characterAtIndex:0];
    switch(key)
    {
        case 27://Esc key
            [self release];
            [NSApp terminate:self];
            break;
        case 'F':
        case 'f':
            [[self window]toggleFullScreen:self]; //repainting occurs automatically
            break;
        default:
            break;
    }
}

-(void)mouseDown:(NSEvent *)theEvent
{
    //code
    [self setNeedsDisplay:YES];
}

-(void)mouseDragged:(NSEvent *)theEvent
{
    //mouse drag code will go here
}

-(void)rightMouseDown:(NSEvent *)theEvent
{
    [self setNeedsDisplay:YES];
}

-(void) dealloc
{
    //code
    
    if (vao_horizontal_ata)
    {
        glDeleteVertexArrays(1, &vao_horizontal_ata);
        vao_horizontal_ata = 0;
    }
     if (vao_vertical_ata) 
    {
        glDeleteVertexArrays(1, &vao_vertical_ata);
        vao_vertical_ata = 0;
    }
    if (vbo_position) 
    {
        glDeleteBuffers(1, &vbo_position);
        vbo_position = 0;
    }
    if (vbo_color) 
    {
	glDeleteBuffers(1, &vbo_color);
	vbo_color = 0;
	}  
	
	if (vbo_position_horizontal) 
    {
        glDeleteBuffers(1, &vbo_position_horizontal);
        vbo_position_horizontal = 0;
    }
	if (vbo_color_horizontal) 
    {
	glDeleteBuffers(1, &vbo_color_horizontal);
	vbo_color_horizontal = 0;
	}
	
	if (vao_I)
	{
		glDeleteVertexArrays(1, &vao_I);
		vao_I = 0;
	}
	if (vbo_position_i)
	{
		glDeleteBuffers(1, &vbo_position_i);
		vbo_position_i = 0;
	}
	if (vbo_position_n)
	{
		glDeleteBuffers(1, &vbo_position_n);
		vbo_position_n = 0;
	}
	if (vbo_color_i)
	{
		glDeleteBuffers(1, &vbo_color_i);
		vbo_color_i = 0;
	}
	if (vbo_color_n)
	{
		glDeleteBuffers(1, &vbo_color_n);
		vbo_color_n = 0;
	}
	if (vao_N)
	{
		glDeleteVertexArrays(1, &vao_N);
		vao_N = 0;
	}

    
    //Detach vertex shader
    glDetachShader(shaderProgramObject_ata, vertexShaderObject_ata);
    
    //Detach fragment shader
    glDetachShader(shaderProgramObject_ata, fragmentShaderObject_ata);
    
    //Delete vertex shader object
    glDeleteShader(vertexShaderObject_ata);
    vertexShaderObject_ata = 0;
    
    //Delete fragment shader object
    glDeleteShader(fragmentShaderObject_ata);
    fragmentShaderObject_ata = 0;
    
    //Delete shader program object
    glDeleteProgram(shaderProgramObject_ata);
    shaderProgramObject_ata = 0;
    CVDisplayLinkStop(displayLink);
    CVDisplayLinkRelease(displayLink);
    [super dealloc];
}
@end

CVReturn MyDisplayLinkCallback(CVDisplayLinkRef displayLink, const CVTimeStamp *pNow,
                               const CVTimeStamp *pOutputTime, CVOptionFlags flagsIn,
                               CVOptionFlags *pFlagsOut, void *pDisplayLinkContext)
{
    CVReturn result=[(GLView *)pDisplayLinkContext getFrameForTime:pOutputTime];
    return(result);
}
