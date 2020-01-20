//headers
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
    [window setTitle:@"Texture"];
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
    
    GLuint vao_pyramid_ata;
    GLuint vao_cube_ata;
    GLuint vbo_position_pyramid_ata;
    GLuint vbo_texture_pyramid_ata;
    GLuint vbo_texture_cube_ata;
    GLuint vbo_position_cube_ata;
    GLuint mvpUniform_ata;
     GLfloat angleTriangleAta;
     GLfloat angleSquareAta;
     GLuint texture_sampler_uniform_ata; 
     GLuint Texture_KundaliAta;
     GLuint Texture_SmileyAta;
     int key_press;	
    vmath::mat4 perspectiveProjectionMatrix_ata;
}


-(id)initWithFrame:(NSRect)frame;
{
    //code
    key_press=0;
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
    [[self openGLContext]makeCurrentContext];
    GLint swapInt = 1;
    [[self openGLContext]setValues:&swapInt forParameter:NSOpenGLCPSwapInterval];
    
    //-----VERTEX SHADER----
    //vertex shader
    vertexShaderObject_ata = glCreateShader(GL_VERTEX_SHADER);
    
    const GLchar *vertexShaderSourceCodeAta =        
    	"#version 410 core" \
		"\n" \
		"in vec4 vPosition;" \
		"in vec2 vTexture0_Coord;" \
		"out vec2 out_texture0_coord;" \
		"uniform mat4 u_mvp_matrix;" \
		"void main(void)" \
		"{" \
		"gl_Position = u_mvp_matrix * vPosition;" \
		"out_texture0_coord = vTexture0_Coord;" \
    "}";
    
    glShaderSource(vertexShaderObject_ata, 1, (const GLchar**)&vertexShaderSourceCodeAta, NULL); 
    
    //compile vertex shader
    glCompileShader(vertexShaderObject_ata);
    //Shader compilation error checking goes here...
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
		"in vec2 out_texture0_coord;" \
		"out vec4 FragColor;" \
		"uniform sampler2D u_texture0_sampler;" \
		"void main(void)" \
		"{" \
		"FragColor = texture(u_texture0_sampler, out_texture0_coord);" \
    		"}";
		
    glShaderSource(fragmentShaderObject_ata, 1, (const GLchar**)&fragmentShaderSourceCodeAta, NULL);
    
    //compile fragment shader
    glCompileShader(fragmentShaderObject_ata);
    //Shader compilation errors goes here..
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
    
    //map our(RAM) memory identifier to GPU memory(VRAM) identifier
    glBindAttribLocation(shaderProgramObject_ata, ATA_ATTRIBUTE_POSITION, "vPosition");
    glBindAttribLocation(shaderProgramObject_ata, ATA_ATTRIBUTE_TEXTURE0, "vTexture0_Coord");
    
    //Link shader program
    glLinkProgram(shaderProgramObject_ata);
    //Linking error checks goes here...
    
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
    //Preparation to put our dynamic(uniform) data into the shader
    mvpUniform_ata = glGetUniformLocation(shaderProgramObject_ata, "u_mvp_matrix");
    texture_sampler_uniform_ata = glGetUniformLocation(shaderProgramObject_ata, "u_texture0_sampler");
    Texture_SmileyAta = [self loadTextureFromBMPFile:"Smiley.bmp"];
    
    // Vertices, colors, shader attribs, vbo, vao initializations
   
    //square
    const GLfloat smileyVerticesAta[] = {
    		-1.0f, 1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		1.0f, 1.0f, 0.0f
	};
	

    glGenVertexArrays(1, &vao_cube_ata);
    glBindVertexArray(vao_cube_ata);

    glGenBuffers(1, &vbo_position_cube_ata);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_position_cube_ata);
    glBufferData(GL_ARRAY_BUFFER, sizeof(smileyVerticesAta), smileyVerticesAta, GL_STATIC_DRAW);

    glVertexAttribPointer(ATA_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(ATA_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

    //texture
    glGenBuffers(1, &vbo_texture_cube_ata);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_texture_cube_ata);
    glBufferData(GL_ARRAY_BUFFER, 32, NULL, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(ATA_ATTRIBUTE_TEXTURE0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(ATA_ATTRIBUTE_TEXTURE0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0); //done with cube vao
    
    // set-up depth buffer
    glClearDepth(1.0f);
    // enable depth testing
    glEnable(GL_DEPTH_TEST);
    // depth test to do
    glDepthFunc(GL_LEQUAL);
    
    // We will always cull back faces for better performance
    glEnable(GL_CULL_FACE);
    
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
    //code
    [[self openGLContext]makeCurrentContext];
    
    CGLLockContext((CGLContextObj)[[self openGLContext]CGLContextObj]);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    GLfloat smileyTexcoordsAta[8];
	
    if (key_press == 1) { 
		smileyTexcoordsAta[0] = 0.0f;
		smileyTexcoordsAta[1] = 1.0f;
		smileyTexcoordsAta[2] = 0.0f;
		smileyTexcoordsAta[3] = 0.0f;
		smileyTexcoordsAta[4] = 1.0f;
		smileyTexcoordsAta[5] = 0.0f;
		smileyTexcoordsAta[6] = 1.0f;
		smileyTexcoordsAta[7] = 1.0f;
	}
	else if (key_press == 2) { 
		smileyTexcoordsAta[0] = 0.0f;
		smileyTexcoordsAta[1] = 0.5f;
		smileyTexcoordsAta[2] = 0.0f;
		smileyTexcoordsAta[3] = 0.0f;
		smileyTexcoordsAta[4] = 0.5f;
		smileyTexcoordsAta[5] = 0.0f;
		smileyTexcoordsAta[6] = 0.5f;
		smileyTexcoordsAta[7] = 0.5f;
	}
	else if (key_press == 3) { 
		smileyTexcoordsAta[0] = 0.0f;
		smileyTexcoordsAta[1] = 2.0f;
		smileyTexcoordsAta[2] = 0.0f;
		smileyTexcoordsAta[3] = 0.0f;
		smileyTexcoordsAta[4] = 2.0f;
		smileyTexcoordsAta[5] = 0.0f;
		smileyTexcoordsAta[6] = 2.0f;
		smileyTexcoordsAta[7] = 2.0f;
	} 
	else if (key_press == 4) { 
		smileyTexcoordsAta[0] = 0.5f;
		smileyTexcoordsAta[1] = 0.5f;
		smileyTexcoordsAta[2] = 0.5f;
		smileyTexcoordsAta[3] = 0.5f;
		smileyTexcoordsAta[4] = 0.5f;
		smileyTexcoordsAta[5] = 0.5f;
		smileyTexcoordsAta[6] = 0.5f;
		smileyTexcoordsAta[7] = 0.5f;
	}
    else if(key_press==0)
    {
        smileyTexcoordsAta[0] = 0.0f;
        smileyTexcoordsAta[1] = 0.0f;
        smileyTexcoordsAta[2] = 0.0f;
        smileyTexcoordsAta[3] = 0.0f;
        smileyTexcoordsAta[4] = 0.0f;
        smileyTexcoordsAta[5] = 0.0f;
        smileyTexcoordsAta[6] = 0.0f;
        smileyTexcoordsAta[7] = 0.0f;
    }
    //Start using shader program object
    glUseProgram(shaderProgramObject_ata); 
    
    //OpenGL drawing
    
    //set modleview and projection matrices to identity matrix
    
    vmath::mat4 modelViewMatrixAta = vmath::mat4::identity();
    vmath::mat4 modelViewProjectionMatrixAta = vmath::mat4::identity();
    vmath::mat4 translationMatrixAta = mat4::identity();
    vmath::mat4 rotationMatrixAta = mat4::identity();
   
 glBindVertexArray(vao_cube_ata);

    translationMatrixAta = vmath::translate(0.0f, 0.0f, -4.0f);
    modelViewMatrixAta = modelViewMatrixAta * translationMatrixAta;
    modelViewProjectionMatrixAta = perspectiveProjectionMatrix_ata * modelViewMatrixAta; //mind the order
    glUniformMatrix4fv(mvpUniform_ata, 1, GL_FALSE, modelViewProjectionMatrixAta);
    
    //bind with cube texture
    glBindBuffer(GL_ARRAY_BUFFER,vbo_texture_cube_ata);
    glBufferData(GL_ARRAY_BUFFER, sizeof(smileyTexcoordsAta), smileyTexcoordsAta, GL_DYNAMIC_DRAW);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, Texture_SmileyAta);
    glUniform1i(texture_sampler_uniform_ata, 0);
    
    // Draw either by glDrawTriangles() or glDrawArrays() or glDrawElements()
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4); //4 is no of positions
    glBindBuffer(GL_ARRAY_BUFFER, 0);

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
	angleTriangleAta = angleTriangleAta + 0.2f;
}

-(GLuint) loadTextureFromBMPFile:(const char *)texFileNameAta
{
	NSBundle *mainBundleAta=[NSBundle mainBundle];
	NSString *appDirNameAta=[mainBundleAta bundlePath];
	NSString *parentDirPathAta=[appDirNameAta stringByDeletingLastPathComponent];
	NSString *textureFileNameWithPathAta=[NSString stringWithFormat:@"%@/%s", parentDirPathAta, texFileNameAta];
	
	NSImage *bmpImagAta=[[NSImage alloc] initWithContentsOfFile:textureFileNameWithPathAta];
	if(!bmpImagAta)
	{
		NSLog(@"Can't find %@", textureFileNameWithPathAta);
		return(0);
	}
	
	CGImageRef cgImageAta = [bmpImagAta CGImageForProposedRect:nil context:nil hints:nil];
	
	int w = (int) CGImageGetWidth(cgImageAta);
	int h = (int) CGImageGetHeight(cgImageAta);
	CFDataRef imageDataAta = CGDataProviderCopyData(CGImageGetDataProvider(cgImageAta));
	
	void *pixels = (void*) CFDataGetBytePtr(imageDataAta);
	
	GLuint bmpTextureAta;
	glGenTextures(1, &bmpTextureAta);
	
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glBindTexture(GL_TEXTURE_2D, bmpTextureAta);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE,pixels);
	
	glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
	CFRelease(imageDataAta);
	return (bmpTextureAta);
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
	case 49: //1
		key_press = 1;
		break;
	case 50:
		key_press = 2;
		break;
	case 51:
		key_press = 3;
		break;
	case 52:
		key_press = 4;
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
}

-(void)mouseDragged:(NSEvent *)theEvent
{
    //code
}

-(void)rightMouseDown:(NSEvent *)theEvent
{
    //code
}

-(void) dealloc
{
    //code
    
    if (vao_pyramid_ata)
     {
        glDeleteVertexArrays(1, &vao_pyramid_ata);
        vao_pyramid_ata = 0;
     }
     if (vao_cube_ata) 
     {
        glDeleteVertexArrays(1, &vao_cube_ata);
        vao_cube_ata = 0;
     }
    if (vbo_position_pyramid_ata) 
    {
        glDeleteBuffers(1, &vbo_position_pyramid_ata);
        vbo_position_pyramid_ata = 0;
    }
    if (vbo_texture_pyramid_ata) 
    {
		glDeleteBuffers(1, &vbo_texture_pyramid_ata);
		vbo_texture_pyramid_ata = 0;
	}    
   
	if (vbo_position_cube_ata) {
		glDeleteBuffers(1, &vbo_position_cube_ata);
		vbo_position_cube_ata = 0;
	}

	if (vbo_texture_cube_ata) {
		glDeleteBuffers(1, &vbo_texture_cube_ata);
		vbo_texture_cube_ata = 0;
	}
	if (Texture_SmileyAta) {
		glDeleteTextures(1, &Texture_SmileyAta);
		Texture_SmileyAta = 0;
	}
	if (Texture_KundaliAta) {
		glDeleteTextures(1, &Texture_KundaliAta);
		Texture_KundaliAta = 0;
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
