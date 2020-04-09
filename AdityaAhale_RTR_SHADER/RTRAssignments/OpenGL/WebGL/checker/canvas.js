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

var vao_quad;
var vbo_position;
var vbo_texture;

var mvpUniform_ata;
var gAngle_ata=0.0;

var samplerUniformAta;

var perspectiveProjectionMatrix_ata;

var checkImageHeight = 64;
var checkImageWidth = 64;
var checkerBoardTexture=new  Uint8Array(checkImageHeight*checkImageWidth*4);
var checkTex = 0;

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
		console.log("Obtaining canvas failed\n");
	else
		console.log("Obtaining canvas succeeded\n");
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
		console.log("Obtaining webgl2 failed\n");
	else
		console.log("Obtaining webgl2 succeeded\n");
	
	gl.viewportWidth  = canvas.width;
	gl.viewportHeight  = canvas.height;
	
	//vertexShader
	var vertexShaderSourceCode_ata =
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
	"in vec2 out_texture0_coord;" +
	"uniform highp sampler2D u_texture0_sampler;"+
	"out vec4 FragColor;"+
	"void main(void)"+
	"{"+
	"FragColor = texture(u_texture0_sampler, out_texture0_coord);"+
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
	
	LoadGLTextures();
	//get MVP uniform
	mvpUniform_ata = gl.getUniformLocation(shaderProgramObject_ata, "u_mvp_matrix");
	samplerUniformAta = gl.getUniformLocation(shaderProgramObject_ata, "u_texture0_sampler");
	
	//Preparation to draw a pyramid

    //Vertices and Texture Preparation
	
    vao_quad = gl.createVertexArray();
    gl.bindVertexArray(vao_quad);
    //vbo for positions
    vbo_position = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, vbo_position);
    gl.bufferData(gl.ARRAY_BUFFER, 32, gl.DYNAMIC_DRAW);
    gl.vertexAttribPointer(WebGLMacros.ATA_ATTRIBUTE_POSITION,
							3,
							gl.FLOAT,
							false, 0, 0);
    gl.enableVertexAttribArray(WebGLMacros.ATA_ATTRIBUTE_POSITION);
    gl.bindBuffer(gl.ARRAY_BUFFER, null);

	var checkerTexCoords = new Float32Array([
			0.0, 0.0,
			0.0, 1.0,
			1.0, 1.0,
			1.0, 0.0 
	]);
	
    //vbo for texture
    vbo_texture = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, vbo_texture);
    gl.bufferData(gl.ARRAY_BUFFER, checkerTexCoords, gl.STATIC_DRAW);
    gl.vertexAttribPointer(WebGLMacros.ATA_ATTRIBUTE_TEXTURE0,
							2,
							gl.FLOAT,
							false, 0, 0);
    gl.enableVertexAttribArray(WebGLMacros.ATA_ATTRIBUTE_TEXTURE0);
    gl.bindBuffer(gl.ARRAY_BUFFER, null);

    gl.bindVertexArray(null);//done with  vao

	gl.clearColor(0.0,0.0,0.0,1.0);
	gl.clearDepth(1.0);
	perspectiveProjectionMatrix_ata =  mat4.create();
}

function LoadGLTextures() {

	MakeCheckImage();
	
	gl.pixelStorei(gl.UNPACK_ALIGNMENT, 1);
	checkTex = gl.createTexture();
	gl.bindTexture(gl.TEXTURE_2D, checkTex);
	
	gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.REPEAT);
	gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.REPEAT);
	gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);
	gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);
	gl.texImage2D(gl.TEXTURE_2D, 0 ,gl.RGBA, 64, 64, 0,  gl.RGBA, gl.UNSIGNED_BYTE, checkerBoardTexture);

}

function MakeCheckImage() {
	var i, j, k = 0;
	
	for (i = 0; i<checkImageHeight; i++)
    {
        for (j = 0; j<checkImageWidth; j++)
        {
			
			c = (((i & 0x8) == 0) ^ ((j & 0x8) == 0)) * 255;
			checkerBoardTexture[(i*64+j)*4+0]=c;
			checkerBoardTexture[(i*64+j)*4+0+1]=c;
			checkerBoardTexture[(i*64+j)*4+0+2]=c;
			checkerBoardTexture[(i*64+j)*4+0+3]=255;

        }
    }
	
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
	var quadVertices = new Float32Array([
										-2.0, -1.0, 0.0,
										-2.0, 1.0, 0.0,
										0.0, 1.0, 0.0,
										0.0, -1.0, 0.0																
											]);
	
	var tiltedQuadVertices = new Float32Array([
										1.0,-1.0,0.0,
										1.0,1.0,0.0,
										2.41421, 1.0, -1.41421,
										2.41421, -1.0, -1.41421
	]);											
	
	gl.clear(gl.COLOR_BUFFER_BIT|gl.DEPTH_BUFFER_BIT);
	gl.useProgram(shaderProgramObject_ata);
	var modelViewMatrix_ata = mat4.create();
	var modelViewProjectionMatrix_ata = mat4.create();
	var angleInRadian = degreeToRadian(gAngle_ata);

    
	mat4.translate(modelViewMatrix_ata, modelViewMatrix_ata, [0.0, 0.0, -4.0]);

	mat4.multiply(modelViewProjectionMatrix_ata, perspectiveProjectionMatrix_ata, modelViewMatrix_ata);

	gl.uniformMatrix4fv(mvpUniform_ata, false, modelViewProjectionMatrix_ata);
	gl.activeTexture(gl.TEXTURE0);
	gl.bindTexture(gl.TEXTURE_2D, checkTex);
	
	gl.uniform1i(samplerUniformAta, 0);
	//draw normal quad
	gl.bindVertexArray(vao_quad);
	gl.bindBuffer(gl.ARRAY_BUFFER,vbo_position);
	gl.bufferData(gl.ARRAY_BUFFER, quadVertices, gl.DYNAMIC_DRAW); 
	gl.drawArrays(gl.TRIANGLE_FAN, 0,4);
	//gl.bindVertexArray(null);
	gl.bindBuffer(gl.ARRAY_BUFFER,null);
	//draw tilted quad
	//gl.bindVertexArray(vao_quad);
	gl.bindBuffer(gl.ARRAY_BUFFER,vbo_position);
	gl.bufferData(gl.ARRAY_BUFFER, tiltedQuadVertices, gl.DYNAMIC_DRAW); 
	gl.drawArrays(gl.TRIANGLE_FAN, 0,4);
	gl.bindBuffer(gl.ARRAY_BUFFER, null);
	
	gl.bindVertexArray(null);

	gl.useProgram(null);
	//update();
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
	
}

function uninitialize()
{
	
	if(checkTex)
	{
		gl.deleteTexture(checkTex);
		checkTex = 0;
	}
	if(vao_quad)
	{
		gl.deleteVertexArray(vao_quad);
		vao_quad = null;
	}
	if(vao_cube)
	{
		gl.deleteVertexArray(vao_cube);
		vao_cube = null;
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