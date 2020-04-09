//global variables
var canvas_ata=null;
var gl=null; //for webgl context
var bFullscreen_ata = false;
var canvas_ata_original_width;
var canvas_ata_original_height;

const WebGLMacros=
{
	ATA_ATTRIBUTE_POSITION:0,
	ATA_ATTRIBUTE_COLOR:1,
	ATA_ATTRIBUTE_NORMAL:2,
	ATA_ATTRIBUTE_TEXTURE0:3
};

var vertexShaderObject_ata;
var fragmentShaderObject_ata;
var shaderProgramObject_ata;

var myViewportWidth;
var myViewportHeight;
var x;
var y;

var modelUniformAta, viewUniformAta, projectionUniformAta;
var lightPositionUniformAta, gLight0PositionUniform, lKeyPressUniformAta;
var laUniformAta, L0d_uniform, L0s_uniform;
var kaUniformAta, kdUniform, ksUniformAta, materialShininessUniformAta;
var isPerVertexLightingUniform, isPerFragmentLightingUniform;

var gbLightAta=false;
var isPerVertex = true;
var lightOnXAta = true;
var lightOnYAta = false;
var lightOnZAta = false;
var mvpUniform_ata;
var gAngle_ata=0.0;

var perspectiveProjectionMatrixAta;
var myRadius = 100;
var light_ambientAta = [0.0, 0.0, 0.0];

var light0_diffuseAta = [1.0, 1.0, 1.0];
var light0_specularAta = [1.0, 1.0, 1.0];

//var light_positionAta = [100.0, 100.0, 100.0, 1.0];
var light_positionAta = [0.0, 0.0, 1.0, 0.0];

var material_ambientAta = [
    [
        // Column 1
        [0.0215, 0.1745, 0.0215],
        [0.135, 0.2225, 0.1575],
        [0.05375, 0.05, 0.06625],
        [0.25, 0.20725, 0.20725],
        [0.1745, 0.01175, 0.01175],
        [0.1, 0.18725, 0.1745]
    ],
    [
        // Column 2
        [0.329412, 0.223529, 0.027451],
        [0.2125, 0.1275, 0.054],
        [0.25, 0.25, 0.25],
        [0.19125, 0.0735, 0.0225],
        [0.24725, 0.1995, 0.0745],
        [0.19225, 0.19225, 0.19225]
    ],
    [
        // Column 3
        [0.0, 0.0, 0.0],
        [0.0, 0.1, 0.06],
        [0.0, 0.0, 0.0],
        [0.0, 0.0, 0.0],
        [0.0, 0.0, 0.0],
        [0.0, 0.0, 0.0]
    ],
    [
        // Column 4
        [0.02, 0.02, 0.02],
        [0.0, 0.05, 0.05],
        [0.0, 0.05, 0.0],
        [0.05, 0.0, 0.0],
        [0.05, 0.05, 0.05],
        [0.05, 0.05, 0.0]
    ]
];

var material_diffuseAta = [
    [
        // Column 1
        [0.07568, 0.61424, 0.07568],
        [0.54, 0.89, 0.63],
        [0.18275, 0.17, 0.22525],
        [1.0, 0.829, 0.829],
        [0.61424, 0.04136, 0.04136],
        [0.396, 0.74151, 0.69102]
    ],
    [
        // Column 2
        [0.780392, 0.568627, 0.113725],
        [0.714, 0.4284, 0.18144],
        [0.4, 0.4, 0.4],
        [0.7038, 0.27048, 0.0828],
        [0.75164, 0.60648, 0.22648],
        [0.50754, 0.50754, 0.50754]
    ],
    [
        // Column 3
        [0.01, 0.01, 0.01],
        [0.0, 0.50980392, 0.50980392],
        [0.1, 0.35, 0.1],
        [0.5, 0.0, 0.0],
        [0.55, 0.55, 0.55],
        [0.5, 0.5, 0.0]
    ],
    [
        // Column 4
        [0.01, 0.01, 0.01],
        [0.4, 0.5, 0.5],
        [0.4, 0.5, 0.4],
        [0.5, 0.4, 0.4],
        [0.5, 0.5, 0.5],
        [0.5, 0.5, 0.4]
    ]
];

var material_specularAta = [
    [
        // Column 1
        [0.633, 0.727811, 0.633],
        [0.316228, 0.316228, 0.316228],
        [0.332741, 0.328634, 0.346435],
        [0.296648, 0.296648, 0.296648],
        [0.727811, 0.626959, 0.626959],
        [0.297254, 0.30829, 0.306678]
    ],
    [
        // Column 2
        [0.992157, 0.941176, 0.807843],
        [0.393548, 0.271906, 0.166721],
        [0.774597, 0.774597, 0.774597],
        [0.256777, 0.137622, 0.086014],
        [0.628281, 0.555802, 0.366065],
        [0.508273, 0.508273, 0.508273]
    ],
    [
        // Column 3
        [0.5, 0.5, 0.5],
        [0.50196078, 0.50196078, 0.50196078],
        [0.45, 0.55, 0.45],
        [0.7, 0.6, 0.6],
        [0.7, 0.7, 0.7],
        [0.6, 0.6, 0.5]
    ],
    [
        // Column 4
        [0.4, 0.4, 0.4],
        [0.04, 0.7, 0.7],
        [0.04, 0.7, 0.04],
        [0.7, 0.04, 0.04],
        [0.7, 0.7, 0.7],
        [0.7, 0.7, 0.04]
    ]
];

var material_shininessAta = [
    [
        // Column 1
        0.6 * 128.0,
        0.1 * 128.0,
        0.3 * 128.0,
        0.088 * 128.0,
        0.6 * 128.0,
        0.1 * 128.0
    ],
    [
        // Column 2
        0.21794872 * 128.0,
        0.2 * 128.0,
        0.6 * 128.0,
        0.1 * 128.0,
        0.4 * 128.0,
        0.4 * 128.0
    ],
    [
        // Column 3
        0.25 * 128.0,
        0.25 * 128.0,
        0.25 * 128.0,
        0.25 * 128.0,
        0.25 * 128.0,
        0.25 * 128.0
    ],
    [
        // Column 4
        0.078125 * 128.0,
        0.078125 * 128.0,
        0.078125 * 128.0,
        0.078125 * 128.0,
        0.078125 * 128.0,
        0.078125 * 128.0
    ]
];

var sphere = null;

//To start animation
var requestAnimationFrame = window.requestAnimationFrame ||
							window.webkitRequestAnimationFrame||
							window.mozRequestAnimationFrame||
							window.oRequestAnimationFrame||
							window.msRequestAnimationFrame;

//To stop animation
var cancelAnimationFrame = 
			window.cancelAnimationFrame||
			window.webkitCancelRequestAnimationFrame||
			window.webkitCancelAnimationFrame||
			window.mozCancelRequestAnimationFrame||
			window.mozCancelAnimationFrame||
			window.oCancelRequestAnimationFrame||
			window.oCancelAnimationFrame||
			window.msCancelRequestAnimationFrame||
			window.msCancelAnimationFrame;
			
//on body load function
function main()
{
	//get canvas_ata elementFromPoint
	canvas_ata = document.getElementById("AMC");
	if(!canvas_ata)
		console.log("Obtaining canvas failed\n");
	else
		console.log("Obtaining canvas succeeded\n");
	//print obtained canvas_ata width and height on console
	console.log("canvas width:" + canvas_ata.width +" height:" +canvas_ata.height);
	canvas_ata_original_width = canvas_ata.width;
	canvas_ata_original_height = canvas_ata.height;
	
	
	//register keyboard and mouse event with window class
	window.addEventListener("keydown", keydown, false);
	window.addEventListener("click", mouseDown, false);
	window.addEventListener("resize", resize, false);
	
	init();
	resize();
	draw();
}

function init(){
	//Get OpenGL context
	gl=canvas_ata.getContext("webgl2");
	if(gl == null)
		console.log("Obtaining  webgl2 failed\n");
	else
		console.log("Obtaining webgl2 succeeded\n");
	
	gl.viewportWidth  = canvas_ata.width;
	gl.viewportHeight  = canvas_ata.height;
	
	//vertexShader
	var vertexShaderSourceCode_ata =
		"#version 300 es" +
		"\n" +
		"in vec4 vPosition;" +
		"in vec3 vNormal;" +
		"uniform mat4 u_model_matrix;" +
		"uniform mat4 u_view_matrix;" +
		"uniform mat4 u_projection_matrix;" +
		"uniform vec4 u_light_positionAta;" +
		"out vec3 tnorm;" +
		"out vec3 light_direction;" +
		"out vec3 viewer_vector;" +
		"void main(void)" +
		"{" +
		"vec4 eye_coordinate = u_view_matrix * u_model_matrix * vPosition;" +
		"tnorm = mat3(u_view_matrix * u_model_matrix) * vNormal;" +
		"light_direction = vec3(u_light_positionAta - eye_coordinate);" +
		"viewer_vector = vec3(-eye_coordinate.xyz);" +

		"gl_Position = u_projection_matrix * u_view_matrix * u_model_matrix * vPosition;" +
		"}";
	vertexShaderObject_ata=gl.createShader(gl.VERTEX_SHADER);
	gl.shaderSource(vertexShaderObject_ata, vertexShaderSourceCode_ata);
	gl.compileShader(vertexShaderObject_ata);
	if(gl.getShaderParameter(vertexShaderObject_ata,gl.COMPILE_STATUS) == false)
	{
		var error_ata=gl.getShaderInfoLog(vertexShaderObject_ata);
		if(error_ata.length > 0)
		{
			alert(error_ata);
			uninitialize();
		}
	}
	
	//fragmentShader
	var fragmentShaderSource_ata =
		"#version 300 es"+
		"\n"+
		"precision highp float;"+
		"vec3 phong_ads_light;" +
		"in vec3 tnorm;" +
		"in vec3 light_direction;" +
		"in vec3 viewer_vector;" +
		"uniform vec3 u_La;" +
		"uniform vec3 u_Ld;" +
		"uniform vec3 u_Ls;" +
		"uniform vec3 u_Ka;" +
		"uniform vec3 u_Kd;" +
		"uniform vec3 u_Ks;" +
		"uniform float u_material_shininess;" +
		"uniform vec4 u_light_positionAta;" +
		"uniform int u_lkeypress;" +
		"out vec4 FragColor;" +
		"void main(void)" +
		"{" +
		"if (u_lkeypress == 1)" +
		"{" +
		"vec3 norm_tnorm = normalize(tnorm);" +
		"vec3 norm_light_direction = normalize(light_direction);" +
		"float tn_dot_lightd = max(dot(norm_light_direction,norm_tnorm),0.0);" +
		"vec3 reflection_vector = reflect(-norm_light_direction, norm_tnorm);" +
		"vec3 norm_viewer_vector = normalize(viewer_vector);" +
		"vec3 ambient = u_La * u_Ka;" +
		"vec3 diffuse = u_Ld * u_Kd * tn_dot_lightd;" +
		"vec3 specular = u_Ls * u_Ks * pow(max(dot(reflection_vector,norm_viewer_vector),0.0),u_material_shininess);" +
		"phong_ads_light = ambient + diffuse + specular;" +
		"}" +
		"else" +
		"{" +
		"phong_ads_light = vec3(1.0,1.0,1.0);" +
		"}" +
		"FragColor = vec4(phong_ads_light,1.0);" +
		"}";
	fragmentShaderObject_ata = gl.createShader(gl.FRAGMENT_SHADER);
	gl.shaderSource(fragmentShaderObject_ata,fragmentShaderSource_ata);
	gl.compileShader(fragmentShaderObject_ata);
	if(gl.getShaderParameter(fragmentShaderObject_ata,gl.COMPILE_STATUS) == false)
	{
		var error_ata=gl.getShaderInfoLog(fragmentShaderObject_ata);
		if(error_ata.length > 0)
		{
			alert(error_ata);
			uninitialize();
		}
	}
	//shader program
	shaderProgramObject_ata=gl.createProgram();
	gl.attachShader(shaderProgramObject_ata, vertexShaderObject_ata);
	gl.attachShader(shaderProgramObject_ata, fragmentShaderObject_ata);
	
	//pre-link binidng of shader program object with vertex shader attributes
	gl.bindAttribLocation(shaderProgramObject_ata, WebGLMacros.ATA_ATTRIBUTE_POSITION, "vPosition");
	gl.bindAttribLocation(shaderProgramObject_ata, WebGLMacros.ATA_ATTRIBUTE_NORMAL, "vNormal");
	
	//linking
	gl.linkProgram(shaderProgramObject_ata);
	if(!gl.getProgramParameter(shaderProgramObject_ata, gl.LINK_STATUS))
	{
		var error_ata = gl.getProgramInfoLog(shaderProgramObject_ata);
		if(error_ata.length > 0)
		{
			alert(error_ata);
			uninitialize();
		}
	}
	
	
	//get MVP uniform
	//mvpUniform_ata = gl.getUniformLocation(shaderProgramObject_ata, "u_mvp_matrix");
	modelUniformAta = gl.getUniformLocation(shaderProgramObject_ata, "u_model_matrix");
	viewUniformAta = gl.getUniformLocation(shaderProgramObject_ata, "u_view_matrix");
	projectionUniformAta = gl.getUniformLocation(shaderProgramObject_ata, "u_projection_matrix");
	lKeyPressUniformAta = gl.getUniformLocation(shaderProgramObject_ata, "u_lkeypress");
	laUniformAta = gl.getUniformLocation(shaderProgramObject_ata, "u_La");
	l0dUniformAta = gl.getUniformLocation(shaderProgramObject_ata, "u_Ld");
	l0sUniformAta = gl.getUniformLocation(shaderProgramObject_ata, "u_Ls");
	
	kaUniformAta = gl.getUniformLocation(shaderProgramObject_ata, "u_Ka");
	kdUniform = gl.getUniformLocation(shaderProgramObject_ata, "u_Kd");
	ksUniformAta = gl.getUniformLocation(shaderProgramObject_ata, "u_Ks");

	lightPositionUniformAta = gl.getUniformLocation(shaderProgramObject_ata, "u_light_positionAta");
	
	materialShininessUniformAta = gl.getUniformLocation(shaderProgramObject_ata, "u_material_shininess");
	
	sphere = new Mesh();
	makeSphere(sphere, 2.0, 50, 50);
	
	gl.clearColor(0.25,0.25,0.25,1.0);
	gl.enable(gl.DEPTH_TEST);
	gl.depthFunc(gl.LEQUAL);
	gl.enable(gl.CULL_FACE);
	perspectiveProjectionMatrixAta =  mat4.create();
}

function resize()
{
	if(bFullscreen_ata == true)
	{
		canvas_ata.width = window.innerWidth;
		canvas_ata.height = window.innerHeight;		
	}else
	{
		canvas_ata.width = canvas_ata_original_width;
		canvas_ata.height = canvas_ata_original_height;
	}
	
	gl.viewport(0,0,canvas_ata.width,canvas_ata.height);
	
	myViewportWidth = canvas_ata.width / 6;
    myViewportHeight = canvas_ata.height / 6;
    x = myViewportWidth / 8;
    y = myViewportHeight;

	mat4.perspective(perspectiveProjectionMatrixAta, 45.0, parseFloat(canvas_ata.width/canvas_ata.height),0.1, 100.0);		
	
	
	
}
function draw()
{
	gl.clear(gl.COLOR_BUFFER_BIT|gl.DEPTH_BUFFER_BIT);
	
	
	for (var i = 0; i < 6; i++) 
	{
        for (var j = 0; j < 4; j++) 
		{
            gl.viewport((i * myViewportWidth), y + (j * myViewportHeight), myViewportWidth, myViewportHeight);

            gl.useProgram(shaderProgramObject_ata);

            if (gbLightAta) 
			{
                gl.uniform1i(lKeyPressUniformAta, 1);

                gl.uniform3fv(laUniformAta, light_ambientAta);
                gl.uniform3fv(l0dUniformAta, light0_diffuseAta);
                gl.uniform3fv(l0sUniformAta, light0_specularAta);

                if (lightOnXAta) 
				{
                    light_positionAta[0] = 0.0;
                    light_positionAta[1] = myRadius * Math.sin(gAngle_ata);
                    light_positionAta[2] = myRadius * Math.cos(gAngle_ata);
                } 
				else if (lightOnYAta) 
				{
                    light_positionAta[0] = myRadius * Math.sin(gAngle_ata);
                    light_positionAta[1] = 0.0;
                    light_positionAta[2] = myRadius * Math.cos(gAngle_ata);
                } 
				else if (lightOnZAta) 
				{
                    light_positionAta[0] = myRadius * Math.sin(gAngle_ata);
                    light_positionAta[1] = myRadius * Math.cos(gAngle_ata);
                    light_positionAta[2] = 0.0;
                }

                gl.uniform4fv(lightPositionUniformAta, light_positionAta);
                gl.uniform3fv(kaUniformAta, material_ambientAta[j][i]);
                gl.uniform3fv(kdUniform, material_diffuseAta[j][i]);
                gl.uniform3fv(ksUniformAta, material_specularAta[j][i]);
                gl.uniform1f(materialShininessUniformAta, material_shininessAta[j][i]);
            } 
			else 
			{
                gl.uniform1i(lKeyPressUniformAta, 0);
            }

            drawSphere();
			gl.useProgram(null);
		}
	}
	update();
	requestAnimationFrame(draw,canvas_ata);
}

function drawSphere() {
    let modelMatrix_ata = mat4.create();
    let viewMatrix_ata = mat4.create();

    mat4.translate(modelMatrix_ata, modelMatrix_ata, [0.0, 0.0, -6.0]);

    gl.uniformMatrix4fv(modelUniformAta, false, modelMatrix_ata);
    gl.uniformMatrix4fv(viewUniformAta, false, viewMatrix_ata);
    gl.uniformMatrix4fv(projectionUniformAta, false, perspectiveProjectionMatrixAta);

    sphere.draw();
}

function toggleFullScreen()
{
	//code
	var fullScreen_element = 
		document.fullscreenElement||
		document.webkitFullscreenElement||
		document.mozFullScreenElement||
		document.msFullscreenElement||
		null;
		
	//if not full screen
	if(fullScreen_element == null)
	{
		if(canvas_ata.requestFullscreen)
			canvas_ata.requestFullscreen();
		else if(canvas_ata.mozRequestFullScreen)
			canvas_ata.mozRequestFullScreen();
		else if(canvas_ata.webkitRequestFullscreen)
			canvas_ata.webkitRequestFullscreen();
		else if(canvas_ata.msRequestFullscreen)
			canvas_ata.msRequestFullscreen();
	}
	else //restore from fullscreen
	{
			if(document.exitFullscreen)
				document.exitFullscreen();
			else if(document.mozCancelFullScreen)
				document.mozCancelFullScreen();
			else if(document.webkitExitFullscreen)
				document.webkitExitFullscreen();
			else if(document.msExitFullscreen)
				document.msExitFullscreen();
	
	}
	resize();
}

function keydown(event)
{
	switch(event.keyCode)
	{
		case 27://Esc
			uninitialize();
			window.close();
			break;
		case 70: //for 'F' or 'f'
			if(bFullscreen_ata == true)
			{	
				bFullscreen_ata = false;
			}
			else
			{			
				bFullscreen_ata = true;							
			}
			toggleFullScreen();
			break;
			
		case 76:
			if(gbLightAta)
				gbLightAta = false;
			else
				gbLightAta = true;
			break;
		case 86:
			if(isPerVertex)
			isPerVertex = false;
		else
			isPerVertex = true;
			break;
			
		case 88:
            lightOnXAta = true;
            lightOnYAta = false;
            lightOnZAta = false;
            break;

        case 89:
			lightOnXAta = false;
            lightOnYAta = true;
            lightOnZAta = false;
            break;

        case 90:
			lightOnXAta = false;
            lightOnYAta = false;
            lightOnZAta = true;
            break;
	}
}
function update()
{
		if( gAngle_ata >= 360.0)
			gAngle_ata = 0.0;
		else
			gAngle_ata = gAngle_ata + 0.1;
}
function degreeToRadian(angleInDegree)
{
	return (angleInDegree *  Math.PI/ 180);
}
function mouseDown()
{
	//alert("Mouse is clicked");
}

function uninitialize()
{
	if (sphere)
	{
		sphere.deallocate();
		sphere = null;
	}

		
	if(shaderProgramObject_ata)
	{
		if(fragmentShaderObject_ata)
		{
			gl.detachShader(shaderProgramObject_ata, fragmentShaderObject_ata);
			fragmentShaderObject_ata = null;
		}
		
		if(vertexShaderObject_ata)
		{
			gl.detachShader(shaderProgramObject_ata, vertexShaderObject_ata);
			vertexShaderObject_ata = null;
		}
		gl.deleteProgram(shaderProgramObject_ata);
		shaderProgramObject_ata = null;
	}
}