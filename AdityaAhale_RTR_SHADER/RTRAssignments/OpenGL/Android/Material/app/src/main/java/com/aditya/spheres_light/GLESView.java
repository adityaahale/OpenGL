package com.aditya.spheres_light;

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
import java.nio.ShortBuffer;

//A view for OpenGLES3.2 graphics which also receives touch events

public class GLESView extends GLSurfaceView implements GLSurfaceView.Renderer, OnGestureListener, OnDoubleTapListener
{
	private final Context context;
	private GestureDetector gestureDetector;
	
	//new class members
	private int vertexShaderObject;
	private int fragmentShaderObject;
	private int shaderProgramObject;
	private float[][] materialProperties;
	private int  lightPositionUniform, doubleTapUniform ;
	private int  modelMatrixUniform, viewMatrixUniform, projectionMatrixUniform;;
	private int	 laUniform, ldUniform, lsUniform;
	private int  kaUniform, kdUniform, ksUniform, materialShininessUniform;
	private float perspectiveProjectionMatrix[]=new float[16];
	private float angle;
	private float zAxis = -2.0f;
	private static final float myRadius = 700.0f;
	
	//sphere loading
	private int[] vao_sphere = new int[1];
    private int[] vbo_sphere_position = new int[1];
    private int[] vbo_sphere_normal = new int[1];
    private int[] vbo_sphere_element = new int[1];
	//light
	private int[] vao_lights = new int[1];
	private int[] vbo_light_position = new int[1];
	private int numVertices, numElements;
	
	//lighting 
	private float light_ambient[] = {0.0f, 0.0f, 0.0f, 0.0f};
	private float light_diffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
	private float light_specular[] = {1.0f, 1.0f, 1.0f, 1.0f};
	private float light_position[] = {0.0f, 0.0f, 0.0f, 1.0f};
	
	
	private int tapCount = 1; 
	private boolean gbLight;
	 private int viewportWidth;
    private int viewportHeight;
    private int x;
    private int y;
	
	private float material_ambient[][][] = {
        {   // Column 1
            {0.0215f, 0.1745f, 0.0215f, 1.0f },
            {0.135f, 0.2225f, 0.1575f, 1.0f },
            {0.05375f, 0.05f, 0.06625f, 1.0f },
            {0.25f, 0.20725f, 0.20725f, 1.0f },
            {0.1745f, 0.01175f, 0.01175f, 1.0f },
            {0.1f, 0.18725f, 0.1745f, 1.0f }
        },
        {   // Column 2
            {0.329412f, 0.223529f, 0.027451f, 1.0f },
            {0.2125f, 0.1275f, 0.054f, 1.0f },
            {0.25f, 0.25f, 0.25f, 1.0f },
            {0.19125f, 0.0735f, 0.0225f, 1.0f },
            {0.24725f, 0.1995f, 0.0745f, 1.0f },
            {0.19225f, 0.19225f, 0.19225f, 1.0f }
        },
        {   // Column 3
            {0.0f, 0.0f, 0.0f, 1.0f },
            {0.0f, 0.1f, 0.06f, 1.0f },
            {0.0f, 0.0f, 0.0f, 1.0f },
            {0.0f, 0.0f, 0.0f, 1.0f },
            {0.0f, 0.0f, 0.0f, 1.0f },
            {0.0f, 0.0f, 0.0f, 1.0f }
        },
        {   // Column 4
            {0.02f, 0.02f, 0.02f, 1.0f },
            {0.0f, 0.05f, 0.05f, 1.0f },
            {0.0f, 0.05f, 0.0f, 1.0f },
            {0.05f, 0.0f, 0.0f, 1.0f },
            {0.05f, 0.05f, 0.05f, 1.0f },
            {0.05f, 0.05f, 0.0f, 1.0f }
        }
    };

    private float material_diffuse[][][] = {
        {   // Column 1
            {0.07568f, 0.61424f, 0.07568f, 1.0f},
            {0.54f, 0.89f, 0.63f, 1.0f},
            {0.18275f, 0.17f, 0.22525f, 1.0f},
            {1.0f, 0.829f, 0.829f, 1.0f},
            {0.61424f, 0.04136f, 0.04136f, 1.0f},
            {0.396f, 0.74151f, 0.69102f, 1.0f},
        },
        {   // Column 2
            {0.780392f, 0.568627f, 0.113725f, 1.0f},
            {0.714f, 0.4284f, 0.18144f, 1.0f},
            {0.4f, 0.4f, 0.4f, 1.0f},
            {0.7038f, 0.27048f, 0.0828f, 1.0f},
            {0.75164f, 0.60648f, 0.22648f, 1.0f},
            {0.50754f, 0.50754f, 0.50754f, 1.0f},
        },
        {   // Column 3
            {0.01f, 0.01f, 0.01f, 1.0f},
            {0.0f, 0.50980392f, 0.50980392f, 1.0f},
            {0.1f, 0.35f, 0.1f, 1.0f},
            {0.5f, 0.0f, 0.0f, 1.0f},
            {0.55f, 0.55f, 0.55f, 1.0f},
            {0.5f, 0.5f, 0.0f, 1.0f},
        },
        {   // Column 4
            {0.01f, 0.01f, 0.01f, 1.0f},
            {0.4f, 0.5f, 0.5f, 1.0f},
            {0.4f, 0.5f, 0.4f, 1.0f},
            {0.5f, 0.4f, 0.4f, 1.0f},
            {0.5f, 0.5f, 0.5f, 1.0f},
            {0.5f, 0.5f, 0.4f, 1.0f},
        },
    };

    private float material_specular[][][] = {
        {   // Column 1
            {0.633f, 0.727811f, 0.633f, 1.0f},
            {0.316228f, 0.316228f, 0.316228f, 1.0f},
            {0.332741f, 0.328634f, 0.346435f, 1.0f},
            {0.296648f, 0.296648f, 0.296648f, 1.0f},
            {0.727811f, 0.626959f, 0.626959f, 1.0f},
            {0.297254f, 0.30829f, 0.306678f, 1.0f},
        },
        {   // Column 2
            {0.992157f, 0.941176f, 0.807843f, 1.0f},
            {0.393548f, 0.271906f, 0.166721f, 1.0f},
            {0.774597f, 0.774597f, 0.774597f, 1.0f},
            {0.256777f, 0.137622f, 0.086014f, 1.0f},
            {0.628281f, 0.555802f, 0.366065f, 1.0f},
            {0.508273f, 0.508273f, 0.508273f, 1.0f},
        },
        {   // Column 3
            {0.50f, 0.50f, 0.50f, 1.0f},
            {0.50196078f, 0.50196078f, 0.50196078f, 1.0f},
            {0.45f, 0.55f, 0.45f, 1.0f},
            {0.7f, 0.6f, 0.6f, 1.0f},
            {0.70f, 0.70f, 0.70f, 1.0f},
            {0.60f, 0.60f, 0.50f, 1.0f},
        },
        {   // Column 4
            {0.4f, 0.4f, 0.4f, 1.0f},
            {0.04f, 0.7f, 0.7f, 1.0f},
            {0.04f, 0.7f, 0.04f, 1.0f},
            {0.7f, 0.04f, 0.04f, 1.0f},
            {0.7f, 0.7f, 0.7f, 1.0f},
            {0.7f, 0.7f, 0.04f, 1.0f},
        }
    };

    private float material_shininess[][] = {
        {   // Column 1
            0.6f * 128.0f,
            0.1f * 128.0f,
            0.3f * 128.0f,
            0.088f * 128.0f,
            0.6f * 128.0f,
            0.1f * 128.0f
        },
        {   // Column 2
            0.21794872f * 128.0f,
            0.2f * 128.0f,
            0.6f * 128.0f,
            0.1f * 128.0f,
            0.4f * 128.0f,
            0.4f * 128.0f
        },
        {   // Column 3
            0.25f * 128.0f,
            0.25f * 128.0f,
            0.25f * 128.0f,
            0.25f * 128.0f,
            0.25f * 128.0f,
            0.25f * 128.0f
        },
        {   // Column 4
            0.078125f * 128.0f,
            0.078125f * 128.0f,
            0.078125f * 128.0f,
            0.078125f * 128.0f,
            0.078125f * 128.0f,
            0.078125f * 128.0f
        }
    };
	
	private int winWidth, winHeight;
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
			winWidth = width;
			winHeight = height;
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
			if(gbLight)
				gbLight=false;
			else
				gbLight=true;
		
		return true;
	}
	@Override //abstract method from OnDoubleTapListener
	public boolean onDoubleTapEvent(MotionEvent e){
		//nothing to do for now. Already handled in onDoubleTap
		return true;
	}
	
	@Override //abstract method from OnDoubleTapListener
	public boolean onSingleTapConfirmed(MotionEvent e){
		//tapCount= (++tapCount) % 3;
		tapCount++;
		if(tapCount>3)
			tapCount = 1;
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
			//return true;
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
		//initialize material properties
		
		//***** VERTEX SHADER ****
		vertexShaderObject = GLES32.glCreateShader(GLES32.GL_VERTEX_SHADER);
		
		//vertex shader source code
		final String vertexShaderSourceCode =  String.format(
         "#version 320 es"+
         "\n"+
         "in vec4 vPosition;"+
         "in vec3 vNormal;"+
		 "uniform vec4 u_light_position;"+
         "uniform mat4 u_model_matrix;"+
         "uniform mat4 u_view_matrix;"+
         "uniform mat4 u_projection_matrix;"+
         "uniform mediump int u_double_tap;"+
		 "out vec3 transformed_normals;" +
		 "out vec3 light_direction;" +
		 "out vec3 viewer_vector;" +		 
         "void main(void)"+
         "{"+
         "if (u_double_tap == 1)"+
         "{"+
         "vec4 eye_coordinates=u_view_matrix * u_model_matrix * vPosition;"+
         "transformed_normals=mat3(u_view_matrix * u_model_matrix) * vNormal;"+
         "light_direction = vec3(u_light_position) - eye_coordinates.xyz;"+
		 "viewer_vector = -eye_coordinates.xyz;"+         
         "}"+
         "gl_Position=u_projection_matrix * u_view_matrix * u_model_matrix * vPosition;"+
         "}"
		); //source code string ends
		
		//provide above source code to shader object
		GLES32.glShaderSource(vertexShaderObject, vertexShaderSourceCode);
		
		GLES32.glCompileShader(vertexShaderObject);
		//error checking
		int[] iShaderCompiledStatus = new int[1]; 
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
		"uniform mediump int u_double_tap;"+
		"in vec3 transformed_normals;" +
		"in vec3 light_direction;" +
		"in vec3 viewer_vector;" +
		"uniform vec3 u_La;" +
		"uniform vec3 u_Ld;" +
		"uniform vec3 u_Ls;" +
		"uniform vec3 u_Ka;" +
		"uniform vec3 u_Kd;" +
		"uniform vec3 u_Ks;" +
		"uniform float u_material_shininess;" +
		"out vec4 FragColor;" +
		"void main(void)" +
		"{" +
		"vec3 phong_ads_color;"+
		"if(u_double_tap == 1)" +
		"{" +
		"vec3 normalized_transformed_normals = normalize(transformed_normals);" +
		"vec3 normalized_light_direction = normalize(light_direction);" +
		"vec3 normalized_viewer_vector = normalize(viewer_vector);" +
		"float tn_dot_ld = max(dot(normalized_transformed_normals, normalized_light_direction), 0.0);" +
		"vec3 ambient = u_La * u_Ka;" +
		"vec3 diffuse = u_Ld * u_Kd * tn_dot_ld;" +
		"vec3 reflection_vector = reflect(-normalized_light_direction, normalized_transformed_normals);" +
		"vec3 specular = u_Ls * u_Ks * pow(max(dot(reflection_vector, normalized_viewer_vector),0.0), u_material_shininess);" +
		"phong_ads_color = ambient + diffuse + specular;" +
		"}" +
		"else" +
		"{" +
		"phong_ads_color = vec3(1.0, 1.0, 1.0);" +
		"}" +
		"FragColor = vec4(phong_ads_color, 1.0);" +
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
		GLES32.glBindAttribLocation(shaderProgramObject, GLESMacros.AMC_ATTRIBUTE_NORMAL, "vNormal");
		
		
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
		modelMatrixUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_model_matrix");
		viewMatrixUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_view_matrix");
		projectionMatrixUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_projection_matrix");
		doubleTapUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_double_tap");
		laUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_La");		
		ldUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_Ld");
		lsUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_Ls");
		
		kaUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_Ka");
		kdUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_Kd");
		ksUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_Ks");
		
		materialShininessUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_material_shininess");
		lightPositionUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_light_position");
		//DATA: vertices, colors, shader attribs, vao, vbos initialization
		
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
		
		// vao for lights
        GLES32.glGenVertexArrays(1,vao_lights,0);
        GLES32.glBindVertexArray(vao_lights[0]);				
		
		
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
		
		GLES32.glBindVertexArray(0);
		
		//enable depth testing
		GLES32.glEnable(GLES32.GL_DEPTH_TEST);
		GLES32.glDepthFunc(GLES32.GL_LEQUAL);
		GLES32.glEnable(GLES32.GL_CULL_FACE); //No culling for animation
		GLES32.glClearColor(0.25f, 0.25f, 0.25f, 1.0f); 
		
		//set projection matrix to identity matrix
		Matrix.setIdentityM(perspectiveProjectionMatrix, 0);
		
	}
	
	private void resize(int width, int height){
		if(height == 0) {
            height = 1;
        }
		
		viewportWidth = width / 6;
        viewportHeight = height / 6;
        x = viewportWidth / 8;
        y = viewportHeight;

		
		//perspective projection
		Matrix.perspectiveM(perspectiveProjectionMatrix, 0, 45.0f,((float)width/(float)height),0.1f,100.0f);
	}
	
	public void draw(){
		GLES32.glClear(GLES32.GL_COLOR_BUFFER_BIT|GLES32.GL_DEPTH_BUFFER_BIT);
		 for(int i = 0; i < 6; i++)
        {
            for(int j = 0; j < 4; ++j)
            {
			//use shader program
			GLES32.glViewport(i * viewportWidth, y+(j * viewportHeight), viewportWidth, viewportHeight);
			
			GLES32.glUseProgram(shaderProgramObject);
		if(gbLight==true) {
			GLES32.glUniform1i(doubleTapUniform, 1);
			GLES32.glUniform3fv(laUniform, 1, light_ambient, 0);
			GLES32.glUniform3fv(ldUniform, 1, light_diffuse, 0);
			GLES32.glUniform3fv(lsUniform, 1, light_specular, 0);	
			//set light rotation
			switch(tapCount)
			{
			case 1: //x axis
				light_position[0] = 0.0f;
				light_position[1] = myRadius * (float)Math.cos(angle);
				light_position[2] = myRadius * (float)Math.sin(angle);
				
				break;
			case 2: //y axis
				light_position[0] = myRadius * (float)Math.cos(angle);
				light_position[1] = 0.0f;
				light_position[2] = myRadius * (float)Math.sin(angle);
				
				break;
			case 3: //z axis
				light_position[0] = myRadius * (float)Math.cos(angle);
				light_position[2] = 0.0f;
				light_position[1] = myRadius * (float)Math.sin(angle);
				
				break;
			default:
				break;
			}
		GLES32.glUniform4fv(lightPositionUniform, 1, light_position, 0);
		GLES32.glUniform3fv(kaUniform, 1, material_ambient[j][i], 0);
		GLES32.glUniform3fv(kdUniform, 1, material_diffuse[j][i], 0);
		GLES32.glUniform3fv(ksUniform, 1, material_specular[j][i], 0);
		GLES32.glUniform1f(materialShininessUniform, material_shininess[j][i]);
		
		}
			else{
				GLES32.glUniform1i(doubleTapUniform, 0);
			}
		drawSpheres();
		
		GLES32.glUseProgram(0);
		
			}
		}
		spin(); //change angle of rotation
		//SwapBuffers 
		requestRender();
	}
	

	void spin(){
		angle =  angle + 0.01f;
		if (angle >= 360.0f) {
			angle = 0.0f;
		}
	}
	
	void drawSpheres() {
		float modelMatrix[] = new float[16];
		float viewMatrix[] = new float [16];
		float rotationMatrix[] = new float [16];
		//set modelview and modelview projection matrix to identity
		Matrix.setIdentityM(modelMatrix, 0);
		Matrix.setIdentityM(viewMatrix, 0);
		
		
		//multiply modelview and projection matrix to get modelViewProjection matrix
		Matrix.translateM(modelMatrix, 0, 0.0f, 0.0f, -2.5f);
		
		GLES32.glUniformMatrix4fv(modelMatrixUniform, 1, false, modelMatrix, 0);
		GLES32.glUniformMatrix4fv(viewMatrixUniform, 1, false, viewMatrix, 0);
			
		GLES32.glUniformMatrix4fv(projectionMatrixUniform, 1, false, perspectiveProjectionMatrix, 0);
		
		GLES32.glBindVertexArray(vao_sphere[0]);
			
		// *** draw, either by glDrawTriangles() or glDrawArrays() or glDrawElements()
		GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element[0]);
		GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);
			
		// unbind vao
		GLES32.glBindVertexArray(0);
	
	
	
	
	}
	
	void uninitialize(){
	
		
		// destroy vao
        if(vao_sphere[0] != 0)
        {
            GLES32.glDeleteVertexArrays(1, vao_sphere, 0);
            vao_sphere[0]=0;
        }
        if(vao_lights[0] != 0)
        {
            GLES32.glDeleteVertexArrays(1, vao_lights, 0);
            vao_lights[0]=0;
        }
		if(vbo_light_position[0] != 0)
        {
            GLES32.glDeleteBuffers(1, vbo_light_position, 0);
            vbo_light_position[0]=0;
        }
        // destroy position vbo
        if(vbo_sphere_position[0] != 0)
        {
            GLES32.glDeleteBuffers(1, vbo_sphere_position, 0);
            vbo_sphere_position[0]=0;
        }
        
        // destroy normal vbo
        if(vbo_sphere_normal[0] != 0)
        {
            GLES32.glDeleteBuffers(1, vbo_sphere_normal, 0);
            vbo_sphere_normal[0]=0;
        }
        
        // destroy element vbo
        if(vbo_sphere_element[0] != 0)
        {
            GLES32.glDeleteBuffers(1, vbo_sphere_element, 0);
            vbo_sphere_element[0]=0;
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