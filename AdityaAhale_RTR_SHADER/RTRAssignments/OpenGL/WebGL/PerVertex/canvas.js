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


var modelUniformAta, viewUniformAta, projectionUniformAta;
var lightPositionUniformAta, lKeyPressUniformAta;
var laUniformAta, ldUniformAta, lsUniformAta;
var kaUniformAta, kdUniformAta, ksUniformAta, materialShininessUniformAta;

var gbLightAta=false;
var mvpUniform_ata;
var gAngle_ata=0.0;

var perspectiveProjectionMatrixAta;

var light_ambientAta = [0.0, 0.0, 0.0];
var light_diffuseAta = [1.0, 1.0, 1.0];
var light_specularAta = [1.0, 1.0, 1.0];
var light_positionAta = [100.0, 100.0, 100.0, 1.0];

var material_ambientAta = [0.0, 0.0, 0.0];
var material_diffuseAta = [1.0, 1.0, 1.0];
var material_specularAta = [1.0, 1.0, 1.0];

var material_shininessAta = 50.0;

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
		console.log("Obtaining webgl2 failed\n");
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
		"uniform int u_lkeypress;" +
		"uniform vec3 u_La;" +
		"uniform vec3 u_Ld;" +
		"uniform vec3 u_Ls;" +
		"uniform vec4 u_light_position;" +
		"uniform vec3 u_Ka;" +
		"uniform vec3 u_Kd;" +
		"uniform vec3 u_Ks;" +
		"uniform float u_material_shininess;" +
		"out vec3 phong_ads_light;" +
		"void main(void)" +
		"{" +
		"if (u_lkeypress == 1)" +
		"{" +
		"vec4 eye_coordinates=u_view_matrix * u_model_matrix * vPosition;" +
		"vec3 transformed_normals=normalize(mat3(u_view_matrix * u_model_matrix) * vNormal);" +
		"vec3 light_direction = normalize(vec3(u_light_position) - eye_coordinates.xyz);" +
		"float tn_dot_ld = max(dot(transformed_normals, light_direction),0.0);" +
		"vec3 ambient = u_La * u_Ka;" +
		"vec3 diffuse = u_Ld * u_Kd * tn_dot_ld;" +
		"vec3 reflection_vector = reflect(-light_direction, transformed_normals);" +
		"vec3 viewer_vector = normalize(-eye_coordinates.xyz);" +
		"vec3 specular = u_Ls * u_Ks * pow(max(dot(reflection_vector, viewer_vector), 0.0), u_material_shininess);" +
		"phong_ads_light=ambient + diffuse + specular;" +
		"}" +
		"else" +
		"{" +
		"phong_ads_light = vec3(1.0, 1.0, 1.0);" +
		"}" +
		"gl_Position=u_projection_matrix * u_view_matrix * u_model_matrix * vPosition;" +
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
		"\n" +
		"precision highp float;" +
		"in vec3 phong_ads_light;" +
		"out vec4 FragColor;" +
		"void main(void)" +
		"{" +
		"FragColor = vec4(phong_ads_light, 1.0);" +
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
	ldUniformAta = gl.getUniformLocation(shaderProgramObject_ata, "u_Ld");
	lsUniformAta = gl.getUniformLocation(shaderProgramObject_ata, "u_Ls");

	kaUniformAta = gl.getUniformLocation(shaderProgramObject_ata, "u_Ka");
	kdUniformAta = gl.getUniformLocation(shaderProgramObject_ata, "u_Kd");
	ksUniformAta = gl.getUniformLocation(shaderProgramObject_ata, "u_Ks");

	lightPositionUniformAta = gl.getUniformLocation(shaderProgramObject_ata, "u_light_position");
	materialShininessUniformAta = gl.getUniformLocation(shaderProgramObject_ata, "u_material_shininess");

	sphere = new Mesh();
	makeSphere(sphere, 2.0, 30, 30);
	
	gl.clearColor(0.0,0.0,0.0,1.0);
	//gl.clearDepth(1.0);
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
	
	mat4.perspective(perspectiveProjectionMatrixAta, 45.0, parseFloat(canvas_ata.width/canvas_ata.height),0.1, 100.0);		
	
	gl.viewport(0,0,canvas_ata.width,canvas_ata.height);
}
function draw()
{
	gl.clear(gl.COLOR_BUFFER_BIT|gl.DEPTH_BUFFER_BIT);
	gl.useProgram(shaderProgramObject_ata);

	if (gbLightAta)
	{
		gl.uniform1i(lKeyPressUniformAta, 1);
		gl.uniform3fv(laUniformAta, light_ambientAta);
		gl.uniform3fv(ldUniformAta, light_diffuseAta);
		gl.uniform3fv(lsUniformAta, light_specularAta);
						
		gl.uniform4fv(lightPositionUniformAta, light_positionAta);
    
		gl.uniform3fv(kaUniformAta, material_ambientAta);
		gl.uniform3fv(kdUniformAta, material_diffuseAta);
		gl.uniform3fv(ksUniformAta, material_specularAta);
		gl.uniform1f(materialShininessUniformAta,material_shininessAta);
	}
	else
	{
		gl.uniform1i(lKeyPressUniformAta, 0);
	}
	
	
	//draw triangle
	//var modelMatrix = mat4.create();
	var modelMatrix_ata = mat4.create();
	var viewMatrix_ata = mat4.create();
	//var modelViewProjectionMatrix_ata =  mat4.create();
	var angleInRadian = degreeToRadian(gAngle_ata);
	
	
	
	//draw sphere
	mat4.translate(modelMatrix_ata, modelMatrix_ata, [0.0,0.0,-6.0]);
	
	gl.uniformMatrix4fv(modelUniformAta,false,modelMatrix_ata);
	gl.uniformMatrix4fv(viewUniformAta,false,viewMatrix_ata);
	gl.uniformMatrix4fv(projectionUniformAta,false,perspectiveProjectionMatrixAta);
	
	sphere.draw();
	
	gl.useProgram(null);
	//update();
	requestAnimationFrame(draw,canvas_ata);
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
	}
}
function update()
{
		if( gAngle_ata >= 360.0)
			gAngle_ata = 0.0;
		else
			gAngle_ata = gAngle_ata + 1.0;
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