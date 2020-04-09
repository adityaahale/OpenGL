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
    
    GLuint VaoAta;
	GLuint Vbo_PositionAta;
	GLuint Vbo_ColorAta;
    
	GLuint samplerUniformAta; 
	GLuint textureMarbleStoneAta;
	GLuint modelUniformAta, viewUniformAta, projectionUniformAta;
	GLuint lightPositionUniformAta;
	GLuint lKeyPressUniformAta;
	GLuint La_uniform, Ld_uniform, Ls_uniform;
	GLuint Ka_uniform, Kd_uniform, Ks_uniform;
	GLuint material_shininess_uniform;
	
	GLfloat lightAmbient[4];
	GLfloat lightDiffuse[4];
	GLfloat lightSpecular[4];
	GLfloat lightPosition[4];

	GLfloat material_ambient[4];
	GLfloat material_diffuse[4];
	GLfloat material_specular[4];
	GLfloat material_shininess;
	bool lightAta;
	GLfloat angleSqAta;
	vmath::mat4 perspectiveProjectionMatrix_ata;
}

- (id) initWithFrame:(CGRect)frame
{
	lightAta = false;
	
    self=[super initWithFrame:frame];
	
	lightAmbient[0]=0.0f;
	lightAmbient[1]=0.0f;
	lightAmbient[2]=0.0f;
	lightAmbient[3]=1.0f;
	
	lightDiffuse[0]=1.0f;
	lightDiffuse[1]=1.0f;
	lightDiffuse[2]=1.0f;
	lightDiffuse[3]=1.0f;
	
	lightSpecular[0]=1.0f;
	lightSpecular[1]=1.0f;
	lightSpecular[2]=1.0f;
	lightSpecular[3]=1.0f;
	
	lightPosition[0]=500.0f;
	lightPosition[1]=500.0f;
	lightPosition[2]=500.0f;
	lightPosition[3]=1.0f;
	
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
		"in vec2 vTexture0_Coord;" \
		"out vec2 out_texture0_coord;" \
		"uniform mat4 u_model_matrix;" \
		"uniform mat4 u_view_matrix;" \
		"uniform mat4 u_projection_matrix;" \
		"uniform vec4 u_light_position;" \
		"uniform mediump int u_lighting_enabled;" \
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
		"out_texture0_coord = vTexture0_Coord;" \
		"out_color = vColor;" \
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
		"FragColor = vec4(phong_ads_color, 1.0) * texture(u_texture0_sampler, out_texture0_coord) * out_color;" \
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
    glBindAttribLocation(shaderProgramObject_ata, ATA_ATTRIBUTE_TEXTURE0, "vTexture0_Coord");
    glBindAttribLocation(shaderProgramObject_ata, ATA_ATTRIBUTE_NORMAL, "vNormal");
    glBindAttribLocation(shaderProgramObject_ata, ATA_ATTRIBUTE_COLOR, "vColor");
    
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
    //texture_sampler_uniform_ata = glGetUniformLocation(shaderProgramObject_ata, "u_texture0_sampler");
	samplerUniformAta = glGetUniformLocation(shaderProgramObject_ata, "u_texture0_sampler");
	modelUniformAta = glGetUniformLocation(shaderProgramObject_ata, "u_model_matrix");
	viewUniformAta = glGetUniformLocation(shaderProgramObject_ata, "u_view_matrix");
	projectionUniformAta = glGetUniformLocation(shaderProgramObject_ata, "u_projection_matrix");
	lKeyPressUniformAta = glGetUniformLocation(shaderProgramObject_ata, "u_lighting_enabled"); //L/1 key pressed or not
	La_uniform = glGetUniformLocation(shaderProgramObject_ata, "u_La");
	Ld_uniform = glGetUniformLocation(shaderProgramObject_ata, "u_Ld");
	Ls_uniform = glGetUniformLocation(shaderProgramObject_ata, "u_Ls");
	lightPositionUniformAta = glGetUniformLocation(shaderProgramObject_ata, "u_light_position");

	//material ambient color intensity
	Ka_uniform = glGetUniformLocation(shaderProgramObject_ata, "u_Ka");
	Kd_uniform = glGetUniformLocation(shaderProgramObject_ata, "u_Kd");
	Ks_uniform = glGetUniformLocation(shaderProgramObject_ata, "u_Ks");
	//shininess of material
	material_shininess_uniform = glGetUniformLocation(shaderProgramObject_ata, "u_material_shininess");


   textureMarbleStoneAta = [self loadTextureFromBMPFile:@"marble":@"bmp"];
    //Texture_StoneAta = [self loadTextureFromBMPFile:"Stone.bmp"];
	
    // Vertices, colors, shader attribs, vbo, vao initializations
    
	const GLfloat cubeData[] = {
        //top
        1.0f, 1.0f, -1.0f,0.0f, 1.0f, 0.0f,0.0f, 1.0f, 0.0f,0.0f, 0.0f,
        -1.0f, 1.0f, -1.0f,0.0f, 1.0f, 0.0f,0.0f, 1.0f, 0.0f,1.0f, 0.0f,
        -1.0f, 1.0f, 1.0f,0.0f, 1.0f, 0.0f,0.0f, 1.0f, 0.0f,1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,0.0f, 1.0f, 0.0f,0.0f, 1.0f, 0.0f,0.0f, 1.0f,
        //bottom
        1.0f, -1.0f, -1.0f,1.0f, 0.5f, 0.0f,0.0f, -1.0f, 0.0f,0.0f, 0.0f,
        -1.0f, -1.0f, -1.0f,1.0f, 0.5f, 0.0f,0.0f, -1.0f, 0.0f,1.0f, 0.0f,
        -1.0f, -1.0f,  1.0f,1.0f, 0.5f, 0.0f,0.0f, -1.0f, 0.0f,1.0f, 1.0f,
        1.0f, -1.0f,  1.0f,1.0f, 0.5f, 0.0f,0.0f, -1.0f, 0.0f,0.0f, 1.0f,
        
        //front
        1.0f, 1.0f, 1.0f,1.0f, 0.0f, 0.0f,0.0f, 0.0f, 1.0f,0.0f, 0.0f,
        -1.0f, 1.0f, 1.0f,1.0f, 0.0f, 0.0f,0.0f, 0.0f, 1.0f,1.0f, 0.0f,
        -1.0f, -1.0f, 1.0f,1.0f, 0.0f, 0.0f,0.0f, 0.0f, 1.0f,1.0f, 1.0f,
        1.0f, -1.0f, 1.0f,1.0f, 0.0f, 0.0f,0.0f, 0.0f, 1.0f,0.0f, 1.0f,
        
        // back
        
        1.0f, 1.0f, -1.0f,1.0f, 1.0f, 0.0f,0.0f, 0.0f, -1.0f,0.0f, 0.0f,
        -1.0f, 1.0f, -1.0f,1.0f, 1.0f, 0.0f,0.0f, 0.0f, -1.0f,1.0f, 0.0f,
        -1.0f, -1.0f, -1.0f,1.0f, 1.0f, 0.0f,0.0f, 0.0f, -1.0f,1.0f, 1.0f,
        1.0f, -1.0f, -1.0f,1.0f, 1.0f, 0.0f,0.0f, 0.0f, -1.0f,0.0f, 1.0f,
        
        // right
        
        1.0f, 1.0f, -1.0f,0.0f, 0.0f, 1.0f,-1.0f, 0.0f, 0.0f,0.0f, 0.0f,
        1.0f, 1.0f, 1.0f,0.0f, 0.0f, 1.0f,-1.0f, 0.0f, 0.0f,1.0f, 0.0f,
        1.0f, -1.0f, 1.0f,0.0f, 0.0f, 1.0f,-1.0f, 0.0f, 0.0f,1.0f, 1.0f,
        1.0f, -1.0f, -1.0f,0.0f, 0.0f, 1.0f,-1.0f, 0.0f, 0.0f,0.0f, 1.0f,
        
        // left
        -1.0f, 1.0f, 1.0f,1.0f, 0.0f, 1.0f,1.0f, 0.0f, 0.0f,0.0f, 0.0f,
        -1.0f, 1.0f, -1.0f,1.0f, 0.0f, 1.0f,1.0f, 0.0f, 0.0f,1.0f, 0.0f,
        -1.0f, -1.0f, -1.0f,1.0f, 0.0f, 1.0f,1.0f, 0.0f, 0.0f,1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 1.0f,1.0f, 0.0f, 0.0f,0.0f, 1.0f
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

	glVertexAttribPointer(ATA_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(ATA_ATTRIBUTE_COLOR);

	glVertexAttribPointer(ATA_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(ATA_ATTRIBUTE_NORMAL);

	glVertexAttribPointer(ATA_ATTRIBUTE_TEXTURE0, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(9 * sizeof(float)));
	glEnableVertexAttribArray(ATA_ATTRIBUTE_TEXTURE0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	
        // set-up depth buffer
        
        // enable depth testing
        glEnable(GL_DEPTH_TEST);
        // depth test to do
        glDepthFunc(GL_LEQUAL);
        //glEnable(GL_CULL_FACE);
       
        // set background color to which it will display even if it will empty. THIS LINE CAN BE IN drawRect().
        glClearColor(0.25f, 0.25f, 0.25f, 0.0f); // blue
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

-(GLuint) loadTextureFromBMPFile:(NSString *)texFileName :(NSString *)extension
{
    NSLog(@"Tex file name: %@ extension:%@", texFileName, extension);
    NSString *textureFileNameWithPath=[[NSBundle mainBundle]pathForResource:texFileName ofType:extension];
    UIImage *bmpImage=[[UIImage alloc]initWithContentsOfFile:textureFileNameWithPath];
    
    if(!bmpImage)
    {
        NSLog(@"Can't find %@", textureFileNameWithPath);
        return(0);
    }
    CGImageRef cgImage = bmpImage.CGImage;
    
    int w = (int) CGImageGetWidth(cgImage);
    int h = (int) CGImageGetHeight(cgImage);
    CFDataRef imageData = CGDataProviderCopyData(CGImageGetDataProvider(cgImage));
    
    void *pixels = (void*) CFDataGetBytePtr(imageData);
    
    GLuint bmpTextureAta;
    glGenTextures(1, &bmpTextureAta);
    
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glBindTexture(GL_TEXTURE_2D, bmpTextureAta);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE,pixels);
    
    glGenerateMipmap(GL_TEXTURE_2D);
    CFRelease(imageData);
    return (bmpTextureAta);
}

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
    if (lightAta == true)
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

	
	translationMatrixAta = translate(0.0f, 0.0f, -7.0f);
	rotationMatrixAta = rotate(angleSqAta, 1.0f, 0.0f, 0.0f);
	rotationMatrixAta *= rotate(angleSqAta, 0.0f, 1.0f, 0.0f);
	rotationMatrixAta *= rotate(angleSqAta, 0.0f, 0.0f, 1.0f);
	modelMatrixAta = modelMatrixAta * translationMatrixAta * rotationMatrixAta;
	
	//modelViewProjectionMatrixAta = PerspectiveProjectionMatrixAta * modelViewMatrixAta;

	glUniformMatrix4fv(modelUniformAta, 1, GL_FALSE, modelMatrixAta);
	glUniformMatrix4fv(viewUniformAta, 1, GL_FALSE, viewMatrixAta);
	glUniformMatrix4fv(projectionUniformAta, 1, GL_FALSE, perspectiveProjectionMatrix_ata);
	
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
    
    //stop using shaders
    glUseProgram(0);    
	glBindRenderbuffer(GL_RENDERBUFFER, colorRenderbuffer);
    [eaglContext presentRenderbuffer:GL_RENDERBUFFER];
	[self update];
}

-(void)update
{
	if (angleSqAta > 360.0f) {
		angleSqAta = 0.0f;
	}
	else
		angleSqAta = angleSqAta + 1.0f;

	
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
    if (lightAta == false) 
	{
		lightAta = true;	
	}
	else 
	{
		lightAta = false;
	}
}

-(void) onDoubleTap:(UITapGestureRecognizer *)gr
{

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
    if (VaoAta)
     {
        glDeleteVertexArrays(1, &VaoAta);
        VaoAta = 0;
     }

    if (Vbo_PositionAta) 
    {
        glDeleteBuffers(1, &Vbo_PositionAta);
        Vbo_PositionAta = 0;
    }
    
	if (textureMarbleStoneAta) {
		glDeleteTextures(1, &textureMarbleStoneAta);
		textureMarbleStoneAta = 0;
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
