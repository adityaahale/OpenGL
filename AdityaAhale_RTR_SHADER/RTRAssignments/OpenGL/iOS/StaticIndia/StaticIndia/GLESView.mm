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
    
	GLuint vao_horizontal_ata;
    GLuint vao_vertical_ata;
	GLuint vao_triline_ata;
	GLuint vao_N;
	GLuint vao_A;
	GLuint vao_D;
	GLuint vao_I;
	
    GLuint vbo_position;
	GLuint vbo_position_line;
	GLuint vbo_position_horizontal;
	GLuint vbo_position_i;
	GLuint vbo_position_d;
	GLuint vbo_position_n;
	GLuint vbo_position_a;
	
	GLuint vbo_color;
	GLuint vbo_color_triline;
	GLuint vbo_color_horizontal;
	GLuint vbo_color_i;
	GLuint vbo_color_d;
	GLuint vbo_color_n;
	GLuint vbo_color_a;

	GLuint mvpUniform_ata;
	GLuint alphaUniform;
	
    GLfloat angleTriangleAta;
    GLfloat angleSquareAta;
    GLfloat angleDHAta;
	GLfloat rot, xtrans, ytrans, xctrans, yctrans, yltrans;
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

- (id) initWithFrame:(CGRect)frame
{
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
        "in vec4 vColor;" \
        "out vec4 out_color;" \
        "uniform mat4 u_mvp_matrix;" \
        "void main(void)" \
        "{" \
        "gl_Position = u_mvp_matrix * vPosition;" \
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
        "in vec4 out_color;" \
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
    
    mvpUniform_ata = glGetUniformLocation(shaderProgramObject_ata, "u_mvp_matrix");
    //alphaUniform = glGetUniformLocation(ShaderProgramObject, "alpha");
	
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
	
	//triangles
	
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

    // set-up depth buffer
        
        // enable depth testing
        glEnable(GL_DEPTH_TEST);
        // depth test to do
        glDepthFunc(GL_LEQUAL);
		//glLineWidth(3.0f);		
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
    
    vmath::mat4 modelViewMatrixAta = vmath::mat4::identity();
    vmath::mat4 modelViewProjectionMatrixAta = vmath::mat4::identity();
    vmath::mat4 translationMatrixAta = mat4::identity();
    vmath::mat4 rotationMatrixAta = mat4::identity();
	
	//bind vao I
    glBindVertexArray(vao_I);
	glLineWidth(3.0f);
	translationMatrixAta = vmath::translate(-2.5f, 0.0f, -6.0f);
    modelViewMatrixAta = modelViewMatrixAta * translationMatrixAta;

    modelViewProjectionMatrixAta = perspectiveProjectionMatrix_ata * modelViewMatrixAta; //mind the order
    
    // Pass above model view matrix projection matrix to vertex shader 
    
    glUniformMatrix4fv(mvpUniform_ata, 1, GL_FALSE, modelViewProjectionMatrixAta);
    
    
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

    translationMatrixAta = vmath::translate(-1.5f, 0.0f, -6.0f);
    modelViewMatrixAta = modelViewMatrixAta * translationMatrixAta;
    modelViewProjectionMatrixAta = perspectiveProjectionMatrix_ata * modelViewMatrixAta; //mind the order
    glUniformMatrix4fv(mvpUniform_ata, 1, GL_FALSE, modelViewProjectionMatrixAta);
    
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
	translationMatrixAta = vmath::translate(1.0f, 0.0f, -6.0f);
    modelViewMatrixAta = modelViewMatrixAta * translationMatrixAta;

    modelViewProjectionMatrixAta = perspectiveProjectionMatrix_ata * modelViewMatrixAta; //mind the order
    
    // Pass above model view matrix projection matrix to vertex shader 
    
    glUniformMatrix4fv(mvpUniform_ata, 1, GL_FALSE, modelViewProjectionMatrixAta);
    
    
    // Draw either by glDrawTriangles() or glDrawArrays() or glDrawElements()
    glDrawArrays(GL_LINES, 0, 3); //3 is no of positions
    
    glBindVertexArray(0);
	
	
	//A vao
	glBindVertexArray(vao_A);
	modelViewMatrixAta = vmath::mat4::identity();
    modelViewProjectionMatrixAta = vmath::mat4::identity();
    translationMatrixAta = mat4::identity();
    rotationMatrixAta = mat4::identity();

    translationMatrixAta = vmath::translate(2.0f, 0.0f, -6.0f);
    modelViewMatrixAta = modelViewMatrixAta * translationMatrixAta;
    modelViewProjectionMatrixAta = perspectiveProjectionMatrix_ata * modelViewMatrixAta; //mind the order
    glUniformMatrix4fv(mvpUniform_ata, 1, GL_FALSE, modelViewProjectionMatrixAta);
    
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

    translationMatrixAta = vmath::translate(2.0f, 0.0f, -6.0f);
    modelViewMatrixAta = modelViewMatrixAta * translationMatrixAta;
    modelViewProjectionMatrixAta = perspectiveProjectionMatrix_ata * modelViewMatrixAta; //mind the order

	glUniformMatrix4fv(mvpUniform_ata, 1, GL_FALSE, modelViewProjectionMatrixAta);
    
    // Draw either by glDrawTriangles() or glDrawArrays() or glDrawElements()
    glDrawArrays(GL_LINES, 0, 6); //3 is no of positions

    glBindVertexArray(0);
    
    //stop using shaders
    glUseProgram(0);    
	glBindRenderbuffer(GL_RENDERBUFFER, colorRenderbuffer);
    [eaglContext presentRenderbuffer:GL_RENDERBUFFER];
	//[self update];
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