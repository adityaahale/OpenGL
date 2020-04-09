#import <OpenGLES/ES3/gl.h>
#import <OpenGLES/ES3/glext.h>
#import "vmath.h"
#import "GLESView.h"
#import "Sphere.h"
using namespace vmath;
@implementation GLESView
{
    EAGLContext *eaglContext;
    GLuint defaultFramebuffer;
    GLuint colorRenderbuffer;
    GLuint depthRenderbuffer;
    
    id displayLink;
    NSInteger animationFrameInterval;
    BOOL isAnimating;
    GLint gWidth,gHeight;
    
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
	bool isPerVertex; 
	bool isPerFragment;
	
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

- (id) initWithFrame:(CGRect)frame
{
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
        CAEAGLLayer *eaglLayer=(CAEAGLLayer *) super.layer;
        
        eaglLayer.opaque=YES;
        eaglLayer.drawableProperties=[NSDictionary dictionaryWithObjectsAndKeys:[NSNumber numberWithBool:FALSE],
                                      kEAGLDrawablePropertyRetainedBacking, kEAGLColorFormatRGBA8,
                                      kEAGLDrawablePropertyColorFormat,nil];
        eaglContext = [[EAGLContext alloc]initWithAPI:kEAGLRenderingAPIOpenGLES3];
        if(eaglContext == nil)
        {
            [self release];
            return(nil);
        }
        [EAGLContext setCurrentContext:eaglContext];
        glGenFramebuffers(1, &defaultFramebuffer);
        glGenRenderbuffers(1, &colorRenderbuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, colorRenderbuffer);
        
        [eaglContext renderbufferStorage:GL_RENDERBUFFER fromDrawable:eaglLayer];
        
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, colorRenderbuffer);
        GLint backingWidth;
        GLint backingHeight;
        
        glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &backingWidth);
        glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &backingHeight);
        
        glGenRenderbuffers(1, &depthRenderbuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, backingWidth, backingHeight);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,  GL_RENDERBUFFER, depthRenderbuffer);
        if(glCheckFramebufferStatus(GL_FRAMEBUFFER)!=GL_FRAMEBUFFER_COMPLETE)
        {
            printf("Failed to create complete framebuffer object %x\n",glCheckFramebufferStatus(GL_FRAMEBUFFER));
            glDeleteFramebuffers(1, &defaultFramebuffer);
            glDeleteRenderbuffers(1, &colorRenderbuffer);
            glDeleteRenderbuffers(1, &depthRenderbuffer);
            return(nil);
        }
        printf("Render: %s | GL VERSION: %s | GLSL Version: %s \n",glGetString(GL_RENDERER),glGetString(GL_VERSION),
               glGetString(GL_SHADING_LANGUAGE_VERSION));
        //some hard coded initializations
        isAnimating = NO;
        animationFrameInterval = 60;
        
           //-----VERTEX SHADER----
    //Create vertex shader. Vertex shader specialist
    vertexShaderObject_ata = glCreateShader(GL_VERTEX_SHADER);
    
    const GLchar *vertexShaderSourceCodeAta =         
		"#version 300 es" \
        "\n" \
        "in vec4 vPosition;" \
		"in vec3 vNormal;" \
		"uniform mat4 u_model_matrix;" \
		"uniform mat4 u_view_matrix;" \
		"uniform mat4 u_projection_matrix;" \
		"uniform vec4 u_light0_position;" \
		"uniform vec4 u_light1_position;" \
		"uniform vec4 u_light2_position;" \
		"uniform mediump int u_lighting_enabled;" \
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
                printf( "Vertex shader compilation log:%s",
                        szInfoLogAta);
                free(szInfoLogAta);
                [self release];
                
            }
        }
    }
    //-----FRAGMENT SHADER----
    //Create fragment shader. Fragment shader specialist
    fragmentShaderObject_ata = glCreateShader(GL_FRAGMENT_SHADER);
    
    //source code of fragment shader
    const GLchar *fragmentShaderSourceCodeAta =      
		"#version 300 es" \
        "\n" \
        "precision highp float;" \
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
                printf("Fragment shader compilation log:%s",
                        szInfoLogAta);
                free(szInfoLogAta);
                [self release];
                
        }
    }
    }
    
    //Create shader program
    shaderProgramObject_ata = glCreateProgram();
    
    //attach shaders to the program
    glAttachShader(shaderProgramObject_ata, vertexShaderObject_ata);
    
    glAttachShader(shaderProgramObject_ata, fragmentShaderObject_ata);
    
    
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
                printf("Shader program linking log:%s", szInfoLogAta);
                [self release];
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
    glBindVertexArray(0); //done with cube vao
    
	// element vbo
	glGenBuffers(1, &vbo_element_sphere_ata);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_element_sphere_ata);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sphere_elements), sphere_elements, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	
        // set-up depth buffer
        
        // enable depth testing
        glEnable(GL_DEPTH_TEST);
        // depth test to do
        glDepthFunc(GL_LEQUAL);
        
       
        // set background color to which it will display even if it will empty. THIS LINE CAN BE IN drawRect().
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // blue
        perspectiveProjectionMatrix_ata = vmath::mat4::identity();

        
		//Gesture recognition
		
		UITapGestureRecognizer *singleTapGestureRecognizer=
		[[UITapGestureRecognizer alloc]initWithTarget:self action:@selector(onSingleTap:)];
		[singleTapGestureRecognizer setNumberOfTapsRequired:1];
		[singleTapGestureRecognizer setNumberOfTouchesRequired:1];
		[singleTapGestureRecognizer setDelegate:self];
		[self addGestureRecognizer :singleTapGestureRecognizer];
		
		UITapGestureRecognizer *doubleTapGestureRecognizer=[[UITapGestureRecognizer alloc]initWithTarget:self action:
		@selector(onDoubleTap:)];
		[doubleTapGestureRecognizer setNumberOfTapsRequired:2];
		[doubleTapGestureRecognizer setNumberOfTouchesRequired:1];
		[doubleTapGestureRecognizer setDelegate:self];
		[self addGestureRecognizer:doubleTapGestureRecognizer];
		
		[singleTapGestureRecognizer requireGestureRecognizerToFail:doubleTapGestureRecognizer];
		
		//Swipe gesture
		UISwipeGestureRecognizer *swipeGestureRecognizer=[[UISwipeGestureRecognizer alloc]initWithTarget:self action:
		@selector(onSwipe:)];
		[self addGestureRecognizer:swipeGestureRecognizer];
		//long press gesture
		UILongPressGestureRecognizer *longPressGestureRecognizer=[[UILongPressGestureRecognizer alloc]initWithTarget:self action:
		@selector(onLongPress:)];
		[self addGestureRecognizer:longPressGestureRecognizer];
	}
	return (self);
}
/*
//Only override draw rect
- (void)drawRect:(CGRect)rect
{
	
}
*/
+(Class)layerClass
{
    return([CAEAGLLayer class]);
}
-(void)drawView:(id)sender
{
    [EAGLContext setCurrentContext:eaglContext];
    glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebuffer);
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
		lightPosition[0] = myRadius * (cosf(angleSquareAta));
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
	glBindRenderbuffer(GL_RENDERBUFFER, colorRenderbuffer);
    [eaglContext presentRenderbuffer:GL_RENDERBUFFER];
	
	[self update]; //update 
    
}

-(void)update
{
	if (angleSquareAta > 360.0f) {
		angleSquareAta = 0.0f;
	}
	else
		angleSquareAta = angleSquareAta + 1.0f;

	if (angleTriangleAta > 360.0f) {
		angleTriangleAta = 0.0f;
	}
	else
		angleTriangleAta = angleTriangleAta + 1.0f;

}

-(void)layoutSubviews
{
    GLint width;
    GLint height;
    glBindRenderbuffer(GL_RENDERBUFFER, colorRenderbuffer);
    [eaglContext renderbufferStorage:GL_RENDERBUFFER fromDrawable:(CAEAGLLayer*)self.layer];
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &width);
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &height);
    glGenRenderbuffers(1, &depthRenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderbuffer);
    glViewport(0, 0, width, height);
    gWidth=width;
    gHeight=height;
    printf("Screen widhth: %d height:%d\n",width,height);
    perspectiveProjectionMatrix_ata = vmath::perspective(45.0f, ((GLfloat)width / (GLfloat)height),0.1f,100.0f);
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        printf("Failed to create complete framebuffer object:%x",glCheckFramebufferStatus(GL_FRAMEBUFFER));
    }
    [self drawView:nil];
}
-(void)startAnimation
{
    if(!isAnimating)
    {
        displayLink=[NSClassFromString(@"CADisplayLink")displayLinkWithTarget:self selector:@selector(drawView:)];
        [displayLink setPreferredFramesPerSecond:animationFrameInterval];
        [displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
        isAnimating=YES;
    }
}
-(void)stopAnimation
{
    if(isAnimating)
    {
        [displayLink invalidate];
        displayLink=nil;
        isAnimating=NO;
    }
}
//Become first responder
-(BOOL) acceptsFirstResponder
{
	//code
	return (YES);
}

- (void) touchesBegan: (NSSet *)touches withEvent:(UIEvent *) event
{
	
}

-(void)onSingleTap:(UITapGestureRecognizer *) gr
{
		if (isPerVertex == true) 
			{
				isPerVertex = false;
				isPerFragment = true;
			}
		else 
			{
				isPerVertex = true;
				isPerFragment = false;
			}
			
}

-(void) onDoubleTap:(UITapGestureRecognizer *)gr
{
		if (lightAta == false) 
			{
				lightAta = true;	
			}
			else 
			{
				lightAta = false;
			}
}

-(void)onSwipe:(UISwipeGestureRecognizer *)gr
{
	[self release];
	exit(0);
}

-(void)onLongPress:(UILongPressGestureRecognizer *)gr
{
    
}

-(void)dealloc
{
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
    
    if(depthRenderbuffer)
    {
        glDeleteRenderbuffers(1, &depthRenderbuffer);
        depthRenderbuffer=0;
    }
    if(colorRenderbuffer)
    {
        glDeleteRenderbuffers(1, &colorRenderbuffer);
        colorRenderbuffer=0;
    }
    if(defaultFramebuffer)
    {
        glDeleteFramebuffers(1, &defaultFramebuffer);
        defaultFramebuffer=0;
    }
    if([EAGLContext currentContext]==eaglContext)
    {
        [EAGLContext setCurrentContext:nil];
    }
    [eaglContext release];
    eaglContext=nil;
    
	[super dealloc];
}

@end
