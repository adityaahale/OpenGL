#import <OpenGLES/ES3/gl.h>
#import <OpenGLES/ES3/glext.h>
#import "vmath.h"
#import "GLESView.h"
#define PI 3.14285714286
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
    
    GLuint vao_triangle_ata;
    GLuint vao_vertical_ata;
	GLuint vao_circle_ata;
	
    GLuint vbo_position;
	GLuint vbo_position_circle;
	GLuint vbo_position_triangle;
	
	GLuint vbo_color;
	GLuint vbo_color_circle;
	GLuint vbo_color_triangle;

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
    
    // Vertices, colors, shader attribs, vbo, vao initializations
    
	//GLfloat lineVerticesAta[8000];
	//GLfloat numberOfVertices = 1000 + 1;
	
	c = sqrtf((x2 - x1)*(x2 - x1) + (y2 - yone)*(y2 - yone)); //These are opposite lengths/sides of vertices
	a = sqrtf((x3 - x2)*(x3 - x2) + (y3 - y2)*(y3 - y2));
	b = sqrtf((x1 - x3)*(x1 - x3) + (yone - y3)*(yone - y3));
	summation = a + b + c;
	s = (float)(a + b + c) / 2.0f;

	area = sqrtf(s*(s - a)*(s - b)*(s - c));
	radius = area / s;

	
	//GLfloat circleVertices[3000];
	//GLfloat numberOfVertices = 1000 + 1;

	GLfloat doublePi = 2.0f * PI;

	GLfloat circleVerticesX[3000];
	GLfloat circleVerticesY[3000];
	GLfloat circleVerticesZ[3000];

	for (int i = 0; i < 3000; i++)
	{
		circleVerticesX[i] = (radius * cosf(i * doublePi / 1000));
		circleVerticesY[i] = (radius * sinf(i * doublePi / 1000));
		circleVerticesZ[i] = 0.0;
	}

	GLfloat allCircleVertices[3000 * 3];

	for (int i = 0; i < 3000; i++)
	{
		allCircleVertices[i * 3] = circleVerticesX[i];
		allCircleVertices[(i * 3) + 1] = circleVerticesY[i];
		allCircleVertices[(i * 3) + 2] = circleVerticesZ[i];
	}
	
	GLfloat circleColor[9000];
	for (int j = 0; j < 3000; j++)
	{
		circleColor[j * 3] = 1.0f;
		circleColor[(j * 3) + 1] = 1.0f;
		circleColor[(j * 3) + 2] = 1.0f;
	}

	glGenVertexArrays(1, &vao_circle_ata);
	glBindVertexArray(vao_circle_ata);

	glGenBuffers(1, &vbo_position_circle);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position_circle);
	glBufferData(GL_ARRAY_BUFFER, sizeof(allCircleVertices), allCircleVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(ATA_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(ATA_ATTRIBUTE_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &vbo_color_circle);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_color_circle);
	glBufferData(GL_ARRAY_BUFFER, sizeof(circleColor), circleColor, GL_STATIC_DRAW);

	glVertexAttribPointer(ATA_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(ATA_ATTRIBUTE_COLOR);

	glBindBuffer(GL_ARRAY_BUFFER, 0);


	glBindVertexArray(0);
	
	
    const GLfloat triangleVerticesAta[] = {
			0.0f,0.7f,
			-0.7f,-0.7f,
			0.7f,-0.7f
    };
    
    //create a vao
    glGenVertexArrays(1, &vao_triangle_ata);
    glBindVertexArray(vao_triangle_ata);
    
    glGenBuffers(1, &vbo_position_triangle);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_position_triangle);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVerticesAta), triangleVerticesAta, GL_STATIC_DRAW);
    
    glVertexAttribPointer(ATA_ATTRIBUTE_POSITION, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(ATA_ATTRIBUTE_POSITION);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    //vbo for triangle colors
    const GLfloat triangleColorsAta[] = {
		1.0f,1.0f,1.0f,
		1.0f,1.0f,1.0f,
		1.0f,1.0f,1.0f,
		1.0f,1.0f,1.0f,
		1.0f,1.0f,1.0f,
		1.0f,1.0f,1.0f,
	};

    glGenBuffers(1, &vbo_color_triangle);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_color_triangle);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangleColorsAta), triangleColorsAta, GL_STATIC_DRAW);

    glVertexAttribPointer(ATA_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(ATA_ATTRIBUTE_COLOR);
	
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);  //done with triangle vao
    
    
    //square
    const GLfloat squareVerticesAta[] = {
		0.0f,0.7f,0.0f,
		0.0f,-0.7f,0.0f
	};

	const GLfloat verColor[] = {
		1.0f,1.0f,1.0f,
		1.0f,1.0f,1.0f
	};
	
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glGenVertexArrays(1, &vao_vertical_ata);
    glBindVertexArray(vao_vertical_ata);

    glGenBuffers(1, &vbo_position);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
    glBufferData(GL_ARRAY_BUFFER, sizeof(squareVerticesAta), squareVerticesAta, GL_STATIC_DRAW);

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
    
    vmath::mat4 modelViewMatrixAta = vmath::mat4::identity();
    vmath::mat4 modelViewProjectionMatrixAta = vmath::mat4::identity();
    vmath::mat4 translationMatrixAta = mat4::identity();
    vmath::mat4 rotationMatrixAta = mat4::identity();
	
    translationMatrixAta = vmath::translate(xtrans, ytrans, -3.0f);
    modelViewMatrixAta = modelViewMatrixAta * translationMatrixAta;
    rotationMatrixAta = vmath::rotate(angleDHAta, 0.0f, 1.0f, 0.0f);
    modelViewMatrixAta = modelViewMatrixAta * rotationMatrixAta ;
    modelViewProjectionMatrixAta = perspectiveProjectionMatrix_ata * modelViewMatrixAta; //mind the order
    
    // Pass above model view matrix projection matrix to vertex shader in 'u_mvp_matrix' shader variable
    
    glUniformMatrix4fv(mvpUniform_ata, 1, GL_FALSE, modelViewProjectionMatrixAta);
    
    //bind vao
    glBindVertexArray(vao_triangle_ata);
    // Draw either by glDrawTriangles() or glDrawArrays() or glDrawElements()
    glDrawArrays(GL_LINE_LOOP, 0, 3); //3 is no of positions
    
    glBindVertexArray(0);
	
    //bind vertical vao
    glBindVertexArray(vao_vertical_ata);
    
    modelViewMatrixAta = vmath::mat4::identity();
    modelViewProjectionMatrixAta = vmath::mat4::identity();
    translationMatrixAta = mat4::identity();
    rotationMatrixAta = mat4::identity();

    translationMatrixAta = vmath::translate(0.0f, yltrans, -3.0f);
    modelViewMatrixAta = modelViewMatrixAta * translationMatrixAta;
    modelViewProjectionMatrixAta = perspectiveProjectionMatrix_ata * modelViewMatrixAta; //mind the order
    glUniformMatrix4fv(mvpUniform_ata, 1, GL_FALSE, modelViewProjectionMatrixAta);
    
    // Draw either by glDrawTriangles() or glDrawArrays() or glDrawElements()
    glDrawArrays(GL_LINES, 0, 4); //3 is no of positions

    glBindVertexArray(0);
	
	//bind circle vao
    glBindVertexArray(vao_circle_ata);
    
    modelViewMatrixAta = vmath::mat4::identity();
    modelViewProjectionMatrixAta = vmath::mat4::identity();
    translationMatrixAta = mat4::identity();
    rotationMatrixAta = mat4::identity();

	tx = ((a*x1) + (b*x2) + (c*x3)) / summation;
	ty = ((a*yone) + (b*y2) + (c*y3)) / summation;
	
    translationMatrixAta = vmath::translate(tx+xctrans, ty-yctrans, -3.0f);
    modelViewMatrixAta = modelViewMatrixAta * translationMatrixAta;
    modelViewProjectionMatrixAta = perspectiveProjectionMatrix_ata * modelViewMatrixAta; //mind the order
    rotationMatrixAta = vmath::rotate(angleDHAta, 0.0f, 1.0f, 0.0f);
    modelViewMatrixAta = modelViewMatrixAta * rotationMatrixAta ;
    
    modelViewProjectionMatrixAta = perspectiveProjectionMatrix_ata * modelViewMatrixAta; //mind the order
    
	glUniformMatrix4fv(mvpUniform_ata, 1, GL_FALSE, modelViewProjectionMatrixAta);
    
    // Draw either by glDrawTriangles() or glDrawArrays() or glDrawElements()
    glDrawArrays(GL_LINE_LOOP, 0, 3000); //3 is no of positions

    glBindVertexArray(0);
	
    
    //stop using shaders
    glUseProgram(0);    
	glBindRenderbuffer(GL_RENDERBUFFER, colorRenderbuffer);
    [eaglContext presentRenderbuffer:GL_RENDERBUFFER];
	[self update];
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
	
	angleDHAta = angleDHAta + 1.0f;
	if (angleDHAta >= 360)
	{
		angleDHAta = 0.0f;
	}

    if (xtrans <= 0.0)
    {
        xtrans = xtrans + 0.009;
        ytrans = ytrans + 0.009;
    }
    
    if (xtrans >= 0.0)
    {
        if (xctrans > 0.0)
        {
            xctrans = xctrans - 0.009;
            yctrans = yctrans + 0.009;
            
        }
    }
    
    if (xtrans > 0.0)
    {
        if (xctrans < 0.0)
        {
            if (yltrans >= 0.0)
            {
                yltrans = yltrans - 0.009;
            }
        }
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
