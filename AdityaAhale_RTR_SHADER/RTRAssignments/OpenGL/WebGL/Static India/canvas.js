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

var Vao_I;
var Vao_N;
var Vao_D;
var Vao_SecondI;
var Vao_A;
var Vao_OrangeL;
var Vao_TriLine;
var Vao_Plane;
var Vao_IAF;
var Vao_Smoke;

var Vbo_Position_I;
var Vbo_Position_D;
var Vbo_Position_SecondI;
var Vbo_Position_A;
var Vbo_Position_N;
var Vbo_Position_orangeLine;
var Vbo_Position_triLine;
var Vbo_Position;

var Vbo_Color_I;
var Vbo_Color_N;
var Vbo_Color_D;
var Vbo_Color_SecondI;
var Vbo_Color_A;
var Vbo_Color_orangeLine;
var Vbo_Color_TriLine;
var Vbo_Color;
var alphaUniform;
/*
var vao_triangle_ata;
var vbo_position;
var vao_square_ata;
var vbo_color;
*/
var mvpUniform_ata;
var gAngle_ata=0.0;

var rot = 0.0, itrans = -8.0, ntrans = 7.0, iitrans = -7.0, atrans = 8.0;
//var xOrColor = 0.0f, yOrColor = 0.0f, zOrColor = 0.0f, xGrColor = 0.0f, yGrColor = 0.0f, zGrColor = 0.0f;
var xTritrans = -6.5,xFlagtrans = -6.5, Topangle = 0.0, Bottomangle = 0.0, xIAFtrans = -8.3,
xPlanetrans = -8.3, xPlane1trans = -8.3, xPlane2trans = -8.3, yPlane1trans = 2.0, yPlane2trans = -2.0;

var alphaBlend = 0.0;

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
	"FragColor = out_color;" +
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
	//alphaUniform = gl.getUniformLocation(shaderProgramObject_ata, "alpha");
	
	//Preparation to draw a pyramid
	
	var I  = new Float32Array([
	                0.0,1.0,0.0,
	                0.0,-1.0,0.0
	        ]);

    var N = new Float32Array([
                            -0.5, 1.0,0.0,
                        -0.5, -1.0,0.0,
                        -0.5, 1.0,0.0,
                        0.5, -1.0,0.0,
                        0.5, -1.0,0.0,
                        0.5, 1.0,0.0,
                        0.5,1.0,0.0
                        ]);

    var D = new Float32Array([ 
                            -0.5, 1.0,0.0,
                        -0.5, -1.0,0.0,
                        -0.7, 1.0,0.0,
                        0.3, 1.0,0.0,
                        -0.7, -1.0,0.0,
                        0.3, -1.0,0.0,
                        0.3, 1.0,0.0,
                        0.3, -1.0,0.0 
                        ]);

    var A = new Float32Array([
                            0.0, 1.0,0.0,
                            -0.5, -1.0,0.0,
	                        0.0, 1.0,0.0,
	                        0.0, 1.0,0.0,
	                        0.5, -1.0,0.0,
	                        0.0, 1.0,0.0
                            ]);


    var TriColor = new Float32Array([
                            1.0, 0.6, 0.2,
                            0.07058, 0.54, 0.0274,
                            1.0, 0.6, 0.2,
                            1.0, 0.6, 0.2,
                            0.07058, 0.54, 0.0274,
                            0.07058, 0.54, 0.0274,
                            1.0, 0.6, 0.2,
                            0.07058, 0.54, 0.0274
                        ]);

    var AColor = new Float32Array([
                            1.0, 0.6, 0.2,
                            0.07058, 0.54, 0.0274,
                            1.0, 0.6, 0.2,
                            0.07058, 0.54, 0.0274,
                            0.07058, 0.54, 0.0274,
                            1.0, 0.6, 0.2,
                            0.07058, 0.54, 0.0274
                        ]);

   var flagColor = new Float32Array([
                            1.0, 0.6, 0.2,
                            1.0, 0.6, 0.2,
                            1.0, 1.0, 1.0,
                            1.0, 1.0, 1.0,
                            0.07058, 0.54, 0.0274,
                            0.07058, 0.54, 0.0274
                        ]);

   var NColor = new Float32Array([
                            1.0, 0.6, 0.2,
                            0.07058, 0.54, 0.0274,
                            1.0, 0.6, 0.2,
                            0.07058, 0.54, 0.0274,
                            0.07058, 0.54, 0.0274,
                            1.0, 0.6, 0.2
                        ]);


   var triVertices = new Float32Array([
                            -0.2, 0.2,0.0,
                             0.2, 0.2,0.0,
                            -0.2, 0.15,0.0,
                             0.2, 0.15,0.0,
                            -0.2, 0.1,0.0,
                             0.2, 0.1,0.0
   ]);

   

    //I
    Vao_I = gl.createVertexArray();
    gl.bindVertexArray(Vao_I);
    //vbo for positions
    Vbo_Position_I = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, Vbo_Position_I);
    gl.bufferData(gl.ARRAY_BUFFER, I, gl.STATIC_DRAW);
    gl.vertexAttribPointer(WebGLMacros.ATA_ATTRIBUTE_POSITION,
							3,
							gl.FLOAT,
							false, 0, 0);
    gl.enableVertexAttribArray(WebGLMacros.ATA_ATTRIBUTE_POSITION);
    gl.bindBuffer(gl.ARRAY_BUFFER, null);

    //vbo for color
    Vbo_Color_I = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, Vbo_Color_I);
    gl.bufferData(gl.ARRAY_BUFFER, TriColor, gl.STATIC_DRAW);
    gl.vertexAttribPointer(WebGLMacros.ATA_ATTRIBUTE_COLOR,
							3,
							gl.FLOAT,
							false, 0, 0);
    gl.enableVertexAttribArray(WebGLMacros.ATA_ATTRIBUTE_COLOR);
    gl.bindBuffer(gl.ARRAY_BUFFER, null);

    gl.bindVertexArray(null);//done with I vao


    //N
    Vao_N = gl.createVertexArray();
    gl.bindVertexArray(Vao_N);
    //vbo for positions
    Vbo_Position_N = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, Vbo_Position_N);
    gl.bufferData(gl.ARRAY_BUFFER, N, gl.STATIC_DRAW);
    gl.vertexAttribPointer(WebGLMacros.ATA_ATTRIBUTE_POSITION,
							3,
							gl.FLOAT,
							false, 0, 0);
    gl.enableVertexAttribArray(WebGLMacros.ATA_ATTRIBUTE_POSITION);
    gl.bindBuffer(gl.ARRAY_BUFFER, null);

    //vbo for color
    Vbo_Color_N = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, Vbo_Color_N);
    gl.bufferData(gl.ARRAY_BUFFER, NColor, gl.STATIC_DRAW);
    gl.vertexAttribPointer(WebGLMacros.ATA_ATTRIBUTE_COLOR,
							3,
							gl.FLOAT,
							false, 0, 0);
    gl.enableVertexAttribArray(WebGLMacros.ATA_ATTRIBUTE_COLOR);
    gl.bindBuffer(gl.ARRAY_BUFFER, null);

    gl.bindVertexArray(null);//done with N vao


    //D
    Vao_D = gl.createVertexArray();
    gl.bindVertexArray(Vao_D);
    //vbo for positions
    Vbo_Position_D = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, Vbo_Position_D);
    gl.bufferData(gl.ARRAY_BUFFER, D, gl.STATIC_DRAW);
    gl.vertexAttribPointer(WebGLMacros.ATA_ATTRIBUTE_POSITION,
							3,
							gl.FLOAT,
							false, 0, 0);
    gl.enableVertexAttribArray(WebGLMacros.ATA_ATTRIBUTE_POSITION);
    gl.bindBuffer(gl.ARRAY_BUFFER, null);

    //vbo for color
    Vbo_Color_N = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, Vbo_Color_N);
    gl.bufferData(gl.ARRAY_BUFFER, TriColor, gl.STATIC_DRAW);
    gl.vertexAttribPointer(WebGLMacros.ATA_ATTRIBUTE_COLOR,
							3,
							gl.FLOAT,
							false, 0, 0);
    gl.enableVertexAttribArray(WebGLMacros.ATA_ATTRIBUTE_COLOR);
    gl.bindBuffer(gl.ARRAY_BUFFER, null);

    gl.bindVertexArray(null);//done with N vao


    //A
    Vao_A = gl.createVertexArray();
    gl.bindVertexArray(Vao_A);
    //vbo for positions
    Vbo_Position_A = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, Vbo_Position_A);
    gl.bufferData(gl.ARRAY_BUFFER, A, gl.STATIC_DRAW);
    gl.vertexAttribPointer(WebGLMacros.ATA_ATTRIBUTE_POSITION,
							3,
							gl.FLOAT,
							false, 0, 0);
    gl.enableVertexAttribArray(WebGLMacros.ATA_ATTRIBUTE_POSITION);
    gl.bindBuffer(gl.ARRAY_BUFFER, null);

    //vbo for color
    Vbo_Color_A = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, Vbo_Color_A);
    gl.bufferData(gl.ARRAY_BUFFER, AColor, gl.STATIC_DRAW);
    gl.vertexAttribPointer(WebGLMacros.ATA_ATTRIBUTE_COLOR,
							3,
							gl.FLOAT,
							false, 0, 0);
    gl.enableVertexAttribArray(WebGLMacros.ATA_ATTRIBUTE_COLOR);
    gl.bindBuffer(gl.ARRAY_BUFFER, null);

    gl.bindVertexArray(null);//done with A vao

    //TriLine
    Vao_TriLine = gl.createVertexArray();
    gl.bindVertexArray(Vao_TriLine);
    //vbo for positions
    Vbo_Position_triLine = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, Vbo_Position_triLine);
    gl.bufferData(gl.ARRAY_BUFFER, triVertices, gl.STATIC_DRAW);
    gl.vertexAttribPointer(WebGLMacros.ATA_ATTRIBUTE_POSITION,
							3,
							gl.FLOAT,
							false, 0, 0);
    gl.enableVertexAttribArray(WebGLMacros.ATA_ATTRIBUTE_POSITION);
    gl.bindBuffer(gl.ARRAY_BUFFER, null);

    //vbo for color
    Vbo_Color_TriLine = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, Vbo_Color_TriLine);
    gl.bufferData(gl.ARRAY_BUFFER, flagColor, gl.STATIC_DRAW);
    gl.vertexAttribPointer(WebGLMacros.ATA_ATTRIBUTE_COLOR,
							3,
							gl.FLOAT,
							false, 0, 0);
    gl.enableVertexAttribArray(WebGLMacros.ATA_ATTRIBUTE_COLOR);
    gl.bindBuffer(gl.ARRAY_BUFFER, null);

    gl.bindVertexArray(null);//done with tri-line vao


	gl.clearColor(0.0,0.0,0.0,1.0);
	gl.clearDepth(1.0);

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
	var modelViewMatrix_ata = mat4.create();
	var modelViewProjectionMatrix_ata = mat4.create();
	var angleInRadian = degreeToRadian(gAngle_ata);


    //draw triline
	gl.lineWidth(5);
	mat4.translate(modelViewMatrix_ata, modelViewMatrix_ata, [2.0, 0.0, -6.0]);

	mat4.multiply(modelViewProjectionMatrix_ata, perspectiveProjectionMatrix_ata, modelViewMatrix_ata);

	gl.uniformMatrix4fv(mvpUniform_ata, false, modelViewProjectionMatrix_ata);
	gl.uniform1f(alphaUniform, 1.0);
	gl.bindVertexArray(Vao_TriLine);
	gl.drawArrays(gl.LINES, 0, 6);
	gl.bindVertexArray(null);


	//draw I
	modelViewMatrix_ata = mat4.identity(modelViewMatrix_ata);
	modelViewProjectionMatrix_ata = mat4.identity(modelViewProjectionMatrix_ata);
	
	mat4.translate(modelViewMatrix_ata, modelViewMatrix_ata, [-2.5, 0.0, -6.0]);
	
	mat4.multiply(modelViewProjectionMatrix_ata, perspectiveProjectionMatrix_ata, modelViewMatrix_ata);
	
	gl.uniformMatrix4fv(mvpUniform_ata, false, modelViewProjectionMatrix_ata);
	gl.uniform1f(alphaUniform, 1.0);
	gl.bindVertexArray(Vao_I);
	gl.drawArrays(gl.LINES, 0,2);
	gl.bindVertexArray(null);
	
    //draw N
	modelViewMatrix_ata = mat4.identity(modelViewMatrix_ata);
	modelViewProjectionMatrix_ata = mat4.identity(modelViewProjectionMatrix_ata);

	mat4.translate(modelViewMatrix_ata, modelViewMatrix_ata, [-1.5, 0.0, -6.0]);

	mat4.multiply(modelViewProjectionMatrix_ata, perspectiveProjectionMatrix_ata, modelViewMatrix_ata);

	gl.uniformMatrix4fv(mvpUniform_ata, false, modelViewProjectionMatrix_ata);
	gl.uniform1f(alphaUniform, 1.0);
	gl.bindVertexArray(Vao_N);
	gl.drawArrays(gl.LINE_STRIP, 0, 6);
	gl.bindVertexArray(null);


    //draw D
	modelViewMatrix_ata = mat4.identity(modelViewMatrix_ata);
	modelViewProjectionMatrix_ata = mat4.identity(modelViewProjectionMatrix_ata);

	mat4.translate(modelViewMatrix_ata, modelViewMatrix_ata, [0.2, 0.0, -6.0]);

	mat4.multiply(modelViewProjectionMatrix_ata, perspectiveProjectionMatrix_ata, modelViewMatrix_ata);

	gl.uniformMatrix4fv(mvpUniform_ata, false, modelViewProjectionMatrix_ata);
	gl.uniform1f(alphaUniform, alphaBlend);
	gl.bindVertexArray(Vao_D);
	gl.drawArrays(gl.LINES, 0, 8);
	gl.bindVertexArray(null);


    //draw Second I
	var modelViewMatrix_ata = mat4.create();
	var modelViewProjectionMatrix_ata = mat4.create();
	var angleInRadian = degreeToRadian(gAngle_ata);

	mat4.translate(modelViewMatrix_ata, modelViewMatrix_ata, [1.0, 0.0, -6.0]);

	mat4.multiply(modelViewProjectionMatrix_ata, perspectiveProjectionMatrix_ata, modelViewMatrix_ata);

	gl.uniformMatrix4fv(mvpUniform_ata, false, modelViewProjectionMatrix_ata);
	gl.uniform1f(alphaUniform, 1.0);
	gl.bindVertexArray(Vao_I);
	gl.drawArrays(gl.LINES, 0, 2);
	gl.bindVertexArray(null);


    //draw A
	modelViewMatrix_ata = mat4.identity(modelViewMatrix_ata);
	modelViewProjectionMatrix_ata = mat4.identity(modelViewProjectionMatrix_ata);

	mat4.translate(modelViewMatrix_ata, modelViewMatrix_ata, [2.0, 0.0, -6.0]);

	mat4.multiply(modelViewProjectionMatrix_ata, perspectiveProjectionMatrix_ata, modelViewMatrix_ata);

	gl.uniformMatrix4fv(mvpUniform_ata, false, modelViewProjectionMatrix_ata);
	gl.uniform1f(alphaUniform, 1.0);
	gl.bindVertexArray(Vao_A);
	gl.drawArrays(gl.LINES, 0, 6);
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
    if (itrans <= -2.5)
    {
        //itrans = itrans + 0.08;
        itrans = itrans + 0.005;
        //ytrans = ytrans + 0.001;
    }

    if (itrans >= -2.5)
    {
        if (atrans > 2.0)
        {
            //atrans = atrans - 0.08;
            atrans = atrans - 0.005;
            //yctrans = yctrans + 0.001;

        }
    }

    if (atrans < 2.0)
    {
        if (ntrans >= 0.0)
        {
            //ntrans = ntrans - 0.08;
            ntrans = ntrans - 0.005;
        }
    }

    if (ntrans < 0.0)
    {
        if (iitrans <= 0.0)
        {
            //iitrans = iitrans + 0.08;
            iitrans = iitrans + 0.005;
        }
    }

    if (iitrans > 0.0)
    {
        alphaBlend = alphaBlend + 0.01;
    }

    if (alphaBlend > 1.0)
    {

        if (xTritrans < 10.0)
        {
            xTritrans = xTritrans + 0.007;

        }
        xPlanetrans = xPlanetrans + 0.009;
        xPlane1trans = xPlane1trans + 0.009;
        xPlane2trans = xPlane2trans + 0.009;
        xIAFtrans = xIAFtrans + 0.009;
        //Topangle = Topangle + 0.008;
        //Bottomangle = Bottomangle - 0.008;

        if (yPlane1trans > 0.078)
        {
            yPlane1trans = yPlane1trans - 0.004;

        }
        if (yPlane2trans < 0.078)
        {
            yPlane2trans = yPlane2trans + 0.004;
        }

        if (xPlanetrans > 3.0)
        {
            xPlane1trans = xPlane1trans + 0.0008;
            yPlane1trans = yPlane1trans - 0.006;//0.007

            Topangle = Topangle - 0.08;//0.08
            if (Topangle < -45.0)
            {
                Topangle = -45.0;
            }
        }

        if (xPlanetrans > 3.0)
        {
            xPlane2trans = xPlane2trans + 0.0008;
            yPlane2trans = yPlane2trans + 0.006;

            Bottomangle = Bottomangle + 0.08;
            if (Bottomangle > 45.0)
            {
                Bottomangle = 45.0;
            }
        }

        if (xTritrans > 1.5)
        {
            xFlagtrans = 2.0;
        }

        //}
        /*if (Bottomangle < 0.0)
		{

		}*/
    }    

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