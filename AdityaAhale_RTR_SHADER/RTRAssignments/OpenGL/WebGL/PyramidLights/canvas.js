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

var vao_pyramidAta;
var vbo_positionAta;
var vbo_normalAta;

var modelUniformAta, viewUniformAta, projectionUniformAta;
var light0PositionUniform, light1PositionUniform, lKeyPressUniform;
var laUniformAta, l0dUniformAta, l0sUniformAta, l1dUniformAta, l1sUniformAta;
var kaUniformAta, kdUniformAta, ksUniformAta, materialShininessUniformAta;

var gbLightAta=false;
var mvpUniform_ata;
var gAngle_ata=0.0;

var perspectiveProjectionMatrixAta;
var radius = 100;

var light_ambientAta = [0.0, 0.0, 0.0];

var light0_diffuseAta = [1.0, 0.0, 0.0];
var light0_specularAta = [1.0, 0.0, 0.0];

var light1_diffuseAta = [0.0, 0.0, 1.0];
var light1_specularAta = [0.0, 0.0, 1.0];

var light0_positionAta = [2.0, 2.0, 0.0, 1.0];
var light1_positionAta = [-2.0, 2.0, 0.0, 1.0];

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
		"\n" +
		"in vec4 vPosition;" +
		"in vec3 vNormal;" +
		"uniform mat4 u_model_matrix;" +
		"uniform mat4 u_view_matrix;" +
		"uniform mat4 u_projection_matrix;" +
		"uniform int u_lkeypress;" +
		"uniform vec3 u_La;" +
		"uniform vec3 u_L0d;" +
		"uniform vec3 u_L0s;" +
		"uniform vec4 u_light0_position;" +
		"uniform vec3 u_L1d;" +
		"uniform vec3 u_L1s;" +
		"uniform vec4 u_light1_position;" +
		"uniform vec3 u_Ka;" +
		"uniform vec3 u_Kd;" +
		"uniform vec3 u_Ks;" +
		"uniform float u_material_shininess;" +
		"out vec3 phong_ads_light;" +
		"void main(void)" +
		"{" +
		"if(u_lkeypress == 1)" +
		"{"+
		"vec4 eye_coordinates = u_view_matrix* u_model_matrix * vPosition;" +
		"vec3 transformed_normals = normalize(mat3(u_view_matrix*u_model_matrix) * vNormal);" +
		"vec3 light0_direction = normalize(vec3(u_light0_position) - eye_coordinates.xyz);" +
		"vec3 light1_direction = normalize(vec3(u_light1_position) - eye_coordinates.xyz);" +
		"float tn_dot_ld0 = max(dot(transformed_normals, light0_direction), 0.0);" +
		"float tn_dot_ld1 = max(dot(transformed_normals, light1_direction), 0.0);" +
		"vec3 ambient = u_La * u_Ka;" +
		"vec3 diffuse0 = u_L0d * u_Kd * tn_dot_ld0;" +
		"vec3 diffuse1 = u_L1d * u_Kd * tn_dot_ld1;" +
		"vec3 reflection_vector0 = reflect(-light0_direction, transformed_normals);" +
		"vec3 reflection_vector1 = reflect(-light1_direction, transformed_normals);" +
		"vec3 viewer_vector = normalize(-eye_coordinates.xyz);" +
		"vec3 specular0 = u_L0s * u_Ks * pow(max(dot(reflection_vector0, viewer_vector),0.0), u_material_shininess);" +
		"vec3 specular1 = u_L1s * u_Ks * pow(max(dot(reflection_vector1, viewer_vector),0.0), u_material_shininess);" +
		"phong_ads_light = ambient + diffuse0 + diffuse1 + specular0 + specular1;" +
		"}" +
		"else" +
		"{" +
		"phong_ads_light = vec3(1.0, 1.0, 1.0);" +
		"}"+
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
	lKeyPressUniform = gl.getUniformLocation(shaderProgramObject_ata, "u_lkeypress");
	laUniformAta = gl.getUniformLocation(shaderProgramObject_ata, "u_La");
	l0dUniformAta = gl.getUniformLocation(shaderProgramObject_ata, "u_L0d");
	l0sUniformAta = gl.getUniformLocation(shaderProgramObject_ata, "u_L0s");
	
	l1dUniformAta = gl.getUniformLocation(shaderProgramObject_ata, "u_L1d");
	l1sUniformAta = gl.getUniformLocation(shaderProgramObject_ata, "u_L1s");
	
	kaUniformAta = gl.getUniformLocation(shaderProgramObject_ata, "u_Ka");
	kdUniformAta = gl.getUniformLocation(shaderProgramObject_ata, "u_Kd");
	ksUniformAta = gl.getUniformLocation(shaderProgramObject_ata, "u_Ks");

	//lightPositionUniform = gl.getUniformLocation(shaderProgramObject_ata, "u_light_position");
	light0PositionUniform = gl.getUniformLocation(shaderProgramObject_ata, "u_light0_position");
	light1PositionUniform = gl.getUniformLocation(shaderProgramObject_ata, "u_light1_position");
	
	materialShininessUniformAta = gl.getUniformLocation(shaderProgramObject_ata, "u_material_shininess");

	var pyramidVertices = new Float32Array([
										//front face
										0.0, 1.0, 0.0, //apex of the triangle
										-1.0, -1.0, 1.0, //left-bottom
										1.0, -1.0, 1.0, //right-bottom
										//right face
										0.0, 1.0, 0.0, //apex
										1.0, -1.0, 1.0,//left bottom
										1.0, -1.0, -1.0, //right bottom
										//back face
										0.0, 1.0, 0.0, //apex
										1.0, -1.0, -1.0,
										-1.0, -1.0, -1.0,
										//left face
										0.0, 1.0, 0.0, //apex
										-1.0, -1.0, -1.0, //left bottom
										-1.0, -1.0, 1.0 //right bottom																		
											]);
	vao_pyramidAta=gl.createVertexArray();
	gl.bindVertexArray(vao_pyramidAta);
	//vbo for positions
	vbo_positionAta =  gl.createBuffer();
	gl.bindBuffer(gl.ARRAY_BUFFER, vbo_positionAta);
	gl.bufferData(gl.ARRAY_BUFFER, pyramidVertices, gl.STATIC_DRAW);
	gl.vertexAttribPointer(WebGLMacros.ATA_ATTRIBUTE_POSITION,
							3,
							gl.FLOAT,
							false, 0, 0);
	gl.enableVertexAttribArray(WebGLMacros.ATA_ATTRIBUTE_POSITION);
	gl.bindBuffer(gl.ARRAY_BUFFER, null);
	
//Preparation to for pyramid normals
	
	var pyramidNormals = new Float32Array([
								0.0, 0.447214, 0.894427, //front face normals
								0.0, 0.447214, 0.894427, //front face normals
								0.0, 0.447214, 0.894427, //front face normals

								0.894427, 0.447214, 0.0, //right face
								0.894427, 0.447214, 0.0, //right face
								0.894427, 0.447214, 0.0, //right face

								0.0, 0.447214, -0.894427, //back face
								0.0, 0.447214, -0.894427, //back face
								0.0, 0.447214, -0.894427, //back face

								-0.894427, 0.447214, 0.0, //left face
								-0.894427, 0.447214, 0.0, //left face
								- 0.894427, 0.447214, 0.0 //left face
								]);
	
	vbo_normalAta =  gl.createBuffer();
	gl.bindBuffer(gl.ARRAY_BUFFER, vbo_normalAta);
	gl.bufferData(gl.ARRAY_BUFFER, pyramidNormals, gl.STATIC_DRAW);
	gl.vertexAttribPointer(WebGLMacros.ATA_ATTRIBUTE_NORMAL,
							3,
							gl.FLOAT,
							false, 0, 0);
	gl.enableVertexAttribArray(WebGLMacros.ATA_ATTRIBUTE_NORMAL);
	gl.bindBuffer(gl.ARRAY_BUFFER, null);


	gl.clearColor(0.0,0.0,0.0,1.0);
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
	
	if(gbLightAta)
	{
	gl.uniform1i(lKeyPressUniform, 1);
	
	gl.uniform3fv(laUniformAta, light_ambientAta);
	
	gl.uniform3fv(l0dUniformAta, light0_diffuseAta);
	gl.uniform3fv(l0sUniformAta, light0_specularAta);
	
	gl.uniform4fv(light0PositionUniform, light0_positionAta);
	
	gl.uniform3fv(l1dUniformAta, light1_diffuseAta);
	gl.uniform3fv(l1sUniformAta, light1_specularAta);
	
	gl.uniform4fv(light1PositionUniform, light1_positionAta);
	
	//set material properties
	gl.uniform3fv(kaUniformAta, material_ambientAta);
	gl.uniform3fv(kdUniformAta, material_diffuseAta);
	gl.uniform3fv(ksUniformAta, material_specularAta);
	gl.uniform1f(materialShininessUniformAta,material_shininessAta);
	}
	else
	{
		gl.uniform1i(lKeyPressUniform,0);
	}
	
	
	var angleInRadian = degreeToRadian(gAngle_ata);
	//gl.uniform4fv(lightPositionUniform, light_position);
	
	//draw pyramid
	//var modelMatrix = mat4.create();
	var modelMatrix_ata = mat4.create();
	var viewMatrix_ata = mat4.create();
	//var modelViewProjectionMatrix_ata =  mat4.create();

	//draw sphere
	mat4.translate(modelMatrix_ata, modelMatrix_ata, [0.0,0.0,-4.0]);
	mat4.rotateY(modelMatrix_ata, modelMatrix_ata,angleInRadian);
	
	gl.uniformMatrix4fv(modelUniformAta,false,modelMatrix_ata);
	gl.uniformMatrix4fv(viewUniformAta,false,viewMatrix_ata);
	gl.uniformMatrix4fv(projectionUniformAta,false,perspectiveProjectionMatrixAta);
	
	gl.bindVertexArray(vao_pyramidAta);
	gl.drawArrays(gl.TRIANGLES, 0,12);
	gl.bindVertexArray(null);
	
	gl.useProgram(null);
	update();
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
		case 86:
			if(isPerVertex)
			isPerVertex = false;
		else
			isPerVertex = true;
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
	if(vao_pyramidAta)
	{
		gl.deleteVertexArray(vao_pyramidAta);
		vao_pyramidAta = null;
	}
	if(vbo_positionAta)
	{
		gl.deleteBuffer(vbo_positionAta);
		vbo_positionAta=null;
	}
	if(vbo_normalAta)
	{
		gl.deleteBuffer(vbo_normalAta);
		vbo_normalAta=null;
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