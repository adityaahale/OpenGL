#import <OpenGLES/ES3/gl.h>
#import <OpenGLES/ES3/glext.h>
#import "vmath.h"
#import "GLESView.h"
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
    GLuint vbo_normals_pyramid_ata;
    GLuint vbo_normal_sphere_ata;
    GLuint vbo_position_sphere_ata;
	GLuint vbo_element_sphere_ata;
	
    GLuint mvpUniform_ata;
   
    GLuint modelMatrixUniformAta, viewMatrixUniformAta, projectionMatrixUniformAta;
	GLuint light0PositionUniformAta, light1PositionUniformAta;
	GLuint l0aUniformAta,l1aUniformAta, l0dUniformAta, l0sUniformAta, l1dUniformAta, l1sUniformAta;
	GLuint kaUniformAta, kdUniformAta, ksUniformAta;
	GLuint material_shininess_uniform;
	GLuint lKeyPressedUniformAta;
	
    bool animateAta;
    bool lightAta;
    GLfloat angleTriangleAta;
    GLfloat angleSquareAta;
    
	vmath::mat4 perspectiveProjectionMatrix_ata;
	
	GLfloat light0AmbientAta[4];
	GLfloat light0DiffuseAta[4];
	GLfloat light0SpecularAta[4];
	GLfloat light1Ambient[4];
	GLfloat light1DiffuseAta[4];
	GLfloat light1SpecularAta[4];

	GLfloat light0Position[4];
	GLfloat light1Position[4];

	GLfloat material_ambient[4];
	GLfloat material_diffuse[4];
	GLfloat material_specular[4];
	GLfloat material_shininess;
	float myRadius;
}

- (id) initWithFrame:(CGRect)frame
{
	lightAta = false;
	animateAta = false;
	
    self=[super initWithFrame:frame];
	
	 myRadius=500.0f;
	
	light0AmbientAta[0]=0.0f;
	light0AmbientAta[1]=0.0f;
	light0AmbientAta[2]=0.0f;
	light0AmbientAta[3]=1.0f;
	
	light0DiffuseAta[0]=1.0f;
	light0DiffuseAta[1]=0.0f;
	light0DiffuseAta[2]=0.0f;
	light0DiffuseAta[3]=0.0f;
	
	light0SpecularAta[0]=1.0f;
	light0SpecularAta[1]=0.0f;
	light0SpecularAta[2]=0.0f;
	light0SpecularAta[3]=0.0f;
	
	light1DiffuseAta[0]=0.0f;
    light1DiffuseAta[1]=0.0f;
	light1DiffuseAta[2]=1.0f;
	light1DiffuseAta[3]=0.0f;
	
	
	light1SpecularAta[0]=0.0f;
	light1SpecularAta[1]=0.0f;
    light1SpecularAta[2]=1.0f;
	light1SpecularAta[3]=0.0f;
	
    
	light0Position[0]=2.0f;
	light0Position[1]=2.0f;
	light0Position[2]=0.0f;
	light0Position[3]=0.0f;
	
	light1Position[0]=-2.0f;
	light1Position[1]=2.0f;
	light1Position[2]=0.0f;
	light1Position[3]=1.0f;
	
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
		"uniform mediump int u_lighting_enabled;" \
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
		"out vec3 phong_ads_color;" \
		"void calculate_light_ads(vec3 La,vec3 Ld, vec3 Ls)"\
		"{" \
		"}"\
		"void main(void)" \
		"{" \
		"if(u_lighting_enabled == 1)" \
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
		"phong_ads_color = ambient0 + ambient1 + diffuse0 + diffuse1 + specular0 + specular1;"
		"}" \
		"else" \
		"{"
		"phong_ads_color = vec3(1.0, 1.0, 1.0);" \
		"}"\
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
        "in vec3 phong_ads_color;"
		"out vec4 FragColor;" \
		"void main(void)" \
		"{" \
		"FragColor = vec4(phong_ads_color, 1.0);" \
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
	
	l0aUniformAta = glGetUniformLocation(shaderProgramObject_ata, "u_La");
	l1aUniformAta = glGetUniformLocation(shaderProgramObject_ata, "u_L1a");
	
	l0dUniformAta = glGetUniformLocation(shaderProgramObject_ata, "u_L0d");
	l0sUniformAta = glGetUniformLocation(shaderProgramObject_ata, "u_L0s");

	l1dUniformAta = glGetUniformLocation(shaderProgramObject_ata, "u_L1d");
	l1sUniformAta = glGetUniformLocation(shaderProgramObject_ata, "u_L1s");

	light0PositionUniformAta = glGetUniformLocation(shaderProgramObject_ata, "u_light0_position");
	light1PositionUniformAta = glGetUniformLocation(shaderProgramObject_ata, "u_light1_position");
	
	kaUniformAta = glGetUniformLocation(shaderProgramObject_ata, "u_Ka");
	kdUniformAta = glGetUniformLocation(shaderProgramObject_ata, "u_Kd");
	ksUniformAta = glGetUniformLocation(shaderProgramObject_ata, "u_Ks");
	//shininess of material
	material_shininess_uniform = glGetUniformLocation(shaderProgramObject_ata, "u_material_shininess");
	
    // Vertices, colors, shader attribs, vbo, vao initializations
   const GLfloat pyramidVerticesAta[] = {
        
        // front
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

	const GLfloat pyramidNormalsAta[] = {
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
	
	
	glGenVertexArrays(1, &vao_pyramid_ata);
	glBindVertexArray(vao_pyramid_ata);
	//set triangle position
	glGenBuffers(1, &vbo_position_pyramid_ata);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position_pyramid_ata);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidVerticesAta), pyramidVerticesAta, GL_STATIC_DRAW);

	glVertexAttribPointer(ATA_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(ATA_ATTRIBUTE_POSITION);

	glGenBuffers(1, &vbo_normals_pyramid_ata);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_normals_pyramid_ata);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidNormalsAta), pyramidNormalsAta, GL_STATIC_DRAW);

	glVertexAttribPointer(ATA_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(ATA_ATTRIBUTE_NORMAL);


	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);  //done with pyramid vao
	
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
    glBindVertexArray(vao_pyramid_ata);
    
    translationMatrixAta = vmath::translate(0.0f, 0.0f, -5.0f);
    rotationMatrixAta = vmath::rotate(angleSquareAta,0.0f, 1.0f, 0.0f);
    rotationMatrixAta = rotationMatrixAta * vmath::rotate(angleSquareAta, 0.0f, 1.0f, 0.0f);  

    modelMatrixAta = modelMatrixAta * translationMatrixAta;
    modelMatrixAta = modelMatrixAta * rotationMatrixAta;
    
    glUniformMatrix4fv(modelMatrixUniformAta, 1, GL_FALSE, modelMatrixAta);
    glUniformMatrix4fv(viewMatrixUniformAta,1,GL_FALSE,viewMatrixAta);
	glUniformMatrix4fv(projectionMatrixUniformAta,1,GL_FALSE, perspectiveProjectionMatrix_ata);
    
	if (lightAta == true) {
		glUniform1i(lKeyPressedUniformAta, 1);
		glUniform3fv(l0aUniformAta, 1, light0AmbientAta);
		glUniform3fv(l1aUniformAta, 1, light1Ambient);
		
		glUniform3fv(l0dUniformAta, 1, light0DiffuseAta);
		glUniform3fv(l0sUniformAta, 1, light0SpecularAta);
		
		glUniform3fv(l1dUniformAta, 1, light1DiffuseAta);
		glUniform3fv(l1sUniformAta, 1, light1SpecularAta);
		
		glUniform4fv(light0PositionUniformAta, 1, light0Position);
		glUniform4fv(light1PositionUniformAta, 1, light1Position);
		
		glUniform3fv(kaUniformAta, 1, material_ambient);
		glUniform3fv(kdUniformAta, 1, material_diffuse);
		glUniform3fv(ksUniformAta, 1, material_specular);
		glUniform1f(material_shininess_uniform, material_shininess);
	}
	else {
		glUniform1i(lKeyPressedUniformAta, 0);
	}
	
	// Draw either by glDrawTriangles() or glDrawArrays() or glDrawElements()
	glDrawArrays(GL_TRIANGLES, 0, 12);
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
    if (animateAta == false) 
			{
				animateAta = true;
			}
			else 
			{
				animateAta = false;
			
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
    
    if (vbo_position_pyramid_ata) 
    {
        glDeleteBuffers(1, &vbo_position_pyramid_ata);
        vbo_position_pyramid_ata = 0;
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
