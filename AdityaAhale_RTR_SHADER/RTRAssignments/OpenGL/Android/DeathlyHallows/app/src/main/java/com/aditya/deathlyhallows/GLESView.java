package com.aditya.deathlyhallows;

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

//A view for OpenGLES3.2 graphics which also receives touch events

public class GLESView extends GLSurfaceView implements GLSurfaceView.Renderer, OnGestureListener, OnDoubleTapListener
{
	private final Context context;
	private GestureDetector gestureDetector;
	
	//new class memebers
	private int vertexShaderObject;
	private int fragmentShaderObject;
	private int shaderProgramObject;
	
	private int[] vao_vertical_line = new int[1];
	private int[] vao_circle = new int[1];
	private int[] vao_triangle = new int[1];
	private int[] vbo_position = new int[1];
	private int[] vbo_color = new int[1];
	private int mvpUniform;
	
	float myRadius;			//1,-1	0,1  -1,-1 
	float area;
	float a, b, c, tx, ty;
	float x1 = 0.0f, x2 = -0.7f, x3 = 0.7f, yone = 0.7f, y2 = -0.7f, y3 = -0.7f;
	float s, summation;
	float rot, xtrans = -3.0f, ytrans = -3.0f, xctrans = 3.0f, yctrans = -3.0f, yltrans = 3.0f;
	private float perspectiveProjectionMatrix[]=new float[16];
	
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
		//uninitialize();
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
		"in vec4 vColor;" +
		"out vec4 out_color;" + 
		"uniform mat4 u_mvp_matrix;" +
		"void main(void)" +
		"{" +
		"gl_Position = u_mvp_matrix * vPosition;" +		
		"out_color = vColor;" +
		"}" 
		); //source code string ends
		
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
		"out vec4 FragColor;" +
		"void main(void)"+
		"{"+
		"FragColor = out_color;" +
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
		GLES32.glBindAttribLocation(shaderProgramObject, GLESMacros.AMC_ATTRIBUTE_COLOR, "vColor");
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
		
		
		//Vertical Line
		final float verticalVertices[] = new float[]{
			0.0f,0.7f,0.0f,
		0.0f,-0.7f,0.0f
		};
		
		final float verticalColors[] = new float[]{
			1.0f,1.0f,1.0f, //white
		1.0f,1.0f,1.0f
		};
		
		GLES32.glGenVertexArrays(1, vao_vertical_line, 0); 	//NOTE additional zero
		GLES32.glBindVertexArray(vao_vertical_line[0]);		//NOTE how it is used
		
		//vbo for positions
		GLES32.glGenBuffers(1, vbo_position, 0); 		//Creates a vbo. 
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position[0]);
		
		//Create a buffer to pass our float data to GPU in native fashion
		ByteBuffer byteBuffer =  ByteBuffer.allocateDirect(verticalVertices.length * 4); //4 is size of float
		byteBuffer.order(ByteOrder.nativeOrder()); //Detect native machine endianess and use it
		FloatBuffer verticesBuffer = byteBuffer.asFloatBuffer();
		verticesBuffer.put(verticalVertices); //fill the data
		verticesBuffer.position(0); //Zero indicates, from where to start using the data
		
		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, verticalVertices.length * 4, verticesBuffer, GLES32.GL_STATIC_DRAW);
		
		GLES32.glVertexAttribPointer(GLESMacros.AMC_ATTRIBUTE_POSITION, 3, GLES32.GL_FLOAT, false, 0, 0);
		
		GLES32.glEnableVertexAttribArray(GLESMacros.AMC_ATTRIBUTE_POSITION);
	
		//vbo for color
		GLES32.glGenBuffers(1, vbo_color, 0); 		//Creates a vbo for color
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_color[0]);
		
		//Create a buffer to pass our float data to GPU in native fashion
		byteBuffer =  ByteBuffer.allocateDirect(verticalColors.length * 4); //4 is size of float
		byteBuffer.order(ByteOrder.nativeOrder()); //Detect native machine endianess and use it
		FloatBuffer colorBuffer = byteBuffer.asFloatBuffer();
		colorBuffer.put(verticalColors); //fill the data
		colorBuffer.position(0); //Zero indicates, from where to start using the data
		
		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, verticalColors.length * 4, colorBuffer, GLES32.GL_STATIC_DRAW);
		
		GLES32.glVertexAttribPointer(GLESMacros.AMC_ATTRIBUTE_COLOR, 3, GLES32.GL_FLOAT, false, 0, 0);
		
		GLES32.glEnableVertexAttribArray(GLESMacros.AMC_ATTRIBUTE_COLOR);
		
		//inner circle
		c = (float)Math.sqrt((x2 - x1)*(x2 - x1) + (y2 - yone)*(y2 - yone)); //These are opposite lengths/sides of vertices
		a = (float)Math.sqrt((x3 - x2)*(x3 - x2) + (y3 - y2)*(y3 - y2));
		b = (float)Math.sqrt((x1 - x3)*(x1 - x3) + (yone - y3)*(yone - y3));
		summation = a + b + c;
		s = (float)(a + b + c) / 2.0f;
	
		area = (float)Math.sqrt(s*(s - a)*(s - b)*(s - c)); 
		myRadius = area / s;						//centre calculation
		
		float circleVertices[]=new float[3000];
		float doublePi = 2.0f * 3.141592653f; //2PI

		float circleVerticesX[]=new float[3000];
		float circleVerticesY[]=new float[3000];
		float circleVerticesZ[]=new float[3000];
		
		
		
		for (int i = 0; i < 3000; i++)
	{
		circleVerticesX[i] = (myRadius * (float)Math.cos(i * doublePi / 1000));
		circleVerticesY[i] = (myRadius * (float)Math.sin(i * doublePi / 1000));
		circleVerticesZ[i] = 0.0f;
	}

	float allCircleVertices[]=new float[9000];

	for (int i = 0; i < 3000; i++)
	{
		allCircleVertices[i * 3] = circleVerticesX[i];
		allCircleVertices[(i * 3) + 1] = circleVerticesY[i];
		allCircleVertices[(i * 3) + 2] = circleVerticesZ[i];
	}


	

	float circleColor[]=new float[9000];
	for (int j = 0; j < 3000; j++)
	{
		circleColor[j * 3] = 1.0f;
		circleColor[(j * 3)+1] = 1.0f;
		circleColor[(j * 3)+2] = 1.0f;
	}
	
		
		GLES32.glGenVertexArrays(1, vao_circle, 0); 	//NOTE additional zero
		GLES32.glBindVertexArray(vao_circle[0]);		//NOTE how it is used
		
		//vbo for positions
		GLES32.glGenBuffers(1, vbo_position, 0); 		//Creates a vbo. 
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position[0]);
		
		//Create a buffer to pass our float data to GPU in native fashion
		ByteBuffer byteBufferC =  ByteBuffer.allocateDirect(allCircleVertices.length * 4); //4 is size of float. This is global mem allocation. All memory location will be initialized to zero
		byteBufferC.order(ByteOrder.nativeOrder()); //Detect native machine endianess and use it
		FloatBuffer verticesBufferC = byteBufferC.asFloatBuffer();
		verticesBufferC.put(allCircleVertices); //fill the data
		verticesBufferC.position(0); //Zero indicates, from where to start using the data
		
		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, allCircleVertices.length * 4, verticesBufferC, GLES32.GL_STATIC_DRAW);
		
		GLES32.glVertexAttribPointer(GLESMacros.AMC_ATTRIBUTE_POSITION, 3, GLES32.GL_FLOAT, false, 0, 0);
		
		GLES32.glEnableVertexAttribArray(GLESMacros.AMC_ATTRIBUTE_POSITION);
	
		//vbo for color
		GLES32.glGenBuffers(1, vbo_color, 0); 		//Creates a vbo for color
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_color[0]);
		
		//Create a buffer to pass our float data to GPU in native fashion
		byteBufferC =  ByteBuffer.allocateDirect(circleColor.length * 4); //4 is size of float. This is global mem allocation. All memory location will be initialized to zero
		byteBufferC.order(ByteOrder.nativeOrder()); //Detect native machine endianess and use it
		FloatBuffer colorBufferC = byteBufferC.asFloatBuffer();
		colorBufferC.put(circleColor); //fill the data
		colorBufferC.position(0); //Zero indicates, from where to start using the data
		
		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, circleColor.length * 4, colorBufferC, GLES32.GL_STATIC_DRAW);
		
		GLES32.glVertexAttribPointer(GLESMacros.AMC_ATTRIBUTE_COLOR, 3, GLES32.GL_FLOAT, false, 0, 0);
		
		GLES32.glEnableVertexAttribArray(GLESMacros.AMC_ATTRIBUTE_COLOR);
	
	
		
		
	//triangle
	float triangleVertices[] = new float[]{
		0.0f,0.7f,
		-0.7f,-0.7f,
		0.7f,-0.7f,
		
	
	};
	
	float triangleColors[] =new float[]{
		1.0f,1.0f,1.0f,
		1.0f,1.0f,1.0f,
		1.0f,1.0f,1.0f,
		1.0f,1.0f,1.0f,
		1.0f,1.0f,1.0f,
		1.0f,1.0f,1.0f,
		1.0f,1.0f,1.0f,
		1.0f,1.0f,1.0f,
		1.0f,1.0f,1.0f,
		1.0f,1.0f,1.0f,
		1.0f,1.0f,1.0f,
		1.0f,1.0f,1.0f,
		1.0f,1.0f,1.0f,
		1.0f,1.0f,1.0f,
		1.0f,1.0f,1.0f,
		1.0f,1.0f,1.0f	
	};
	
		
		GLES32.glGenVertexArrays(1, vao_triangle, 0); 	//NOTE additional zero
		GLES32.glBindVertexArray(vao_triangle[0]);		//NOTE how it is used
		
		//vbo for positions
		GLES32.glGenBuffers(1, vbo_position, 0); 		//Creates a vbo. 
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position[0]);
		
		//Create a buffer to pass our float data to GPU in native fashion
		ByteBuffer byteBufferT =  ByteBuffer.allocateDirect(triangleVertices.length * 4); //4 is size of float. This is global mem allocation. All memory location will be initialized to zero
		byteBufferT.order(ByteOrder.nativeOrder()); //Detect native machine endianess and use it
		FloatBuffer verticesBufferT = byteBufferT.asFloatBuffer();
		verticesBufferT.put(triangleVertices); //fill the data
		verticesBufferT.position(0); //Zero indicates, from where to start using the data
		
		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, triangleVertices.length * 4, verticesBufferT, GLES32.GL_STATIC_DRAW);
		
		GLES32.glVertexAttribPointer(GLESMacros.AMC_ATTRIBUTE_POSITION, 2, GLES32.GL_FLOAT, false, 0, 0);
		
		GLES32.glEnableVertexAttribArray(GLESMacros.AMC_ATTRIBUTE_POSITION);
	
		//vbo for color
		GLES32.glGenBuffers(1, vbo_color, 0); 		//Creates a vbo for color
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_color[0]);
		
		//Create a buffer to pass our float data to GPU in native fashion
		byteBufferT =  ByteBuffer.allocateDirect(triangleColors.length * 4); //4 is size of float. This is global mem allocation. All memory location will be initialized to zero
		byteBufferT.order(ByteOrder.nativeOrder()); //Detect native machine endianess and use it
		FloatBuffer colorBufferT = byteBufferT.asFloatBuffer();
		colorBufferT.put(triangleColors); //fill the data
		colorBufferT.position(0); //Zero indicates, from where to start using the data
		
		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, triangleColors.length * 4, colorBufferT, GLES32.GL_STATIC_DRAW);
		
		GLES32.glVertexAttribPointer(GLESMacros.AMC_ATTRIBUTE_COLOR, 3, GLES32.GL_FLOAT, false, 0, 0);
		
		GLES32.glEnableVertexAttribArray(GLESMacros.AMC_ATTRIBUTE_COLOR);
		
	
		//done with vaos
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
		GLES32.glBindVertexArray(0);
		
		//enable depth testing
		GLES32.glEnable(GLES32.GL_DEPTH_TEST);
		//GLES32.glDepthFunc(GLES32.GL_LEQUAL);
		GLES32.glEnable(GLES32.GL_CULL_FACE);
		GLES32.glClearColor(0.0f, 0.0f, 0.0f, 1.0f); //black
		
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
		
		//draw vertical line
		//reset model view matrix
		Matrix.setIdentityM(modelViewMatrix, 0);
		Matrix.setIdentityM(modelViewProjectionMatrix, 0);
		
		//multiply modelview and projection matrix to get modelViewProjection matrix
		Matrix.translateM(modelViewMatrix, 0, 0.0f, yltrans, -3.0f);
		Matrix.multiplyMM(modelViewProjectionMatrix, 0, perspectiveProjectionMatrix, 0,modelViewMatrix,0);
		
		//pass above matrix to u_mvp_matrix
		
		GLES32.glUniformMatrix4fv(mvpUniform, 1, false, modelViewProjectionMatrix, 0);
		
		//bind vao. Start playing
		GLES32.glBindVertexArray(vao_vertical_line[0]);
		
		//draw using glDrawArrays
		GLES32.glDrawArrays(GLES32.GL_LINES, 0, 2);
		
		GLES32.glBindVertexArray(0);
	
	
		//draw innercircle
		//reset model view matrix
		Matrix.setIdentityM(modelViewMatrix, 0);
		Matrix.setIdentityM(modelViewProjectionMatrix, 0);
		
		tx = ((a*x1) + (b*x2) + (c*x3)) / summation;
		ty = ((a*yone) + (b*y2) + (c*y3)) / summation;
		//multiply modelview and projection matrix to get modelViewProjection matrix
		Matrix.translateM(modelViewMatrix, 0, tx+xctrans, ty-yctrans, -3.0f);
		Matrix.rotateM(modelViewMatrix, 0, rot , 0.0f, 1.0f, 0.0f);
		Matrix.multiplyMM(modelViewProjectionMatrix, 0, perspectiveProjectionMatrix, 0,modelViewMatrix,0);
		
		//pass above matrix to u_mvp_matrix
		
		GLES32.glUniformMatrix4fv(mvpUniform, 1, false, modelViewProjectionMatrix, 0);
		
		//bind vao. Start playing
		GLES32.glBindVertexArray(vao_circle[0]);
		
		//draw using glDrawArrays
		GLES32.glDrawArrays(GLES32.GL_LINE_LOOP, 0, 3000);
		//GLES32.glDrawArrays(GLES32.GL_LINES, 0, 3);
		//GLES32.glDrawArrays(GLES32.GL_LINES, 0, 3);
		
		GLES32.glBindVertexArray(0);
	
	
		//draw triangle
		//reset model view matrix
		GLES32.glLineWidth(1.0f);
		Matrix.setIdentityM(modelViewMatrix, 0);
		Matrix.setIdentityM(modelViewProjectionMatrix, 0);
		
		//multiply modelview and projection matrix to get modelViewProjection matrix
		Matrix.translateM(modelViewMatrix, 0, xtrans, ytrans, -3.0f);
		Matrix.rotateM(modelViewMatrix, 0, rot , 0.0f, 1.0f, 0.0f);
		Matrix.multiplyMM(modelViewProjectionMatrix, 0, perspectiveProjectionMatrix, 0,modelViewMatrix,0);
		
		//pass above matrix to u_mvp_matrix
		
		GLES32.glUniformMatrix4fv(mvpUniform, 1, false, modelViewProjectionMatrix, 0);
		
		//bind vao. Start playing
		GLES32.glBindVertexArray(vao_triangle[0]);
		
		//draw using glDrawArrays
		GLES32.glDrawArrays(GLES32.GL_LINE_LOOP, 0, 3);
		//GLES32.glDrawArrays(GLES32.GL_LINES, 0, 3);
		//GLES32.glDrawArrays(GLES32.GL_LINES, 0, 3);
		
		GLES32.glBindVertexArray(0);

		GLES32.glUseProgram(0);
		
		update();
		//SwapBuffers 
		requestRender();
	}
	
	void update()
	{
		if(rot > 360.0){
			rot = 0.0f;
		}else{
			rot = rot + 1.0f;
		}
		if (xtrans <= 0.0)
		{
		xtrans = xtrans + 0.009f;
		ytrans = ytrans + 0.009f;
		}

		if (xtrans >= 0.0)
		{
			if (xctrans > 0.0)
			{
				xctrans = xctrans - 0.009f;
				yctrans = yctrans + 0.009f;

			}	


		}

		if (xtrans > 0.0)
		{
			if (xctrans < 0.0)
			{
				if (yltrans >= 0.0)
				{
					yltrans = yltrans - 0.009f;
				}
			}
		}
	
	}

	void uninitialize(){
		//destroy vao
		if(vao_vertical_line[0] != 0){
			GLES32.glDeleteVertexArrays(1,vao_vertical_line, 0);
			vao_vertical_line[0] = 0;			
		}
		
		if(vbo_position[0] != 0){
			GLES32.glDeleteBuffers(1,vbo_position, 0);
			vbo_position[0] = 0;			
		}
		if(vao_circle[0] != 0){
			GLES32.glDeleteVertexArrays(1,vao_circle, 0);
			vao_circle[0] = 0;			
		}
		if(vao_triangle[0] != 0){
			GLES32.glDeleteVertexArrays(1,vao_triangle, 0);
			vao_triangle[0] = 0;			
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