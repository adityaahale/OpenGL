#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>

#import <QuartzCore/CVDisplayLink.h>
#import <OpenGL/gl3.h>
#import <OpenGL/gl3ext.h>
#import "vmath.h"
#import "Sphere.h"

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
    
    //window code®
    NSRect win_rect;
    win_rect = NSMakeRect(0.0,0.0,800.0,600.0);
    
        //create simple window
    window = [[NSWindow alloc] initWithContentRect:win_rect
                                         styleMask:NSWindowStyleMaskTitled|NSWindowStyleMaskClosable|
              NSWindowStyleMaskMiniaturizable|NSWindowStyleMaskResizable
                                           backing:NSBackingStoreBuffered defer:NO];
    [window setTitle:@"Sphere"];
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
    GLuint vao_sphere_ata;
    GLuint vbo_position_pyramid_ata;
    GLuint vbo_color_pyramid_ata;
    GLuint vbo_normal_sphere_ata;
    GLuint vbo_position_sphere_ata;
	GLuint vbo_element_sphere_ata;
    GLuint mvpUniform_ata;
    
	GLuint modelMatrixUniformAta, viewMatrixUniformAta, projectionMatrixUniformAta;
	GLuint light0PositionUniformAta,  light1PositionUniformAta, light2PositionUniformAta;
	GLuint laUniformAta, l0dUniformAta, l0sUniformAta, l1dUniformAta, l1sUniformAta, l2dUniformAta, l2sUniformAta;
	GLuint kaUniformAta, kdUniformAta, ksUniformAta;
	GLuint material_shininess_uniform;
	GLuint lKeyPressedUniformAta;
	bool animateAta;
	bool lightAta;
	GLfloat angleTriangleAta;
    GLfloat angleSquareAta;
    vmath::mat4 perspectiveProjectionMatrix_ata;
	
	float sphere_vertices[1146];
	float sphere_normals[1146];
	float sphere_textures[764];
	unsigned short sphere_elements[2280];
	unsigned int numVerticesAta, numElementsAta;
	
	GLfloat lightAmbientAta[4];
	
	GLfloat light0DiffuseAta[4];
	GLfloat light0SpecularAta[4];
	GLfloat light1DiffuseAta[4];
	GLfloat light1SpecularAta[4];
	GLfloat light2DiffuseAta[4];
	GLfloat light2SpecularAta[4];
	
	GLfloat lightPosition[4];

	GLfloat material_ambient[4];
	GLfloat material_diffuse[4];
	GLfloat material_specular[4];
	GLfloat material_shininess;
	Sphere *sphere;
	float myRadius;
}


-(id)initWithFrame:(NSRect)frame;
{
    //code
	lightAta = false;
	animateAta = false;
	
    self=[super initWithFrame:frame];
    
    myRadius=100.0f;
	
	lightAmbientAta[0]=0.0f;
	lightAmbientAta[1]=0.0f;
	lightAmbientAta[2]=0.0f;
	lightAmbientAta[3]=1.0f;
	
	light0DiffuseAta[0]=1.0f;
	light0DiffuseAta[1]=0.0f;
	light0DiffuseAta[2]=0.0f;
	light0DiffuseAta[3]=0.0f;
	
	light0SpecularAta[0]=1.0f;
	light0SpecularAta[1]=0.0f;
	light0SpecularAta[2]=0.0f;
	light0SpecularAta[3]=0.0f;
	
	light1DiffuseAta[0]=0.0f;
	light1DiffuseAta[2]=0.0f;
	light1DiffuseAta[3]=0.0f;
	light1DiffuseAta[1]=1.0f;
	
	light1SpecularAta[0]=0.0f;
	light1SpecularAta[2]=0.0f;
	light1SpecularAta[3]=0.0f;
	light1SpecularAta[1]=1.0f;
	

	light2DiffuseAta[0]=0.0f;
	light2DiffuseAta[1]=0.0f;
	light2DiffuseAta[3]=0.0f;
	light2DiffuseAta[2]=1.0f;
	
	light2SpecularAta[0]=0.0f;
	light2SpecularAta[1]=0.0f;
	light2SpecularAta[3]=0.0f;
	light1SpecularAta[2]=1.0f;
	
	lightPosition[0]=0.0f;
	lightPosition[1]=0.0f;
	lightPosition[2]=0.0f;
	lightPosition[3]=0.0f;
	
	material_ambient[0]=0.0f;
	material_ambient[1]=0.0f;
	material_ambient[2]=0.0f;
	material_ambient[3]=1.0f;
	
	material_diffuse[0]=1.0f;
	material_diffuse[1]=1.0f;
	material_diffuse[2]=1.0f;
	material_diffuse[3]=1.0f;
	
    material_specular[0]=1.0f;
    material_specular[1]=1.0f;
    material_specular[2]=1.0f;
	material_specular[3]=1.0f;
	
	material_shininess = 50.0f;
	
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
		"in vec3 vNormal;" \
		"uniform mat4 u_model_matrix;" \
		"uniform mat4 u_view_matrix;" \
		"uniform mat4 u_projection_matrix;" \
		"uniform vec4 u_light0_position;" \
		"uniform vec4 u_light1_position;" \
		"uniform vec4 u_light2_position;" \
		"uniform int u_lighting_enabled;" \
		"out vec3 transformed_normals_light0;" \
		"out vec3 light0_direction;" \
		"out vec3 viewer_vector_light0;" \
		"out vec3 transformed_normals_light1;" \
		"out vec3 light1_direction;" \
		"out vec3 viewer_vector_light1;" \
		"out vec3 transformed_normals_light2;" \
		"out vec3 light2_direction;" \
		"out vec3 viewer_vector_light2;" \
		"void main(void)" \
		"{" \
		"if(u_lighting_enabled == 1)" \
		"{" \
		"vec4 eye_coordinates_light0 = u_view_matrix * u_model_matrix * vPosition;" \
		"transformed_normals_light0 = mat3(u_view_matrix * u_model_matrix) * vNormal;" \
		"light0_direction = vec3(u_light0_position) - eye_coordinates_light0.xyz;" \
		"viewer_vector_light0 = -eye_coordinates_light0.xyz;" \
		"vec4 eye_coordinates_light1 = u_view_matrix * u_model_matrix * vPosition;" \
		"transformed_normals_light1 = mat3(u_view_matrix * u_model_matrix) * vNormal;" \
		"light1_direction = vec3(u_light1_position) - eye_coordinates_light1.xyz;" \
		"viewer_vector_light1 = -eye_coordinates_light1.xyz;" \
		"vec4 eye_coordinates_light2 = u_view_matrix * u_model_matrix * vPosition;" \
		"transformed_normals_light2 = mat3(u_view_matrix * u_model_matrix) * vNormal;" \
		"light2_direction = vec3(u_light2_position) - eye_coordinates_light2.xyz;" \
		"viewer_vector_light2 = -eye_coordinates_light2.xyz;" \
		"}" \
		"gl_Position = u_projection_matrix * u_view_matrix * u_model_matrix * vPosition;" \
		"}";
    
    glShaderSource(vertexShaderObject_ata, 1, (const GLchar**)&vertexShaderSourceCodeAta, NULL); 
    
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
		"in vec3 transformed_normals_light0;" \
		"in vec3 light0_direction;" \
		"in vec3 viewer_vector_light0;" \
		"in vec3 transformed_normals_light1;" \
		"in vec3 light1_direction;" \
		"in vec3 viewer_vector_light1;" \
		"in vec3 transformed_normals_light2;" \
		"in vec3 light2_direction;" \
		"in vec3 viewer_vector_light2;" \
		"uniform vec3 u_La;" \
		"uniform vec3 u_L0d;" \
		"uniform vec3 u_L0s;" \
		"uniform vec3 u_L1d;" \
		"uniform vec3 u_L1s;" \
		"uniform vec3 u_L2d;" \
		"uniform vec3 u_L2s;" \
		"uniform vec3 u_Ka;" \
		"uniform vec3 u_Kd;" \
		"uniform vec3 u_Ks;" \
		"uniform float u_material_shininess;" \
		"uniform int u_lighting_enabled;" \
		"out vec4 FragColor;" \
		"void main(void)" \
		"{" \
		"vec3 phong_ads_light;"\
		"if(u_lighting_enabled == 1)" \
		"{" \
		"vec3 normalized_transformed_normals_light0 = normalize(transformed_normals_light0);" \
		"vec3 normalized_light0_direction = normalize(light0_direction);" \
		"vec3 normalized_viewer_vector_light0 = normalize(viewer_vector_light0);" \
		"float tn_dot_ld_light0 = max(dot(normalized_transformed_normals_light0, normalized_light0_direction), 0.0);" \
		"vec3 ambient = u_La * u_Ka;" \
		"vec3 diffuse_light0 = u_L0d * u_Kd * tn_dot_ld_light0;" \
		"vec3 reflection_vector_light0 = reflect(-normalized_light0_direction, normalized_transformed_normals_light0);" \
		"vec3 specular_light0 = u_L0s * u_Ks * pow(max(dot(reflection_vector_light0, normalized_viewer_vector_light0),0.0), u_material_shininess);" \
		"vec3 normalized_transformed_normals_light1 = normalize(transformed_normals_light1);" \
		"vec3 normalized_light1_direction = normalize(light1_direction);" \
		"vec3 normalized_viewer_vector_light1 = normalize(viewer_vector_light1);" \
		"float tn_dot_ld_light1 = max(dot(normalized_transformed_normals_light1, normalized_light1_direction), 0.0);" \
		"vec3 diffuse_light1 = u_L1d * u_Kd * tn_dot_ld_light1;" \
		"vec3 reflection_vector_light1 = reflect(-normalized_light1_direction, normalized_transformed_normals_light1);" \
		"vec3 specular_light1 = u_L1s * u_Ks * pow(max(dot(reflection_vector_light1, normalized_viewer_vector_light1),0.0), u_material_shininess);" \
		"vec3 normalized_transformed_normals_light2 = normalize(transformed_normals_light2);" \
		"vec3 normalized_light2_direction = normalize(light2_direction);" \
		"vec3 normalized_viewer_vector_light2 = normalize(viewer_vector_light2);" \
		"float tn_dot_ld_light2 = max(dot(normalized_transformed_normals_light2, normalized_light2_direction), 0.0);" \
		"vec3 diffuse_light2 = u_L2d * u_Kd * tn_dot_ld_light2;" \
		"vec3 reflection_vector_light2 = reflect(-normalized_light2_direction, normalized_transformed_normals_light2);" \
		"vec3 specular_light2 = u_L2s * u_Ks * pow(max(dot(reflection_vector_light2, normalized_viewer_vector_light2),0.0), u_material_shininess);" \
		
		"phong_ads_light = ambient + ambient + ambient + diffuse_light0 + diffuse_light1 + diffuse_light2 + specular_light0 + specular_light1+ specular_light2;" \
		"}" \
		"else" \
		"{" \
		"phong_ads_light = vec3(1.0, 1.0, 1.0);" \
		"}" \
		"FragColor = vec4(phong_ads_light, 1.0);" \
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
    
    //map our(RAM) memory identifier to GPU memory(VRAM) identifier
    glBindAttribLocation(shaderProgramObject_ata, ATA_ATTRIBUTE_POSITION, "vPosition");
    glBindAttribLocation(shaderProgramObject_ata, ATA_ATTRIBUTE_NORMAL, "vNormal");
    
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
    
    //mvpUniform_ata = glGetUniformLocation(shaderProgramObject_ata, "u_mvp_matrix");
    modelMatrixUniformAta = glGetUniformLocation(shaderProgramObject_ata, "u_model_matrix");
	viewMatrixUniformAta = glGetUniformLocation(shaderProgramObject_ata, "u_view_matrix");
	projectionMatrixUniformAta = glGetUniformLocation(shaderProgramObject_ata, "u_projection_matrix");
	lKeyPressedUniformAta = glGetUniformLocation(shaderProgramObject_ata, "u_lighting_enabled");
	
	laUniformAta = glGetUniformLocation(shaderProgramObject_ata, "u_La");
	
	l0dUniformAta = glGetUniformLocation(shaderProgramObject_ata, "u_L0d");
	l0sUniformAta = glGetUniformLocation(shaderProgramObject_ata, "u_L0s");

	l1dUniformAta = glGetUniformLocation(shaderProgramObject_ata, "u_L1d");
	l1sUniformAta = glGetUniformLocation(shaderProgramObject_ata, "u_L1s");

	l2dUniformAta = glGetUniformLocation(shaderProgramObject_ata, "u_L2d");
	l2sUniformAta = glGetUniformLocation(shaderProgramObject_ata, "u_L2s");
	
	light0PositionUniformAta = glGetUniformLocation(shaderProgramObject_ata, "u_light0_position");
	light1PositionUniformAta = glGetUniformLocation(shaderProgramObject_ata, "u_light1_position");
	light2PositionUniformAta = glGetUniformLocation(shaderProgramObject_ata, "u_light2_position");
    
	kaUniformAta = glGetUniformLocation(shaderProgramObject_ata, "u_Ka");
	kdUniformAta = glGetUniformLocation(shaderProgramObject_ata, "u_Kd");
	ksUniformAta = glGetUniformLocation(shaderProgramObject_ata, "u_Ks");
	//shininess of material
	material_shininess_uniform = glGetUniformLocation(shaderProgramObject_ata, "u_material_shininess");

	
	
	// Vertices, colors, shader attribs, vbo, vao initializations
    sphere = new Sphere();
	sphere->getSphereVertexData(sphere_vertices, sphere_normals, sphere_textures, sphere_elements);
	numVerticesAta = sphere->getNumberOfSphereVertices();
	numElementsAta = sphere->getNumberOfSphereElements();
   
   
    glGenVertexArrays(1, &vao_sphere_ata);
    glBindVertexArray(vao_sphere_ata);

    glGenBuffers(1, &vbo_position_sphere_ata);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_position_sphere_ata);
    glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_vertices), sphere_vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(ATA_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(ATA_ATTRIBUTE_POSITION);
	
	glGenBuffers(1, &vbo_normal_sphere_ata);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_normal_sphere_ata);
	glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_normals), sphere_normals, GL_STATIC_DRAW);

	glVertexAttribPointer(ATA_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(ATA_ATTRIBUTE_NORMAL);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0); //done with vao
    
	// element vbo
	glGenBuffers(1, &vbo_element_sphere_ata);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_element_sphere_ata);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sphere_elements), sphere_elements, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
    
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
    [[self openGLContext]makeCurrentContext];
    
    CGLLockContext((CGLContextObj)[[self openGLContext]CGLContextObj]);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    
    //Start using shader program object
    glUseProgram(shaderProgramObject_ata); //run shaders
   	
	
    //OpenGL drawing
    
    //set modleview and projection matrices to identity matrix
    
    vmath::mat4 modelMatrixAta = vmath::mat4::identity();
    vmath::mat4 viewMatrixAta = vmath::mat4::identity();
    vmath::mat4 translationMatrixAta = mat4::identity();
    vmath::mat4 rotationMatrixAta = mat4::identity();
   
    //bind square vao
    glBindVertexArray(vao_sphere_ata);
    
    translationMatrixAta = vmath::translate(0.0f, 0.0f, -2.0f);
    rotationMatrixAta = vmath::rotate(angleSquareAta,1.0f, 0.0f, 0.0f);
    rotationMatrixAta = rotationMatrixAta * vmath::rotate(angleSquareAta, 0.0f, 1.0f, 0.0f);  

    modelMatrixAta = modelMatrixAta * translationMatrixAta;
    //modelMatrixAta = modelMatrixAta * rotationMatrixAta;
    
    glUniformMatrix4fv(modelMatrixUniformAta, 1, GL_FALSE, modelMatrixAta);
    glUniformMatrix4fv(viewMatrixUniformAta,1,GL_FALSE,viewMatrixAta);
	glUniformMatrix4fv(projectionMatrixUniformAta,1,GL_FALSE, perspectiveProjectionMatrix_ata);
    
	if (lightAta == true) {
		glUniform1i(lKeyPressedUniformAta, 1);
		glUniform3fv(laUniformAta, 1, lightAmbientAta);
		
		glUniform3fv(l0dUniformAta, 1, light0DiffuseAta);
		glUniform3fv(l0sUniformAta, 1, light0SpecularAta);
		
		glUniform3fv(l1dUniformAta, 1, light1DiffuseAta);
		glUniform3fv(l1sUniformAta, 1, light1SpecularAta);
		
		glUniform3fv(l2dUniformAta, 1, light2DiffuseAta);
		glUniform3fv(l2sUniformAta, 1, light2SpecularAta);
		
		//light0
		lightPosition[1] = myRadius * cosf(angleSquareAta);
		lightPosition[2] = myRadius * sinf(angleSquareAta);
		glUniform4fv(light0PositionUniformAta, 1, lightPosition);
		lightPosition[1] = 0.0f;
		lightPosition[2] = 0.0f;

		//light1
		lightPosition[0] = myRadius * cosf(angleSquareAta);
		lightPosition[2] = myRadius * sinf(angleSquareAta);
		glUniform4fv(light1PositionUniformAta, 1, lightPosition);
		lightPosition[0] = 0.0f;
		lightPosition[2] = 0.0f;

		//light2
		lightPosition[0] = myRadius * (cosf(angleSquareAta);
		lightPosition[1] = myRadius * sinf(angleSquareAta);
		glUniform4fv(light2PositionUniformAta, 1, lightPosition);
		lightPosition[0] = 0.0f;
		lightPosition[1] = 0.0f;
		
		glUniform3fv(kaUniformAta, 1, material_ambient);
		glUniform3fv(kdUniformAta, 1, material_diffuse);
		glUniform3fv(ksUniformAta, 1, material_specular);
		glUniform1f(material_shininess_uniform, material_shininess);
	}
	else {
		glUniform1i(lKeyPressedUniformAta, 0);
	}
	
	// Draw either by glDrawTriangles() or glDrawArrays() or glDrawElements()
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_element_sphere_ata);
	glDrawElements(GL_TRIANGLES, numElementsAta, GL_UNSIGNED_SHORT, 0);
	
	glBindVertexArray(0);


	//stop using shaders
	glUseProgram(0);

	[self update]; //update 
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
			
		case 'A':
		case 'a':
			
			if (animateAta == false) 
			{
				animateAta = true;
			}
			else 
			{
				animateAta = false;
			
			}
			break;
			
		case 'L':
		case 'l':
			
			if (lightAta == false) 
			{
				lightAta = true;	
			}
			else 
			{
				lightAta = false;
			}
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
    
    if (vao_pyramid_ata)
     {
        glDeleteVertexArrays(1, &vao_pyramid_ata);
        vao_pyramid_ata = 0;
     }
     if (vao_sphere_ata) 
     {
        glDeleteVertexArrays(1, &vao_sphere_ata);
        vao_sphere_ata = 0;
     }
    if (vbo_position_pyramid_ata) 
    {
        glDeleteBuffers(1, &vbo_position_pyramid_ata);
        vbo_position_pyramid_ata = 0;
    }
    if (vbo_color_pyramid_ata) 
    {
		glDeleteBuffers(1, &vbo_color_pyramid_ata);
		vbo_color_pyramid_ata = 0;
	}    
   
	if (vbo_position_sphere_ata) {
		glDeleteBuffers(1, &vbo_position_sphere_ata);
		vbo_position_sphere_ata = 0;
	}

	if (vbo_normal_sphere_ata) {
		glDeleteBuffers(1, &vbo_normal_sphere_ata);
		vbo_normal_sphere_ata = 0;
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