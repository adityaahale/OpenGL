//global variables
var canvas=null;
var gl=null; //for webgl context
var bFullscreenAta = false;
var canvas_original_widthAta;
var canvas_original_heightAta;

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
var vbo_texture;
var vao_cube_ata;
var mvpUniformAta;
var gAngleAta=0.0;

var perspectiveProjectionMatrix;

var pyramid_texture_ata = 0;
var cube_texture_ata = 0;
var uniform_texture0_sampler_ata;

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
	//get canvas elementFromPoint
	canvas = document.getElementById("AMC");
	if(!canvas)
		console.log("Obtaining canvas from main document failed\n");
	else
		console.log("Obtaining canvas from main document succeeded\n");
	//print obtained canvas width and height on console
	console.log("Canvas width:" + canvas.width +" height:" +canvas.height);
	canvas_original_widthAta = canvas.width;
	canvas_original_heightAta = canvas.height;
	
	
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
	gl=canvas.getContext("webgl2");
	if(gl == null)
		console.log("Obtaining 2D webgl2 failed\n");
	else
		console.log("Obtaining 2D webgl2 succeeded\n");
	
	gl.viewportWidth  = canvas.width;
	gl.viewportHeight  = canvas.height;
	
	//vertex 
	var vertexShaderSourceCodeAta =
	"#version 300 es"+
	"\n" +
	"in vec4 vPosition;" +
	"in vec2 vTexture0_Coord;" +
	"out vec2 out_texture0_coord;" +
	"uniform mat4 u_mvp_matrix;"+
	"void main(void)"+
	"{"+
	"gl_Position = u_mvp_matrix * vPosition;"+
	"out_texture0_coord = vTexture0_Coord;" +
	"}";
	vertexShaderObject_ata=gl.createShader(gl.VERTEX_SHADER);
	gl.shaderSource(vertexShaderObject_ata, vertexShaderSourceCodeAta);
	gl.compileShader(vertexShaderObject_ata);
	if(gl.getShaderParameter(vertexShaderObject_ata,gl.COMPILE_STATUS) == false)
	{
		var error=gl.getShaderInfoLog(vertexShaderObject_ata);
		if(error.length > 0)
		{
			alert(error);
			uninitialize();
		}
	}
	
	//fragmentShader
	var fragmentShaderSourceAta =
	"#version 300 es"+
	"\n"+
	"precision highp float;"+
	"in vec2 out_texture0_coord;" +
	"uniform highp sampler2D u_texture0_sampler;"+
	"out vec4 FragColor;"+
	"void main(void)"+
	"{"+
	"FragColor = texture(u_texture0_sampler, out_texture0_coord);"+
	"}";
	fragmentShaderObject_ata = gl.createShader(gl.FRAGMENT_SHADER);
	gl.shaderSource(fragmentShaderObject_ata,fragmentShaderSourceAta);
	gl.compileShader(fragmentShaderObject_ata);
	if(gl.getShaderParameter(fragmentShaderObject_ata,gl.COMPILE_STATUS) == false)
	{
		var error=gl.getShaderInfoLog(fragmentShaderObject_ata);
		if(error.length > 0)
		{
			alert(error);
			uninitialize();
		}
	}
	//shader program
	shaderProgramObject_ata=gl.createProgram();
	gl.attachShader(shaderProgramObject_ata, vertexShaderObject_ata);
	gl.attachShader(shaderProgramObject_ata, fragmentShaderObject_ata);
	
	//pre-link binidng of shader program object with vertex shader attributes
	gl.bindAttribLocation(shaderProgramObject_ata, WebGLMacros.ATA_ATTRIBUTE_POSITION, "vPosition");
	gl.bindAttribLocation(shaderProgramObject_ata, WebGLMacros.ATA_ATTRIBUTE_TEXTURE0, "vTexture0_Coord");
	//linking
	gl.linkProgram(shaderProgramObject_ata);
	if(!gl.getProgramParameter(shaderProgramObject_ata, gl.LINK_STATUS))
	{
		var error = gl.getProgramInfoLog(shaderProgramObject_ata);
		if(error.length > 0)
		{
			alert(error);
			uninitialize();
		}
	}
	
	//Load smiley texture
	cube_texture_ata = gl.createTexture();
	cube_texture_ata.image = new Image();
	cube_texture_ata.image.src = "Smiley.png";
	cube_texture_ata.image.onload = function ()
	{
		gl.bindTexture(gl.TEXTURE_2D, cube_texture_ata);
		gl.pixelStorei(gl.UNPACK_FLIP_Y_WEBGL, true);
		gl.texImage2D(gl.TEXTURE_2D, 0 , gl.RGBA, gl.RGBA, gl.UNSIGNED_BYTE, cube_texture_ata.image);
		gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);
		gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);
		gl.bindTexture(gl.TEXTURE_2D, null);
	}
	//get MVP uniform
	mvpUniformAta = gl.getUniformLocation(shaderProgramObject_ata, "u_mvp_matrix");
	uniform_texture0_sampler_ata =  gl.getUniformLocation(shaderProgramObject_ata, "u_texture0_sampler");
	//Preparation to draw a pyramid
		
	var cubeVertices = new Float32Array([
										-1.0, 1.0, 0.0,
										-1.0, -1.0, 0.0,
										1.0, -1.0, 0.0,
										1.0, 1.0, 0.0	
									]);
									
	var cubeTexcoords = new Float32Array([
									0.0, 1.0,
										0.0, 0.0,
										1.0, 0.0,
										1.0, 1.0
									]);
	vao_cube_ata=gl.createVertexArray();
	gl.bindVertexArray(vao_cube_ata);
	//vbo for positions
	vbo_position =  gl.createBuffer();
	gl.bindBuffer(gl.ARRAY_BUFFER, vbo_position);
	gl.bufferData(gl.ARRAY_BUFFER, cubeVertices, gl.STATIC_DRAW);
	gl.vertexAttribPointer(WebGLMacros.ATA_ATTRIBUTE_POSITION,
							3,
							gl.FLOAT,
							false, 0, 0);
	gl.enableVertexAttribArray(WebGLMacros.ATA_ATTRIBUTE_POSITION);	

	gl.bindBuffer(gl.ARRAY_BUFFER, null);

	
	vbo_texture =  gl.createBuffer();
	gl.bindBuffer(gl.ARRAY_BUFFER, vbo_texture);
	gl.bufferData(gl.ARRAY_BUFFER, cubeTexcoords, gl.STATIC_DRAW);
	gl.vertexAttribPointer(WebGLMacros.ATA_ATTRIBUTE_TEXTURE0,
							2,
							gl.FLOAT,
							false, 0, 0);
	gl.enableVertexAttribArray(WebGLMacros.ATA_ATTRIBUTE_TEXTURE0);
	gl.bindBuffer(gl.ARRAY_BUFFER, null);
	gl.bindVertexArray(null);//done with pyramid vao
	
	gl.clearColor(0.0,0.0,0.0,1.0);
	gl.clearDepth(1.0);
	gl.enable(gl.DEPTH_TEST);
	gl.depthFunc(gl.LEQUAL);
	perspectiveProjectionMatrix =  mat4.create();
}

function resize()
{
	if(bFullscreenAta == true)
	{
		canvas.width = window.innerWidth;
		canvas.height = window.innerHeight;		
	}else
	{
		canvas.width = canvas_original_widthAta;
		canvas.height = canvas_original_heightAta;
	}
	
	mat4.perspective(perspectiveProjectionMatrix, 45.0, parseFloat(canvas.width/canvas.height),0.1, 100.0);		
	
	gl.viewport(0,0,canvas.width,canvas.height);
}
function draw()
{
	gl.clear(gl.COLOR_BUFFER_BIT|gl.DEPTH_BUFFER_BIT);
	gl.useProgram(shaderProgramObject_ata);
	
	var modelViewMatrix = mat4.create();
	var modelViewProjectionMatrix =  mat4.create();
	var angleInRadian = degreeToRadian(gAngleAta);
		
	//draw cube
	modelViewMatrix =  mat4.identity(modelViewMatrix);
	modelViewProjectionMatrix =  mat4.identity(modelViewProjectionMatrix);
	mat4.translate(modelViewMatrix, modelViewMatrix, [0.0,0.0,-5.0]);
	
	mat4.multiply(modelViewProjectionMatrix, perspectiveProjectionMatrix, modelViewMatrix);
	gl.bindTexture(gl.TEXTURE_2D, cube_texture_ata);
	gl.uniform1i(uniform_texture0_sampler_ata, 0);
	gl.uniformMatrix4fv(mvpUniformAta,false,modelViewProjectionMatrix);
	gl.bindVertexArray(vao_cube_ata);
	gl.drawArrays(gl.TRIANGLE_FAN, 0,4);
	
	gl.bindVertexArray(null);
		
	gl.useProgram(null);
	update();
	requestAnimationFrame(draw,canvas);
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
		if(canvas.requestFullscreen)
			canvas.requestFullscreen();
		else if(canvas.mozRequestFullScreen)
			canvas.mozRequestFullScreen();
		else if(canvas.webkitRequestFullscreen)
			canvas.webkitRequestFullscreen();
		else if(canvas.msRequestFullscreen)
			canvas.msRequestFullscreen();
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
			if(bFullscreenAta == true)
				bFullscreenAta = false;
			else
				bFullscreenAta = true;			
		toggleFullScreen();			
			break;
	}
}
function update()
{
		if( gAngleAta >= 360.0)
			gAngleAta = 0.0;
		else
			gAngleAta = gAngleAta + 1.0;
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
	if(pyramid_texture_ata)
	{
		gl.deleteTexture(pyramid_texture_ata);
		pyramid_texture_ata = 0;
	}
	if(cube_texture_ata)
	{
		gl.deleteTexture(cube_texture_ata);
		cube_texture_ata = 0;
	}
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
	
	if(vbo_texture)
	{
		gl.deleteBuffer(vbo_texture);
		vbo_texture=null;
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