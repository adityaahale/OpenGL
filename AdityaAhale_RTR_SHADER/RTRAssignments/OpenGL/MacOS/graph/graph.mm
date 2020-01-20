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
    [window setTitle:@"DeathlyHallows"];
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
	GLuint vao_line_ata;
	
    GLuint vbo_position;
	GLuint vbo_position_line;
	GLuint vbo_position_horizontal;
	
	GLuint vbo_color;
	GLuint vbo_color_line;
	GLuint vbo_color_horizontal;

    GLuint mvpUniform_ata;
    GLfloat angleTriangleAta;
    GLfloat angleSquareAta;
	GLfloat angleDHAta;
	GLfloat rot, xtrans, ytrans, xctrans, yctrans, yltrans;
	
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
	rot = 0.0f; 
	xtrans = -3.0f;
	ytrans = -3.0f; 
	xctrans = 3.0f; 
	yctrans = -3.0f; 
	yltrans = 3.0f;
    x1 = 0.0f; 
	x2 = -0.7f; 
	x3 = 0.7f; 
	yone = 0.7f; 
	y2 = -0.7f; 
	y3 = -0.7f;
	
	self=[super initWithFrame:frame];
    
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
    "out vec4 FragColor;" \
    "void main(void)" \
    "{" \
    "FragColor = out_color;" \
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
    
    // Vertices, colors, shader attribs, vbo, vao initializations
    /*c = sqrtf((x2 - x1)*(x2 - x1) + (y2 - yone)*(y2 - yone)); //These are opposite lengths/sides of vertices
	a = sqrtf((x3 - x2)*(x3 - x2) + (y3 - y2)*(y3 - y2));
	b = sqrtf((x1 - x3)*(x1 - x3) + (yone - y3)*(yone - y3));
	summation = a + b + c;
	s = (float)(a + b + c) / 2.0f;

	area = sqrtf(s*(s - a)*(s - b)*(s - c));
	radius = area / s;
	*/


	GLfloat lineVerticesAta[8000];
	//GLfloat numberOfVertices = 1000 + 1;
	
	int k = 0;
	for (float i = -1.0f; i < 1; i += (float)2 / 40)
	{
		//glColor3f(0.0f, 0.0f, 1.0f);
		lineVerticesAta[k] = 1.0f;
		k++;
		lineVerticesAta[k]= i;
		k++;
		lineVerticesAta[k] = 0.0f;
		k++;

		lineVerticesAta[k] = -1.0f;
		k++;
		lineVerticesAta[k] = i;
		k++;
		lineVerticesAta[k] = 0.0f;
		k++;

		lineVerticesAta[k] = i;
		k++;
		lineVerticesAta[k] =-1.0f;
		k++;
		lineVerticesAta[k] = 0.0f;
		k++;

		lineVerticesAta[k] = i;
		k++;
		lineVerticesAta[k]=1.0f;
		k++;
		lineVerticesAta[k] = 0.0f;
		k++;
	}
	

	glGenVertexArrays(1, &vao_line_ata);
	glBindVertexArray(vao_line_ata);

	glGenBuffers(1, &vbo_position_line);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position_line);
	glBufferData(GL_ARRAY_BUFFER, sizeof(lineVerticesAta), lineVerticesAta, GL_STATIC_DRAW);

	glVertexAttribPointer(ATA_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(ATA_ATTRIBUTE_POSITION);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

	glVertexAttrib3f(ATA_ATTRIBUTE_COLOR, 0.0f, 0.0f, 1.0f);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
	
	
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
    
    //vbo for triangle colors
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
    glBindVertexArray(0);  //done with triangle vao
    
    
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
    
    // set-up depth buffer
    glClearDepth(1.0f);
    // enable depth testing
    glEnable(GL_DEPTH_TEST);
    // depth test to do
    glDepthFunc(GL_LEQUAL);
    glLineWidth(3.0f);
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
	
    
    //bind lines vao
    
    glBindVertexArray(vao_line_ata);
    glLineWidth(1.0f);
    
    
    translationMatrixAta = vmath::translate(0.0f, 0.0f, -1.0f);
    modelViewMatrixAta = modelViewMatrixAta * translationMatrixAta;
    modelViewProjectionMatrixAta = perspectiveProjectionMatrix_ata * modelViewMatrixAta; //mind the order
    
    glUniformMatrix4fv(mvpUniform_ata, 1, GL_FALSE, modelViewProjectionMatrixAta);
    
    // Draw either by glDrawTriangles() or glDrawArrays() or glDrawElements()
    glDrawArrays(GL_LINES, 0, 200); //3 is no of positions
    
    glBindVertexArray(0);
    
    //horizontal
    glBindVertexArray(vao_horizontal_ata);
    glLineWidth(3.0f);
    
    modelViewMatrixAta = vmath::mat4::identity();
    modelViewProjectionMatrixAta = vmath::mat4::identity();
    translationMatrixAta = mat4::identity();
    rotationMatrixAta = mat4::identity();
    translationMatrixAta = vmath::translate(0.0f, 0.0f, -1.0f);
    modelViewMatrixAta = modelViewMatrixAta * translationMatrixAta;

    modelViewProjectionMatrixAta = perspectiveProjectionMatrix_ata * modelViewMatrixAta; //mind the order
    
    // Pass above model view matrix projection matrix to vertex shader 
    
    glUniformMatrix4fv(mvpUniform_ata, 1, GL_FALSE, modelViewProjectionMatrixAta);
    
    //bind vao
    // Draw either by glDrawTriangles() or glDrawArrays() or glDrawElements()
    glDrawArrays(GL_LINES, 0, 3); //3 is no of positions
    
    glBindVertexArray(0);
	
    //bind vertical vao
    glBindVertexArray(vao_vertical_ata);
    
    modelViewMatrixAta = vmath::mat4::identity();
    modelViewProjectionMatrixAta = vmath::mat4::identity();
    translationMatrixAta = mat4::identity();
    rotationMatrixAta = mat4::identity();

    translationMatrixAta = vmath::translate(0.0f, 0.0f, -1.0f);
    modelViewMatrixAta = modelViewMatrixAta * translationMatrixAta;
    modelViewProjectionMatrixAta = perspectiveProjectionMatrix_ata * modelViewMatrixAta; //mind the order
    glUniformMatrix4fv(mvpUniform_ata, 1, GL_FALSE, modelViewProjectionMatrixAta);
    
    // Draw either by glDrawTriangles() or glDrawArrays() or glDrawElements()
    glDrawArrays(GL_LINES, 0, 3); //3 is no of positions

    glBindVertexArray(0);
	
	
	
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
	
	angleDHAta = angleDHAta + 0.1f;
	if (angleDHAta >= 360)
	{
		angleDHAta = 0.0f;
	}

	if (xtrans <= 0.0)
	{
		xtrans = xtrans + 0.1;
		ytrans = ytrans + 0.1;
	}

	if (xtrans >= 0.0)
	{
		if (xctrans > 0.0)
		{
			xctrans = xctrans - 0.1;
			yctrans = yctrans + 0.1;

		}
	}

	if (xtrans > 0.0)
	{
		if (xctrans < 0.0)
		{
			if (yltrans >= 0.0)
			{
				yltrans = yltrans - 0.1;
			}
		}
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
    //code
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
