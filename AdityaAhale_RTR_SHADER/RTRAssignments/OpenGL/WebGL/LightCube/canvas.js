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

var vao_pyramid_ata;
var vbo_position;
var vao_cube_ata;
var vbo_normal;
var modelViewUniformAta, projectionUniformAta;
var ldUniformAta, kdUniformAta, lightPositionUniformAta, lKeyPressedUniformAta ;
var gbLightAta=false;
var mvpUniform_ata;
var gAngle_ata=0.0;
var gbAnimateAta = false;

var perspectiveProjectionMatrixAta;


//To start animation
var requestAnimationFrame = window.requestAnimationFrame ||
							window.webkitRequestAnimationFrame||
							window.mozRequestAnimationFrame||
							window.oRequestAnimationFrame||
							window.msRequestAnimationFrame;

//To stop animation
var cancelAnimationFrage = 
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
	"precision highp float;"+
	"in vec4 vPosition;" +
	"in vec3 vNormal;" +
	"uniform mat4 u_model_view_matrix;" +
	"uniform mat4 u_projection_matrix;" +
	"uniform mediump int u_lkeypressed;" +
	"uniform vec3 u_Ld;" +
	"uniform vec3 u_Kd;" +
	"uniform vec4 u_light_position;" +
	"out vec3 diffuse_light;" +
	"void main(void)" +
	"{" +
	"if(u_lkeypressed == 1)"+
	"{" +
	"vec4 eyeCoordinates = u_model_view_matrix * vPosition;" +
	"vec3 tnorm = normalize(mat3(u_model_view_matrix)* vNormal);" +
	"vec3 s = normalize(vec3(u_light_position - eyeCoordinates));" +
	"diffuse_light = u_Ld * u_Kd * max(dot(s, tnorm),0.0);" +
	"}"+
	"gl_Position = u_projection_matrix * u_model_view_matrix * vPosition;" +
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
	"in vec3 diffuse_light;" +
	"out vec4 FragColor;" +
	"uniform  int u_lkeypressed;" + 
	"void main(void)" +
	"{"+
	"vec4 color;" +
	"if(u_lkeypressed == 1)" +
	"{" +
	"color = vec4(diffuse_light,1.0);" +
	"}"+
	"else" +
	"{"+
	"color = vec4(1.0, 1.0, 1.0, 1.0);"+
	"}"+
	"FragColor = color;" +
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
	modelViewUniformAta = gl.getUniformLocation(shaderProgramObject_ata, "u_model_view_matrix");
	projectionUniformAta = gl.getUniformLocation(shaderProgramObject_ata, "u_projection_matrix");
	lKeyPressedUniformAta = gl.getUniformLocation(shaderProgramObject_ata, "u_lkeypressed");
	ldUniformAta = gl.getUniformLocation(shaderProgramObject_ata, "u_Ld");
	kdUniformAta = gl.getUniformLocation(shaderProgramObject_ata, "u_Kd");
	lightPositionUniformAta = gl.getUniformLocation(shaderProgramObject_ata, "u_light_position");
	
	//Prepartion to draw Cube
	var cubeVertices_ata = new Float32Array([
									//front face
										-1.0, 1.0, 1.0, //left top
										-1.0, -1.0, 1.0,  //left bottom
										1.0, -1.0, 1.0,  //right bottom
										1.0, 1.0, 1.0, //right top
										//right face
										1.0, 1.0, 1.0,//left top
										1.0, -1.0, 1.0, //left bottom
										1.0, -1.0, -1.0, //right bottom
										1.0, 1.0, -1.0,//right top

										//back face
										1.0, 1.0, -1.0,//left top
										1.0, -1.0, -1.0,//left bottom
										-1.0, -1.0, -1.0, //right bottom
										-1.0, 1.0, -1.0, //right top

										//left face
										- 1.0, 1.0, -1.0,
										-1.0, -1.0, -1.0,
										-1.0, -1.0, 1.0,
										-1.0, 1.0, 1.0,

										//top face
										-1.0, 1.0, -1.0,
										-1.0, 1.0, 1.0,
										1.0, 1.0, 1.0,
										1.0, 1.0, -1.0,

										//bottom face
										-1.0, -1.0, -1.0,
										-1.0, -1.0, 1.0,
										1.0, -1.0, 1.0,
										1.0, -1.0, -1.0
											]);
											
	vao_cube_ata=gl.createVertexArray();
	gl.bindVertexArray(vao_cube_ata);
	
	//vbo for positions
	vbo_position =  gl.createBuffer();
	gl.bindBuffer(gl.ARRAY_BUFFER, vbo_position);
	gl.bufferData(gl.ARRAY_BUFFER, cubeVertices_ata, gl.STATIC_DRAW);
	gl.vertexAttribPointer(WebGLMacros.ATA_ATTRIBUTE_POSITION,
							3,
							gl.FLOAT,
							false, 0, 0);
	gl.enableVertexAttribArray(WebGLMacros.ATA_ATTRIBUTE_POSITION);
	gl.bindBuffer(gl.ARRAY_BUFFER, null);
	
	//vbo for color
	var cubeColors_ata = new Float32Array([
									0.0,0.0,1.0,
		0.0,0.0,1.0,
		0.0,0.0,1.0,
		0.0,0.0,1.0,
		
		1.0,0.0,0.0,
		1.0,0.0,0.0,
		1.0,0.0,0.0,
		1.0,0.0,0.0,

		0.0,0.0,-1.0,
		0.0,0.0,-1.0,
		0.0,0.0,-1.0,
		0.0,0.0,-1.0,

		-1.0,0.0,0.0,
		-1.0,0.0,0.0,
		-1.0,0.0,0.0,
		-1.0,0.0,0.0,

		0.0,1.0,0.0,
		0.0,1.0,0.0,
		0.0,1.0,0.0,
		0.0,1.0,0.0,

		0.0,-1.0,0.0,
		0.0,-1.0,0.0,
		0.0,-1.0,0.0,
		0.0,-1.0,0.0,
	]);
	vbo_normal =  gl.createBuffer();
	gl.bindBuffer(gl.ARRAY_BUFFER, vbo_normal);
	gl.bufferData(gl.ARRAY_BUFFER, cubeColors_ata, gl.STATIC_DRAW);
	gl.vertexAttribPointer(WebGLMacros.ATA_ATTRIBUTE_NORMAL,
							3,
							gl.FLOAT,
							false, 0, 0);
	gl.enableVertexAttribArray(WebGLMacros.ATA_ATTRIBUTE_NORMAL);
	gl.bindBuffer(gl.ARRAY_BUFFER, null);
	
	
	gl.bindVertexArray(null);//done with square vao
	
	
	gl.clearColor(0.0,0.0,0.0,1.0);
	gl.clearDepth(1.0);
	gl.enable(gl.DEPTH_TEST);
	gl.depthFunc(gl.LEQUAL);
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
	if(gbLightAta){
			gl.uniform1i(lKeyPressedUniformAta, 1);
			gl.uniform3f(ldUniformAta, 1.0, 1.0, 1.0);
			gl.uniform3f(kdUniformAta, 0.5, 0.5, 0.5);
						
			gl.uniform4fv(lightPositionUniformAta, [0.0, 0.0, 2.0, 1.0]);
	}
	else
	{
			gl.uniform1i(lKeyPressedUniformAta, 0);
	}
	
	
	//draw triangle
	var modelMatrixAta = mat4.create();
	var modelViewMatrix_ata = mat4.create();
	//var modelViewProjectionMatrix_ata =  mat4.create();
	var angleInRadian = degreeToRadian(gAngle_ata);
	
	
	
	//draw square
	mat4.translate(modelMatrixAta, modelMatrixAta, [0.0,0.0,-4.0]);
	mat4.rotateX(modelMatrixAta, modelMatrixAta,angleInRadian);
	mat4.rotateY(modelMatrixAta, modelMatrixAta,angleInRadian);
	mat4.rotateZ(modelViewMatrix_ata, modelMatrixAta,angleInRadian);
	
	
	gl.uniformMatrix4fv(modelViewUniformAta,false,modelViewMatrix_ata);
	gl.uniformMatrix4fv(projectionUniformAta,false,perspectiveProjectionMatrixAta);
	
	gl.bindVertexArray(vao_cube_ata);
	gl.drawArrays(gl.TRIANGLE_FAN, 0,4);
	gl.drawArrays(gl.TRIANGLE_FAN, 4,4);
	gl.drawArrays(gl.TRIANGLE_FAN, 8,4);
	gl.drawArrays(gl.TRIANGLE_FAN, 12,4);
	gl.drawArrays(gl.TRIANGLE_FAN, 16,4);
	gl.drawArrays(gl.TRIANGLE_FAN, 20,4);
	gl.bindVertexArray(null);	
	
	gl.useProgram(null);
	if(gbAnimateAta == true)
	{	
		update();
	}
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
				bFullscreen_ata = false;
			else
				bFullscreen_ata = true;			
		toggleFullScreen();			
			break;
		
		case 65:
			if(gbAnimateAta == true)
				gbAnimateAta = false;
			else
				gbAnimateAta = true;
			break;
		case 76:
			if(gbLightAta)
				gbLightAta = false;
			else
				gbLightAta = true;
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
	
}

function uninitialize()
{
	
	if(vao_pyramid_ata)
	{
		gl.deleteVertexArray(vao_pyramid_ata);
		vao_pyramid_ata = null;
	}
	if(vao_cube_ata)
	{
		gl.deleteVertexArray(vao_cube_ata);
		vao_cube_ata = null;
	}
	if(vbo_position)
	{
		gl.deleteBuffer(vbo_position);
		vbo_position=null;
	}
	
	if(vbo_normal)
	{
		gl.deleteBuffer(vbo_normal);
		vbo_normal=null;
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