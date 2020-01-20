//global variables
var canvas=null;
var gl=null; //for webgl context
var bFullscreen_ata = false;
var canvas_original_width_ata;
var canvas_original_height_ata;

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

var vao_triangle_ata;
var vbo_position;
var vao_square_ata;
var vbo_color;
var mvpUniform_ata;
var gAngle_ata=0.0;

var perspectiveProjectionMatrix_ata;


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
	canvas_original_width_ata = canvas.width;
	canvas_original_height_ata = canvas.height;
	
	
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
	
	//vertexShader
	var vertexShaderSourceCode_ata =
	"#version 300 es"+
	"\n" +
	"in vec4 vPosition;" +
	"in vec4 vColor;"+
	"uniform mat4 u_mvp_matrix;"+	
	"out vec4 out_color;"+
	"void main(void)"+
	"{"+
	"gl_Position = u_mvp_matrix * vPosition;"+
	"out_color = vColor;"+
	"}";
	vertexShaderObject_ata=gl.createShader(gl.VERTEX_SHADER);
	gl.shaderSource(vertexShaderObject_ata, vertexShaderSourceCode_ata);
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
	var fragmentShaderSource_ata =
	"#version 300 es"+
	"\n"+
	"precision highp float;"+
	"in vec4 out_color;" +
	"out vec4 FragColor;"+
	"void main(void)"+
	"{"+
	"FragColor = out_color;"+
	"}";
	fragmentShaderObject_ata = gl.createShader(gl.FRAGMENT_SHADER);
	gl.shaderSource(fragmentShaderObject_ata,fragmentShaderSource_ata);
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
	gl.bindAttribLocation(shaderProgramObject_ata, WebGLMacros.ATA_ATTRIBUTE_COLOR, "vColor");
	
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
	
	
	//get MVP uniform
	mvpUniform_ata = gl.getUniformLocation(shaderProgramObject_ata, "u_mvp_matrix");
	
	//Preparation to draw a pyramid
	
	var triangleVertices_ata = new Float32Array([
										0.0, 1.0, 0.0, //apex of the triangle
										-1.0, -1.0, 0.0, //left-bottom
										1.0, -1.0, 0.0, //right-bottom													
											]);
											
	vao_triangle_ata=gl.createVertexArray();
	gl.bindVertexArray(vao_triangle_ata);
	//vbo for positions
	vbo_position =  gl.createBuffer();
	gl.bindBuffer(gl.ARRAY_BUFFER, vbo_position);
	gl.bufferData(gl.ARRAY_BUFFER, triangleVertices_ata, gl.STATIC_DRAW);
	gl.vertexAttribPointer(WebGLMacros.ATA_ATTRIBUTE_POSITION,
							3,
							gl.FLOAT,
							false, 0, 0);
	gl.enableVertexAttribArray(WebGLMacros.ATA_ATTRIBUTE_POSITION);
	gl.bindBuffer(gl.ARRAY_BUFFER, null);
	
	//vbo for color
	var triangleColors_ata = new Float32Array([1.0, 0.0, 0.0,
											0.0,1.0,0.0,
											0.0, 0.0, 1.0
											]);
	vbo_color =  gl.createBuffer();
	gl.bindBuffer(gl.ARRAY_BUFFER, vbo_color);
	gl.bufferData(gl.ARRAY_BUFFER, triangleColors_ata, gl.STATIC_DRAW);
	gl.vertexAttribPointer(WebGLMacros.ATA_ATTRIBUTE_COLOR,
							3,
							gl.FLOAT,
							false, 0, 0);
	gl.enableVertexAttribArray(WebGLMacros.ATA_ATTRIBUTE_COLOR);
	gl.bindBuffer(gl.ARRAY_BUFFER, null);
	
	gl.bindVertexArray(null);//done with triangle vao
	
	var squareVertices_ata = new Float32Array([
										-1.0, 1.0, 0.0, //left-top 
										-1.0, -1.0, 0.0, //left-bottom
										1.0, -1.0, 0.0, //right-bottom
										1.0, 1.0, 0.0	//right-top
											]);
											
	vao_square_ata=gl.createVertexArray();
	gl.bindVertexArray(vao_square_ata);
	
	//vbo for positions
	vbo_position =  gl.createBuffer();
	gl.bindBuffer(gl.ARRAY_BUFFER, vbo_position);
	gl.bufferData(gl.ARRAY_BUFFER, squareVertices_ata, gl.STATIC_DRAW);
	gl.vertexAttribPointer(WebGLMacros.ATA_ATTRIBUTE_POSITION,
							3,
							gl.FLOAT,
							false, 0, 0);
	gl.enableVertexAttribArray(WebGLMacros.ATA_ATTRIBUTE_POSITION);
	gl.bindBuffer(gl.ARRAY_BUFFER, null);
	
	//vbo for color
	gl.vertexAttrib3f(WebGLMacros.ATA_ATTRIBUTE_COLOR, 0.0, 0.0, 1.0);
	
	gl.bindVertexArray(null);//done with square vao
	
	
	gl.clearColor(0.0,0.0,0.0,1.0);
	gl.clearDepth(1.0);
	gl.enable(gl.DEPTH_TEST);
	gl.depthFunc(gl.LEQUAL);
	perspectiveProjectionMatrix_ata =  mat4.create();
}

function resize()
{
	if(bFullscreen_ata == true)
	{
		canvas.width = window.innerWidth;
		canvas.height = window.innerHeight;		
	}else
	{
		canvas.width = canvas_original_width_ata;
		canvas.height = canvas_original_height_ata;
	}
	
	mat4.perspective(perspectiveProjectionMatrix_ata, 45.0, parseFloat(canvas.width/canvas.height),0.1, 100.0);		
	
	gl.viewport(0,0,canvas.width,canvas.height);
}
function draw()
{
	gl.clear(gl.COLOR_BUFFER_BIT|gl.DEPTH_BUFFER_BIT);
	gl.useProgram(shaderProgramObject_ata);
	
	//draw triangle
	var modelViewMatrix_ata = mat4.create();
	var modelViewProjectionMatrix_ata =  mat4.create();
	var angleInRadian = degreeToRadian(gAngle_ata);
	
	mat4.translate(modelViewMatrix_ata, modelViewMatrix_ata, [-1.5,0.0,-4.0]);
	mat4.rotateY(modelViewMatrix_ata, modelViewMatrix_ata,angleInRadian);
	mat4.multiply(modelViewProjectionMatrix_ata, perspectiveProjectionMatrix_ata, modelViewMatrix_ata);
	
	gl.uniformMatrix4fv(mvpUniform_ata,false,modelViewProjectionMatrix_ata);
	gl.bindVertexArray(vao_triangle_ata);
	gl.drawArrays(gl.TRIANGLES, 0,3);
	gl.bindVertexArray(null);
	
	//draw square
	modelViewMatrix_ata =  mat4.identity(modelViewMatrix_ata);
	modelViewProjectionMatrix_ata =  mat4.identity(modelViewProjectionMatrix_ata);
	mat4.translate(modelViewMatrix_ata, modelViewMatrix_ata, [1.5,0.0,-4.0]);
	mat4.rotateX(modelViewMatrix_ata, modelViewMatrix_ata,angleInRadian);
	mat4.multiply(modelViewProjectionMatrix_ata, perspectiveProjectionMatrix_ata, modelViewMatrix_ata);
	
	gl.uniformMatrix4fv(mvpUniform_ata,false,modelViewProjectionMatrix_ata);
	gl.bindVertexArray(vao_square_ata);
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
			if(bFullscreen_ata == true)
				bFullscreen_ata = false;
			else
				bFullscreen_ata = true;			
		toggleFullScreen();			
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
	
	if(vao_triangle_ata)
	{
		gl.deleteVertexArray(vao_triangle_ata);
		vao_triangle_ata = null;
	}
	if(vao_square_ata)
	{
		gl.deleteVertexArray(vao_square_ata);
		vao_square_ata = null;
	}
	if(vbo_position)
	{
		gl.deleteBuffer(vbo_position);
		vbo_position=null;
	}
	
	if(vbo_color)
	{
		gl.deleteBuffer(vbo_color);
		vbo_color=null;
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