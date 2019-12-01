package com.aditya.texture;

import android.content.Context;
import android.graphics.Color;
import android.widget.TextView;
import android.view.Gravity;
import android.view.MotionEvent;
import android.view.GestureDetector;
import android.view.GestureDetector.OnGestureListener;
import android.view.GestureDetector.OnDoubleTapListener;



//For OpenGLES
import android.opengl.GLSurfaceView;
import javax.microedition.khronos.opengles.GL10;
import javax.microedition.khronos.egl.EGLConfig;
import android.opengl.GLES32; 			
import android.opengl.Matrix;

//Java nio(non blocking i/o
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;

//texture related imports
import android.graphics.BitmapFactory; 
import android.graphics.Bitmap; // For PNG image
import android.opengl.GLUtils;  //for texImage2D() call

//A view for OpenGLES3 graphics which also receives touch events

public class GLESView extends GLSurfaceView implements GLSurfaceView.Renderer, OnGestureListener, OnDoubleTapListener
{
	private final Context context;
	private GestureDetector gestureDetector;
	
	
	//new class memebers
	private int vertexShaderObject;
	private int fragmentShaderObject;
	private int shaderProgramObject;
	
	private int[] vao_pyramid = new int[1];
	private int[] vao_cube = new int[1];
	private int[] vbo_position = new int[1];
	private int[] vbo_color = new int[1];
	private int[] vbo_texure = new int[1];
	private int mvpUniform;
	private int texture0_sampler_uniform;
	
	private int[] texture_kundali = new int[1];
	private int[] texture_stone = new int[1];
	private float perspectiveProjectionMatrix[]=new float[16];
	private float angle_pyramid;
	private float angle_cube;
	private float zAxis = -5.0f;
	public GLESView(Context context){
			super(context);
			this.context = context;
			//OpenGLES version negotiation step. Set EGLContext to current supported version of OpenGL-ES
			setEGLContextClientVersion(3);
			//set renderer for drawing on the GLSurfaceView
			setRenderer(this);
			setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY); //on screen rendering
			gestureDetector = new GestureDetector(context, this, null, false);
			gestureDetector.setOnDoubleTapListener(this);
	}
	
	
	//Overriden method of GLSurfaceView.Renderer(init code)
	@Override
	public void onSurfaceCreated(GL10 gl,EGLConfig config){
		//OpenGL-ES version check
		String glesVersion = gl.glGetString(GL10.GL_VERSION);
		System.out.println("RTR:" + glesVersion);
		
		String glslVersion = gl.glGetString(GLES32.GL_SHADING_LANGUAGE_VERSION);
		System.out.println("RTR: Shading lang version:"+ glslVersion);
		
		initialize(gl);
	}
	@Override
	public void onSurfaceChanged(GL10 unused, int width, int height) //like resize
    {
			resize(width,height);
	}

	@Override
	public void onDrawFrame(GL10 unused){
		draw();
	}
	@Override
	public boolean onTouchEvent(MotionEvent event){
			
			int eventAction = event.getAction(); 
			if(!gestureDetector.onTouchEvent(event)){
				super.onTouchEvent(event);			
			}
			return true;
	}
	@Override //abstract method from OnDoubleTapListener
	public boolean onDoubleTap(MotionEvent e){		
		return true;
	}
	@Override //abstract method from OnDoubleTapListener
	public boolean onDoubleTapEvent(MotionEvent e){
		//nothing to do for now. Already handled in onDoubleTap
		return true;
	}
	
	@Override //abstract method from OnDoubleTapListener
	public boolean onSingleTapConfirmed(MotionEvent e){
		return true;
	}
	
	@Override //abstract method from OnGestureListener
	public boolean onDown(MotionEvent e){
		//already handled in onSingleTapConfirmed
		return true;
	}
	
	@Override //abstract method from OnGestureListener
	public boolean onFling(MotionEvent e1, MotionEvent e2, float velocityX, float velocityY){		
			return true;
	}
	
	@Override //method from OnGestureListener
	public void onLongPress(MotionEvent e){		
	
	}
	
	@Override //method from OnGestureListener
	public boolean onScroll(MotionEvent e1, MotionEvent e2, float distanceX, float distanceY){	
		uninitialize();
		System.out.println("RTR:on scroll exit");
		System.exit(0);
		return true;
	}
	
	@Override //method from OnGestureListener
	public void onShowPress(MotionEvent e){
		//nothing to do
	}
	
	@Override //method from OnGestureListener
	public boolean onSingleTapUp(MotionEvent e){
		return true;
	}
	
	private void initialize(GL10 gl){
		//***** VERTEX SHADER ****
		vertexShaderObject = GLES32.glCreateShader(GLES32.GL_VERTEX_SHADER);
		
		//vertex shader source code
		final String vertexShaderSourceCode =  String.format(
		"#version 320 es" +
		"\n" +
		"in vec4 vPosition;" +
		"in vec2 vTexture0_Coord;" +
		"out vec2 out_texture0_coord;" +
		"uniform mat4 u_mvp_matrix;" +
		"void main(void)" +
		"{" +
		"gl_Position = u_mvp_matrix * vPosition;" +		
		"out_texture0_coord = vTexture0_Coord;" +
		"}" 
		); 
		
		//provide above source code to shader object
		GLES32.glShaderSource(vertexShaderObject, vertexShaderSourceCode);
		
		GLES32.glCompileShader(vertexShaderObject);
		//error checking
		int[] iShaderCompiledStatus = new int[1]; //taken as array, bcz this will be a out param
		int[] iInfoLogLength = new int[1];
		String szInfoLog = null;
		GLES32.glGetShaderiv(vertexShaderObject, GLES32.GL_COMPILE_STATUS, iShaderCompiledStatus, 0); //note additional zero
		if(iShaderCompiledStatus[0] == GLES32.GL_FALSE){
			GLES32.glGetShaderiv(vertexShaderObject, GLES32.GL_INFO_LOG_LENGTH, iInfoLogLength, 0);
			if(iInfoLogLength[0] > 0){
				szInfoLog = GLES32.glGetShaderInfoLog(vertexShaderObject);
				System.out.println("RTR: Vertex shader compilation log:" + szInfoLog);
				uninitialize();
				System.exit(0);
			}
		}
		
		//***** FRAGMENT SHADER ****
		//create fragment shader
		fragmentShaderObject = GLES32.glCreateShader(GLES32.GL_FRAGMENT_SHADER);
		//fragment shader source code
		final String fragmentShaderSourceCode = String.format(
		"#version 320 es"+
		"\n"+
		"precision highp float;"+
		"in vec4 out_color;" +
		"in vec2 out_texture0_coord;" +
		"uniform sampler2D u_texture0_sampler;" +
		"out vec4 FragColor;" +
		"void main(void)"+
		"{"+
		"FragColor = texture(u_texture0_sampler, out_texture0_coord);" +
		"}"
		);
		
		//Provide fragment shader source code to shader
		GLES32.glShaderSource(fragmentShaderObject,fragmentShaderSourceCode);
		//check compilation erros in fragment shaders
		GLES32.glCompileShader(fragmentShaderObject);
		//re-initialize variables
		iShaderCompiledStatus[0] = 0;
		iInfoLogLength[0] = 0;
		szInfoLog = null;
		GLES32.glGetShaderiv(fragmentShaderObject, GLES32.GL_COMPILE_STATUS, iShaderCompiledStatus, 0); //note additional zero
		if(iShaderCompiledStatus[0] == GLES32.GL_FALSE){
			GLES32.glGetShaderiv(fragmentShaderObject, GLES32.GL_INFO_LOG_LENGTH, iInfoLogLength, 0);
			if(iInfoLogLength[0] > 0){
				szInfoLog = GLES32.glGetShaderInfoLog(fragmentShaderObject);
				System.out.println("RTR: Fragment shader compilation log:" + szInfoLog);
				uninitialize();
				System.exit(0);
			}
		}
		
		shaderProgramObject = GLES32.glCreateProgram();
		GLES32.glAttachShader(shaderProgramObject, vertexShaderObject);
		GLES32.glAttachShader(shaderProgramObject, fragmentShaderObject);
		
		//pre-link binding of shader program object with vertex shader attribute
		GLES32.glBindAttribLocation(shaderProgramObject, GLESMacros.AMC_ATTRIBUTE_POSITION, "vPosition");
		GLES32.glBindAttribLocation(shaderProgramObject, GLESMacros.AMC_ATTRIBUTE_TEXTURE0, "vTexture0_Coord");
		//link program
		GLES32.glLinkProgram(shaderProgramObject);
		int[] iShaderProgramLinkStatus = new int[1]; //taken as array, bcz this will be a out param
		iInfoLogLength[0] = 0;
		szInfoLog = null;
		GLES32.glGetProgramiv(shaderProgramObject, GLES32.GL_LINK_STATUS, iShaderProgramLinkStatus, 0); //note additional zero
		if(iShaderProgramLinkStatus[0] == GLES32.GL_FALSE){
			GLES32.glGetProgramiv(shaderProgramObject, GLES32.GL_INFO_LOG_LENGTH, iInfoLogLength, 0);
			if(iInfoLogLength[0] > 0){
				szInfoLog = GLES32.glGetProgramInfoLog(shaderProgramObject);
				System.out.println("RTR: Shader program link log:" + szInfoLog);
				uninitialize();
				System.exit(0);
			}
		}
		
		//get MVP uniform location
		mvpUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_mvp_matrix");
		texture0_sampler_uniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_texture0_sampler");
		
		//load texture images
		
		texture_stone[0] = loadGLTexture(R.raw.stone);
		texture_kundali[0] = loadGLTexture(R.raw.kundali);
				
		
		final float pyramidVertices[] = new float[]{
			0.0f, 1.0f, 0.0f, //apex of the pyramid
		-1.0f, -1.0f, 1.0f, //left-bottom
		1.0f, -1.0f, 1.0f, //right-bottom
		//right face
		0.0f, 1.0f, 0.0f, //apex
		1.0f, -1.0f, 1.0f,//left bottom
		1.0f, -1.0f, -1.0f, //right bottom
		//back face
		0.0f, 1.0f, 0.0f, //apex
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		//left face
		0.0f, 1.0f, 0.0f, //apex
		-1.0f, -1.0f, -1.0f, //left bottom
		-1.0f, -1.0f, 1.0f //right bottom
		};
		
		float pyramidTexcoords[] = new float[]{
		0.5f, 1.0f, //front top
		0.0f, 0.0f, //front left
		1.0f, 0.0f, //front right

		0.5f, 1.0f, //right top
		1.0f, 0.0f, //right left
		0.0f, 0.0f, //right right

		0.5f, 1.0f, //back top
		1.0f, 0.0f, //back left
		0.0f, 0.0f, //back right

		0.5f, 1.0f, //left top
		0.0f, 0.0f, //left left
		1.0f, 0.0f //left right

	};
		
		GLES32.glGenVertexArrays(1, vao_pyramid, 0); 	//NOTE additional zero
		GLES32.glBindVertexArray(vao_pyramid[0]);		//NOTE how it is used
		
		//vbo for positions
		GLES32.glGenBuffers(1, vbo_position, 0); 		//Creates a vbo. 
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position[0]);
		
		//Create a buffer to pass our float data to GPU in native fashion
		ByteBuffer byteBuffer =  ByteBuffer.allocateDirect(pyramidVertices.length * 4); //4 is size of float. This is global mem allocation. All memory location will be initialized to zero
		byteBuffer.order(ByteOrder.nativeOrder()); //Detect native machine endianess and use it
		FloatBuffer verticesBuffer = byteBuffer.asFloatBuffer();
		verticesBuffer.put(pyramidVertices); //fill the data
		verticesBuffer.position(0); //Zero indicates, from where to start using the data
		
		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, pyramidVertices.length * 4, verticesBuffer, GLES32.GL_STATIC_DRAW);
		
		GLES32.glVertexAttribPointer(GLESMacros.AMC_ATTRIBUTE_POSITION, 3, GLES32.GL_FLOAT, false, 0, 0);
		
		GLES32.glEnableVertexAttribArray(GLESMacros.AMC_ATTRIBUTE_POSITION);
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
		
		
		//vbo for texture	
		GLES32.glGenBuffers(1, vbo_texure, 0); 		//Creates a vbo. 
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_texure[0]);
		
		//Create a buffer to pass our float data to GPU in native fashion
		byteBuffer =  ByteBuffer.allocateDirect(pyramidTexcoords.length * 4); //4 is size of float. This is global mem allocation. All memory location will be initialized to zero
		byteBuffer.order(ByteOrder.nativeOrder()); //Detect native machine endianess and use it
		verticesBuffer = byteBuffer.asFloatBuffer();
		verticesBuffer.put(pyramidTexcoords); //fill the data
		verticesBuffer.position(0); //Zero indicates, from where to start using the data
		
		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, pyramidTexcoords.length * 4, verticesBuffer, GLES32.GL_STATIC_DRAW);
		
		GLES32.glVertexAttribPointer(GLESMacros.AMC_ATTRIBUTE_TEXTURE0, 2, GLES32.GL_FLOAT, false, 0, 0);
		
		GLES32.glEnableVertexAttribArray(GLESMacros.AMC_ATTRIBUTE_TEXTURE0);
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
	
		final float cubeVertices[] = new float[]{
			//front face
			-1.0f, 1.0f, 1.0f, //left top
			-1.0f, -1.0f, 1.0f,  //left bottom
			1.0f, -1.0f, 1.0f,  //right bottom
			1.0f, 1.0f, 1.0f, //right top
			//right face
			1.0f, 1.0f, 1.0f,//left top
			1.0f, -1.0f, 1.0f, //left bottom
			1.0f, -1.0f, -1.0f, //right bottom
			1.0f, 1.0f, -1.0f,//right top

			//back face
			1.0f, 1.0f, -1.0f,//left top
			1.0f, -1.0f, -1.0f,//left bottom
			-1.0f, -1.0f, -1.0f, //right bottom
			-1.0f, 1.0f, -1.0f, //right top

			//left face
			- 1.0f, 1.0f, -1.0f,
			-1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f, 1.0f,
			-1.0f, 1.0f, 1.0f,

			//top face
			-1.0f, 1.0f, -1.0f,
			-1.0f, 1.0f, 1.0f,
			1.0f, 1.0f, 1.0f,
			1.0f, 1.0f, -1.0f,

			//bottom face
			-1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f, 1.0f,
			1.0f, -1.0f, 1.0f,
			1.0f, -1.0f, -1.0f
		};
		
		//cube texture coords
	 float cubeTexcoords[] = new float[]{
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,

		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,

		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,

		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,

		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,

		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f
	};

		
		GLES32.glGenVertexArrays(1, vao_cube, 0); 	//NOTE additional zero
		GLES32.glBindVertexArray(vao_cube[0]);		//NOTE how it is used
		
		//vbo for positions
		GLES32.glGenBuffers(1, vbo_position, 0); 		//Creates a vbo. 
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position[0]);
		
		//Create a buffer to pass our float data to GPU in native fashion
		byteBuffer =  ByteBuffer.allocateDirect(cubeVertices.length * 4); //4 is size of float.
		byteBuffer.order(ByteOrder.nativeOrder()); //Detect native machine endianess and use it
		verticesBuffer = byteBuffer.asFloatBuffer();
		verticesBuffer.put(cubeVertices); //fill the data
		verticesBuffer.position(0); //Zero indicates, from where to start using the data
		
		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, cubeVertices.length * 4, verticesBuffer, GLES32.GL_STATIC_DRAW);
		
		GLES32.glVertexAttribPointer(GLESMacros.AMC_ATTRIBUTE_POSITION, 3, GLES32.GL_FLOAT, false, 0, 0);
		
		GLES32.glEnableVertexAttribArray(GLESMacros.AMC_ATTRIBUTE_POSITION);
		
		
		//vbo for texture	
		GLES32.glGenBuffers(1, vbo_texure, 0); 		//Creates a vbo. 
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_texure[0]);
		
		//Create a buffer to pass our float data to GPU in native fashion
		byteBuffer =  ByteBuffer.allocateDirect(cubeTexcoords.length * 4); //4 is size of float.
		byteBuffer.order(ByteOrder.nativeOrder()); //Detect native machine endianess and use it
		verticesBuffer = byteBuffer.asFloatBuffer();
		verticesBuffer.put(cubeTexcoords); //fill the data
		verticesBuffer.position(0); //Zero indicates, from where to start using the data
		
		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, cubeTexcoords.length * 4, verticesBuffer, GLES32.GL_STATIC_DRAW);
		
		GLES32.glVertexAttribPointer(GLESMacros.AMC_ATTRIBUTE_TEXTURE0, 2, GLES32.GL_FLOAT, false, 0, 0);
		
		GLES32.glEnableVertexAttribArray(GLESMacros.AMC_ATTRIBUTE_TEXTURE0);
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
	
		
		//done with vaos
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
		GLES32.glBindVertexArray(0);
		
		//enable depth testing
		GLES32.glEnable(GLES32.GL_DEPTH_TEST);
		GLES32.glDepthFunc(GLES32.GL_LEQUAL);
		//GLES32.glEnable(GLES32.GL_CULL_FACE); //No culling for animation
		GLES32.glClearColor(0.0f, 0.0f, 0.0f, 1.0f); //black
		//GLES32.glEnable(GLES32.GL_TEXTURE_2D);
		//set projection matrix to identity matrix
		Matrix.setIdentityM(perspectiveProjectionMatrix, 0);
		
	}
	
	private void resize(int width, int height){
		GLES32.glViewport(0,0,width,height);
		
		
		//perspective projection
		Matrix.perspectiveM(perspectiveProjectionMatrix, 0, 45.0f,((float)width/(float)height),0.1f,100.0f);
	}
	
	public void draw(){
		GLES32.glClear(GLES32.GL_COLOR_BUFFER_BIT|GLES32.GL_DEPTH_BUFFER_BIT);
		//use shader program
		GLES32.glUseProgram(shaderProgramObject);
		float modelViewMatrix[] = new float[16];
		float modelViewProjectionMatrix[] = new float [16];
		//set modelview and modelview projection matrix to identity
		Matrix.setIdentityM(modelViewMatrix, 0);
		Matrix.setIdentityM(modelViewProjectionMatrix, 0);
		
		//multiply modelview and projection matrix to get modelViewProjection matrix
		Matrix.translateM(modelViewMatrix, 0, -1.5f, 0.0f, zAxis);
		Matrix.rotateM(modelViewMatrix, 0, angle_pyramid , 0.0f, 1.0f, 0.0f);
		Matrix.multiplyMM(modelViewProjectionMatrix, 0, perspectiveProjectionMatrix, 0,modelViewMatrix,0);
		
		//pass above matrix to u_mvp_matrix
		
		GLES32.glUniformMatrix4fv(mvpUniform, 1, false, modelViewProjectionMatrix, 0);
		
		//bind vao. Start playing
		GLES32.glBindVertexArray(vao_pyramid[0]);
		
		//bind pyramid texture
		GLES32.glActiveTexture(GLES32.GL_TEXTURE0);
		GLES32.glBindTexture(GLES32.GL_TEXTURE_2D, texture_stone[0]);
		GLES32.glUniform1i(texture0_sampler_uniform, 0);
		
		//draw using glDrawArrays
		GLES32.glDrawArrays(GLES32.GL_TRIANGLES, 0, 12);
		
		GLES32.glBindVertexArray(0);
		
		//draw cube
		//reset model view matrix
		Matrix.setIdentityM(modelViewMatrix, 0);
		Matrix.setIdentityM(modelViewProjectionMatrix, 0);
		
		//multiply modelview and projection matrix to get modelViewProjection matrix
		Matrix.translateM(modelViewMatrix, 0, 1.5f, 0.0f, zAxis);
		Matrix.scaleM(modelViewMatrix, 0, 0.75f, 0.75f, 0.75f);
		Matrix.rotateM(modelViewMatrix, 0, angle_cube , 1.0f, 0.0f, 0.0f); 
		Matrix.rotateM(modelViewMatrix, 0, angle_cube , 0.0f, 1.0f, 0.0f);
		Matrix.rotateM(modelViewMatrix, 0, angle_cube , 0.0f, 0.0f, 1.0f);
		Matrix.multiplyMM(modelViewProjectionMatrix, 0, perspectiveProjectionMatrix, 0,modelViewMatrix,0);
		
		//pass above matrix to u_mvp_matrix
		
		GLES32.glUniformMatrix4fv(mvpUniform, 1, false, modelViewProjectionMatrix, 0);
		
		//bind vao. Start playing
		GLES32.glBindVertexArray(vao_cube[0]);
		
		//bind cube texture
		GLES32.glActiveTexture(GLES32.GL_TEXTURE0);
		GLES32.glBindTexture(GLES32.GL_TEXTURE_2D, texture_kundali[0]);
		GLES32.glUniform1i(texture0_sampler_uniform, 0);
		//draw using glDrawArrays
		GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 0, 4); 
		GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 4, 4);
		GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 8, 4); 
		GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 12, 4); 
		GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 16, 4); 
		GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 20, 4); 
		
		GLES32.glBindVertexArray(0);
	
		
		
		GLES32.glUseProgram(0);
		
		update(); 
		//SwapBuffers 
		requestRender();
	}
	void update(){
		if(angle_pyramid > 360.0){
			angle_pyramid = 0.0f;
			angle_cube = 0.0f;
		}else{
			angle_pyramid = angle_pyramid + 1.0f;
			angle_cube = angle_cube + 1.0f;
		}
	}
	private int loadGLTexture(int imageFileResourceID){
		BitmapFactory.Options options = new BitmapFactory.Options();
		options.inScaled = false;
		
		//read resource data using glabal context
		Bitmap bitmap = BitmapFactory.decodeResource(context.getResources(), imageFileResourceID, options);
		
		int[] texture = new int[1];
		
		//create a texture 
		GLES32.glGenTextures(1, texture, 0);
		GLES32.glPixelStorei(GLES32.GL_UNPACK_ALIGNMENT, 1);

		GLES32.glBindTexture(GLES32.GL_TEXTURE_2D, texture[0]);
		GLES32.glTexParameteri(GLES32.GL_TEXTURE_2D, GLES32.GL_TEXTURE_MAG_FILTER, GLES32.GL_LINEAR);
		GLES32.glTexParameteri(GLES32.GL_TEXTURE_2D, GLES32.GL_TEXTURE_MIN_FILTER, GLES32.GL_LINEAR_MIPMAP_LINEAR);
		GLUtils.texImage2D(GLES32.GL_TEXTURE_2D, 0, bitmap, 0);

		GLES32.glGenerateMipmap(GLES32.GL_TEXTURE_2D);
		
		return(texture[0]);
		
	}
	void uninitialize(){
		//destroy vao
		if(vao_pyramid[0] != 0){
			GLES32.glDeleteVertexArrays(1,vao_pyramid, 0);
			vao_pyramid[0] = 0;			
		}
		
		if(vao_cube[0] != 0){
			GLES32.glDeleteVertexArrays(1,vao_cube, 0);
			vao_cube[0] = 0;			
		}
		
		if(vbo_position[0] != 0){
			GLES32.glDeleteBuffers(1,vbo_position, 0);
			vbo_position[0] = 0;			
		}
		
		if(vbo_texure[0] != 0){
			GLES32.glDeleteBuffers(1,vbo_texure, 0);
			vbo_texure[0] = 0;			
		}
		
		
		if(shaderProgramObject != 0){
			if(vertexShaderObject != 0){
				//detach first then delete
				GLES32.glDetachShader(shaderProgramObject, vertexShaderObject);
				GLES32.glDeleteShader(vertexShaderObject);
				vertexShaderObject=0;
			}
			if(fragmentShaderObject != 0){
				//detach first then delete
				GLES32.glDetachShader(shaderProgramObject, fragmentShaderObject);
				GLES32.glDeleteShader(fragmentShaderObject);
				fragmentShaderObject=0;
			}
			if( shaderProgramObject != 0 ){
				GLES32.glDeleteProgram(shaderProgramObject);
				shaderProgramObject = 0;
			}
		}
	}
	
	
}