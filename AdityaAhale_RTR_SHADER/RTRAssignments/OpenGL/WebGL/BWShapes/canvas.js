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

var vao_triangle_ata;
var vbo_position;
var vao_square_ata;
var mvpUniformAta;
var gAngleAta=0.0;

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
	
	//vertex shaderProgramObject_ata
	var vertexShaderSourceCodeAta =
	"#version 300 es"+
	"\n" +
	"in vec4 vPosition;" +
	"uniform mat4 u_mvp_matrix;"+
	"void main(void)"+
	"{"+
	"gl_Position = u_mvp_matrix * vPosition;"+
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
	"out vec4 FragColor;"+
	"void main(void)"+
	"{"+
	"FragColor = vec4(1.0,1.0,1.0,1.0);"+
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
	mvpUniformAta = gl.getUniformLocation(shaderProgramObject_ata, "u_mvp_matrix");
	
	//Preparation to draw a pyramid
	
	var triangleVerticesAta = new Float32Array([
										0.0, 1.0, 0.0, //apex of the triangle
										-1.0, -1.0, 0.0, //left-bottom
										1.0, -1.0, 0.0, //right-bottom													
											]);
											
	vao_triangle_ata=gl.createVertexArray();
	gl.bindVertexArray(vao_triangle_ata);
	//vbo for positions
	vbo_position =  gl.createBuffer();
	gl.bindBuffer(gl.ARRAY_BUFFER, vbo_position);
	gl.bufferData(gl.ARRAY_BUFFER, triangleVerticesAta, gl.STATIC_DRAW);
	gl.vertexAttribPointer(WebGLMacros.ATA_ATTRIBUTE_POSITION,
							3,
							gl.FLOAT,
							false, 0, 0);
	gl.enableVertexAttribArray(WebGLMacros.ATA_ATTRIBUTE_POSITION);
	gl.bindBuffer(gl.ARRAY_BUFFER, null);
	
	gl.bindVertexArray(null);//done with triangle vao
	
	var squareVerticesAta = new Float32Array([
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
	gl.bufferData(gl.ARRAY_BUFFER, squareVerticesAta, gl.STATIC_DRAW);
	gl.vertexAttribPointer(WebGLMacros.ATA_ATTRIBUTE_POSITION,
							3,
							gl.FLOAT,
							false, 0, 0);
	gl.enableVertexAttribArray(WebGLMacros.ATA_ATTRIBUTE_POSITION);
	gl.bindBuffer(gl.ARRAY_BUFFER, null);
	
	gl.bindVertexArray(null);//done with square vao
	
	
	gl.clearColor(0.0,0.0,0.0,1.0);
	gl.clearDepth(1.0);
	gl.enable(gl.DEPTH_TEST);
	gl.depthFunc(gl.LEQUAL);
	perspectiveProjectionMatrix_ata =  mat4.create();
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
	
	mat4.perspective(perspectiveProjectionMatrix_ata, 45.0, parseFloat(canvas.width/canvas.height),0.1, 100.0);		
	
	gl.viewport(0,0,canvas.width,canvas.height);
}
function draw()
{
	gl.clear(gl.COLOR_BUFFER_BIT|gl.DEPTH_BUFFER_BIT);
	gl.useProgram(shaderProgramObject_ata);
	
	//draw triangle
	var modelViewMatrixAta = mat4.create();
	var modelViewProjectionMatrixAta =  mat4.create();
	var angleInRadianAta = degreeToRadian(gAngleAta);
	mat4.translate(modelViewMatrixAta, modelViewMatrixAta, [-1.5,0.0,-4.0]);
	//mat4.rotateY(modelViewMatrixAta, modelViewMatrixAta,angleInRadianAta);
	mat4.multiply(modelViewProjectionMatrixAta, perspectiveProjectionMatrix_ata, modelViewMatrixAta);
	
	gl.uniformMatrix4fv(mvpUniformAta,false,modelViewProjectionMatrixAta);
	gl.bindVertexArray(vao_triangle_ata);
	gl.drawArrays(gl.TRIANGLES, 0,3);
	gl.bindVertexArray(null);
	
	//draw square
	modelViewMatrixAta =  mat4.identity(modelViewMatrixAta);
	modelViewProjectionMatrixAta =  mat4.identity(modelViewProjectionMatrixAta);
	mat4.translate(modelViewMatrixAta, modelViewMatrixAta, [1.5,0.0,-4.0]);
	mat4.multiply(modelViewProjectionMatrixAta, perspectiveProjectionMatrix_ata, modelViewMatrixAta);
	
	gl.uniformMatrix4fv(mvpUniformAta,false,modelViewProjectionMatrixAta);
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