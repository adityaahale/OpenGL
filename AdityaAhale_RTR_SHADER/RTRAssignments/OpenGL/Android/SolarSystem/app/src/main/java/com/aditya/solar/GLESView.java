package com.aditya.solar;

import android.support.v7.widget.AppCompatTextView;
import android.content.Context;
import android.graphics.Color;
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
import java.nio.ShortBuffer;
//A view for OpenGLES3.2 graphics which also receives touch events

public class GLESView extends GLSurfaceView implements GLSurfaceView.Renderer, OnGestureListener, OnDoubleTapListener
{
	private final Context context;
	private GestureDetector gestureDetector;
	
	//new class memebers
	private int vertexShaderObject;
	private int fragmentShaderObject;
	private int shaderProgramObject;
	
	private int[] vao = new int[1];
	private int[] vbo = new int[1];
	private int mvpUniform;
	private float zAxis = -4.0f;
	//sphere loading
	private int[] vao_sphere = new int[1];
    private int[] vbo_sphere_position = new int[1];
    private int[] vbo_sphere_normal = new int[1];
    private int[] vbo_sphere_element = new int[1];
	private int numVertices, numElements;
	public int year = 0, day = 0;
	
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
		//elbow = (elbow + 3) % 360;
		return true;
	}
	@Override //abstract method from OnDoubleTapListener
	public boolean onDoubleTapEvent(MotionEvent e){
		//nothing to do for now. Already handled in onDoubleTap
		return true;
	}
	
	@Override //abstract method from OnDoubleTapListener
	public boolean onSingleTapConfirmed(MotionEvent e){
		// shoulder = (shoulder + 3) % 360;
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
		int[] iShaderCompiledStatus = new int[1]; //taken as array, because this will be a out param
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
		
		
		Sphere sphere=new Sphere();
        float sphere_vertices[]=new float[1146];
        float sphere_normals[]=new float[1146];
        float sphere_textures[]=new float[764];
        short sphere_elements[]=new short[2280];
        sphere.getSphereVertexData(sphere_vertices, sphere_normals, sphere_textures, sphere_elements);
        numVertices = sphere.getNumberOfSphereVertices();
        numElements = sphere.getNumberOfSphereElements();

        // vao
        GLES32.glGenVertexArrays(1,vao_sphere,0);
        GLES32.glBindVertexArray(vao_sphere[0]);
        
        // position vbo
        GLES32.glGenBuffers(1,vbo_sphere_position,0);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER,vbo_sphere_position[0]);
        
        ByteBuffer byteBuffer=ByteBuffer.allocateDirect(sphere_vertices.length * 4);
        byteBuffer.order(ByteOrder.nativeOrder());
        FloatBuffer verticesBuffer=byteBuffer.asFloatBuffer();
        verticesBuffer.put(sphere_vertices);
        verticesBuffer.position(0);
        
        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER,
                            sphere_vertices.length * 4,
                            verticesBuffer,
                            GLES32.GL_STATIC_DRAW);
        
        GLES32.glVertexAttribPointer(GLESMacros.AMC_ATTRIBUTE_POSITION,
                                     3,
                                     GLES32.GL_FLOAT,
                                     false,0,0);
        
        GLES32.glEnableVertexAttribArray(GLESMacros.AMC_ATTRIBUTE_POSITION);
        
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER,0);
        
        // normal vbo
        GLES32.glGenBuffers(1,vbo_sphere_normal,0);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER,vbo_sphere_normal[0]);
        
        byteBuffer=ByteBuffer.allocateDirect(sphere_normals.length * 4);
        byteBuffer.order(ByteOrder.nativeOrder());
        verticesBuffer=byteBuffer.asFloatBuffer();
        verticesBuffer.put(sphere_normals);
        verticesBuffer.position(0);
        
        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER,
                            sphere_normals.length * 4,
                            verticesBuffer,
                            GLES32.GL_STATIC_DRAW);
        
        GLES32.glVertexAttribPointer(GLESMacros.AMC_ATTRIBUTE_NORMAL,
                                     3,
                                     GLES32.GL_FLOAT,
                                     false,0,0);
        
        GLES32.glEnableVertexAttribArray(GLESMacros.AMC_ATTRIBUTE_NORMAL);
        
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER,0);
       // GLES32.glVertexAttrib3f(GLESMacros.AMC_ATTRIBUTE_COLOR, 0.0f,0.0f,1.0f);
        // element vbo
        GLES32.glGenBuffers(1,vbo_sphere_element,0);
        GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER,vbo_sphere_element[0]);
        
        byteBuffer=ByteBuffer.allocateDirect(sphere_elements.length * 2);
        byteBuffer.order(ByteOrder.nativeOrder());
        ShortBuffer elementsBuffer=byteBuffer.asShortBuffer();
        elementsBuffer.put(sphere_elements);
        elementsBuffer.position(0);
        
        GLES32.glBufferData(GLES32.GL_ELEMENT_ARRAY_BUFFER,
                            sphere_elements.length * 2,
                            elementsBuffer,
                            GLES32.GL_STATIC_DRAW);
        
        GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER,0);

        GLES32.glBindVertexArray(0);
		
		//enable depth testing
		GLES32.glEnable(GLES32.GL_DEPTH_TEST);
		GLES32.glDepthFunc(GLES32.GL_LEQUAL);
		GLES32.glEnable(GLES32.GL_CULL_FACE);
		GLES32.glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		
	
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
		float translate1Matrix[] = new float[16];
		float translate2Matrix[] = new float[16];
		float translate3Matrix[] = new float[16];
		float translate4Matrix[] = new float[16];
		float rotationMatrixSho[] = new float[16];
		float rotationMatrixElb[] = new float[16];
		float scaleMatrix[] = new float[16];
		float scaleInvMatrix[] = new float[16];
		
		//set modelview and modelview projection matrix to identity
		Matrix.setIdentityM(modelViewMatrix, 0);
		Matrix.setIdentityM(modelViewProjectionMatrix, 0);
		Matrix.setIdentityM(translate1Matrix, 0);
		Matrix.setIdentityM(translate2Matrix, 0);
		Matrix.setIdentityM(translate3Matrix, 0);
		Matrix.setIdentityM(translate4Matrix, 0);
		Matrix.setIdentityM(rotationMatrixSho, 0);
		Matrix.setIdentityM(rotationMatrixElb, 0);
		Matrix.setIdentityM(scaleMatrix, 0);
		Matrix.setIdentityM(scaleInvMatrix, 0);

	
		//multiply modelview and projection matrix to get modelViewProjection matrix
	//	Matrix.translateM(translate1Matrix, 0, -0.5f, 0.0f, -12.0f);
		Matrix.translateM(translate2Matrix, 0, 0.0f, 0.0f, -12.0f);
		
		//Matrix.scaleM(scaleMatrix,0,2.0f, 0.5f, 1.0f);
		
		//Matrix.multiplyMM(modelViewMatrix, 0, modelViewMatrix, 0,translate1Matrix,0);
		Matrix.multiplyMM(modelViewMatrix, 0, modelViewMatrix, 0,translate2Matrix,0);
		//Matrix.multiplyMM(modelViewMatrix, 0, modelViewMatrix, 0,rotationMatrixSho,0);
		
		//Matrix.multiplyMM(modelViewMatrix, 0, modelViewMatrix, 0,scaleMatrix,0);
		Matrix.multiplyMM(modelViewProjectionMatrix, 0, perspectiveProjectionMatrix, 0,modelViewMatrix,0);
		
		//pass above matrix to u_mvp_matrix
		
		GLES32.glUniformMatrix4fv(mvpUniform, 1, false, modelViewProjectionMatrix, 0);
		
		GLES32.glVertexAttrib3f(GLESMacros.AMC_ATTRIBUTE_COLOR, 0.5f, 0.35f, 0.05f);
		//bind vao. 
		
		GLES32.glBindVertexArray(vao_sphere[0]);
        
        // *** draw, either by glDrawTriangles() or glDrawArrays() or glDrawElements()
        GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element[0]);
        GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);
        
        // unbind vao`
        GLES32.glBindVertexArray(0);

		//Matrix.scaleM(scaleInvMatrix,0,0.5f, 2.0f, 1.0f);
		Matrix.rotateM(rotationMatrixSho, 0, (float)year, 0.0f, 1.0f, 0.0f);
		Matrix.translateM(translate3Matrix, 0, 1.5f, 0.0f, 0.0f);
		Matrix.rotateM(rotationMatrixElb, 0, (float)-day, 0.0f, 0.0f, 1.0f);
		Matrix.translateM(translate4Matrix, 0, 1.0f, 0.0f, 0.0f);
		Matrix.scaleM(scaleMatrix,0,2.0f, 0.5f, 1.0f);
		
		//Matrix.multiplyMM(modelViewMatrix, 0, modelViewMatrix, 0,scaleInvMatrix,0);
		Matrix.multiplyMM(modelViewMatrix, 0, modelViewMatrix, 0,rotationMatrixSho,0);
		Matrix.multiplyMM(modelViewMatrix, 0, modelViewMatrix, 0,translate3Matrix,0);
		Matrix.multiplyMM(modelViewMatrix, 0, modelViewMatrix, 0,rotationMatrixElb,0);
		//Matrix.multiplyMM(modelViewMatrix, 0, modelViewMatrix, 0,translate4Matrix,0);
		//Matrix.multiplyMM(modelViewMatrix, 0, modelViewMatrix, 0,scaleMatrix,0);
		Matrix.multiplyMM(modelViewProjectionMatrix, 0, perspectiveProjectionMatrix, 0,modelViewMatrix,0);
		
		GLES32.glUniformMatrix4fv(mvpUniform, 1, false, modelViewProjectionMatrix, 0);
		
		GLES32.glVertexAttrib3f(GLESMacros.AMC_ATTRIBUTE_COLOR, 0.0f, 0.0f, 1.0f);
		//bind vao. 
		
		GLES32.glBindVertexArray(vao_sphere[0]);
        
        // *** draw, either by glDrawTriangles() or glDrawArrays() or glDrawElements()
        GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element[0]);
        GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);
        
        // unbind vao`
        GLES32.glBindVertexArray(0);
		
		GLES32.glUseProgram(0);
		
		update();
		//SwapBuffers 
		requestRender();
	}
	
	void update()
	{
		year = (year + 3) % 360;
		day = (day + 6) % 360;	
	}
	
	void uninitialize(){
		//destroy vao
		if(vao[0] != 0){
			GLES32.glDeleteVertexArrays(1,vao, 0);
			vao[0] = 0;			
		}
		
		if(vbo[0] != 0){
			GLES32.glDeleteBuffers(1,vbo, 0);
			vbo[0] = 0;			
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