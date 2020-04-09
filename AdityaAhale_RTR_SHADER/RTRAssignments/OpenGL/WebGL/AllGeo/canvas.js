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

var Vao_HorLine;
var Vao_VerLine;
var Vao_Line;
var Vao_Circle;
var Vao_Triangle;
var Vao_Square;
var Vao_OuterCircle;

var Vbo_Position_HorLine;
var Vbo_Position_VerLine;
var Vbo_Position_Line;
var Vbo_Position_Circle;
var Vbo_Position_OuterCircle;
var Vbo_Position_Triangle;
var Vbo_Position_Square;

var Vbo_Color_HorLine;
var Vbo_Color_VerLine;
var Vbo_Color_Line;
var Vbo_Color_Circle;
var Vbo_Color_OuterCircle;
var Vbo_Color_Triangle;
var Vbo_Color_Square;

var mvpUniform_ata;
var gAngle_ata=0.0;

var radius;			//1,-1	0,1  -1,-1 
var area;
var a, b, c, tx, ty;
var x1 = 0.0, x2 = -0.7, x3 = 0.7, yone = 0.7, y2 = -0.7, y3 = -0.7;
var s, summation;

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
	
	var lineVertices = Array(200);
	var k = 0;
	for (var i = -1.0; i < 1; i +=2/40)
	{
	    lineVertices[k] = 1.0;
	    k++;
	    lineVertices[k] = i;
	    k++;
	    lineVertices[k] = 0.0;
	    k++;

	    lineVertices[k] = -1.0;
	    k++;
	    lineVertices[k] = i;
	    k++;
	    lineVertices[k] = 0.0;
	    k++;

	    lineVertices[k] = i;
	    k++;
	    lineVertices[k] = -1.0;
	    k++;
	    lineVertices[k] = 0.0;
	    k++;

	    lineVertices[k] = i;
	    k++;
	    lineVertices[k] = 1.0;
	    k++;
	    lineVertices[k] = 0.0;
	    k++;
	}

    
	var radius;
	c = Math.sqrt((x2 - x1)*(x2 - x1) + (y2 - yone)*(y2 - yone)); //These are opposite lengths/sides of vertices
	a = Math.sqrt((x3 - x2)*(x3 - x2) + (y3 - y2)*(y3 - y2));
	b = Math.sqrt((x1 - x3) * (x1 - x3) + (yone - y3) * (yone - y3));
	summation = a + b + c;
	s = (a + b + c) / 2.0;
	
	area = Math.sqrt(s * (s - a) * (s - b) * (s - c));
	radius = area / s;

	var doublePi = 2.0 * 3.141592653;
	var circleVerticesX=new Array(3000);
	var circleVerticesY = new Array(3000);
	var circleVerticesZ = new Array(3000);
	var allCircleVertices = new Array(3000);

	var circleOVerticesX= new Array(3000);
	var circleOVerticesY=new Array(3000);
	var circleOVerticesZ=new Array(3000);
	var allOCircleVertices = new Array(3000);

	for (var i = 0; i < 3000; i++)
	{
		circleVerticesX[i] = (radius * Math.cos(i * doublePi / 1000));
		circleVerticesY[i] = (radius * Math.sin(i * doublePi / 1000));
		circleVerticesZ[i] = 0.0;

		circleOVerticesX[i] = (1.0 * Math.cos(i * doublePi / 1000));
		circleOVerticesY[i] = (1.0 * Math.sin(i * doublePi / 1000));
		circleOVerticesZ[i] = 0.0;
	}

	for (var i = 0; i < 3000; i++)
	{
		allCircleVertices[i * 3] = circleVerticesX[i];
		allCircleVertices[(i * 3) + 1] = circleVerticesY[i];
		allCircleVertices[(i * 3) + 2] = circleVerticesZ[i];

		allOCircleVertices[i * 3] = circleOVerticesX[i];
		allOCircleVertices[(i * 3) + 1] = circleOVerticesY[i];
		allOCircleVertices[(i * 3) + 2] = circleOVerticesZ[i];
	}
    

	var triangleVertices = new Float32Array([
	                0.0,0.7,
                    -0.7,-0.7,
	                0.7,-0.7
	]);

	var squareVertices = new Float32Array([
                            -0.7, 0.7, 0.0,
		                    -0.7, -0.7, 0.0,
		                    0.7, -0.7, 0.0,
		                    0.7, 0.7, 0.0
	]);

	var horizontalLine = new Float32Array([
                            -1.0,0.0,0.0,
		                     1.0,0.0,0.0
	]);

	var verticalLine = new Float32Array([
                           0.0,1.0,0.0,
		                   0.0,-1.0,0.0
	]);


	var circleColor = new Array(3000);

	for (var j = 0; j < 3000; j++)
	{
		circleColor[j * 3] = 1.0;
		circleColor[(j * 3)+1] = 1.0;
		circleColor[(j * 3)+2] = 0.0;
	}

    

    var triangleColor = new Float32Array([
                            1.0,1.0,0.0,
		                    1.0,1.0,0.0,
		                    1.0,1.0,0.0,
		                    1.0,1.0,0.0,
		                    1.0,1.0,0.0,
		                    1.0,1.0,0.0,
		                    1.0,1.0,0.0,
		                    1.0,1.0,0.0,
		                    1.0,1.0,0.0,
		                    1.0,1.0,0.0,
		                    1.0,1.0,0.0,
		                    1.0,1.0,0.0,
		                    1.0,1.0,0.0,
		                    1.0,1.0,0.0,
		                    1.0,1.0,0.0,
		                    1.0,1.0,0.0,
                            ]);


    var verColor = new Float32Array([
                            0.0,1.0,0.0,
		                    0.0,1.0,0.0
                        ]);

    var horColor = new Float32Array([
                            1.0,0.0,0.0,
		                    1.0,0.0,0.0
                        ]);



    //Graph
    Vao_Line = gl.createVertexArray();
    gl.bindVertexArray(Vao_Line);
    //vbo for positions
    Vbo_Position_Line = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, Vbo_Position_Line);
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(lineVertices), gl.STATIC_DRAW);
    gl.vertexAttribPointer(WebGLMacros.ATA_ATTRIBUTE_POSITION,
							3,
							gl.FLOAT,
							false, 0, 0);
    gl.enableVertexAttribArray(WebGLMacros.ATA_ATTRIBUTE_POSITION);
    gl.bindBuffer(gl.ARRAY_BUFFER, null);

    //vbo for color
   /* Vbo_Color_I = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, Vbo_Color_I);
    gl.bufferData(gl.ARRAY_BUFFER, TriColor, gl.STATIC_DRAW);*/
   /* gl.vertexAttribPointer(WebGLMacros.ATA_ATTRIBUTE_COLOR,
							3,
							gl.FLOAT,
							false, 0, 0);*/
    gl.vertexAttrib3f(WebGLMacros.ATA_ATTRIBUTE_COLOR, 0.0, 0.0, 1.0);
   // gl.enableVertexAttribArray(WebGLMacros.ATA_ATTRIBUTE_COLOR);
   // gl.bindBuffer(gl.ARRAY_BUFFER, null);

    gl.bindVertexArray(null);//done with I vao

    
    //Circle
    Vao_Circle = gl.createVertexArray();
    gl.bindVertexArray(Vao_Circle);
    //vbo for positions
    Vbo_Position_Circle = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, Vbo_Position_Circle);
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(allCircleVertices), gl.STATIC_DRAW);
    gl.vertexAttribPointer(WebGLMacros.ATA_ATTRIBUTE_POSITION,
							3,
							gl.FLOAT,
							false, 0, 0);
    gl.enableVertexAttribArray(WebGLMacros.ATA_ATTRIBUTE_POSITION);
    gl.bindBuffer(gl.ARRAY_BUFFER, null);
    
    //vbo for color
    Vbo_Color_Circle = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, Vbo_Color_Circle);
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(circleColor), gl.STATIC_DRAW);
    gl.vertexAttribPointer(WebGLMacros.ATA_ATTRIBUTE_COLOR,
							3,
							gl.FLOAT,
							false, 0, 0);
    gl.enableVertexAttribArray(WebGLMacros.ATA_ATTRIBUTE_COLOR);
    gl.bindBuffer(gl.ARRAY_BUFFER, null);
    
    gl.bindVertexArray(null);//done with N vao
    

    //Outer Circle
    Vao_OuterCircle = gl.createVertexArray();
    gl.bindVertexArray(Vao_OuterCircle);
    //vbo for positions
    Vbo_Position_OuterCircle = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, Vbo_Position_OuterCircle);
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(allOCircleVertices), gl.STATIC_DRAW);
    gl.vertexAttribPointer(WebGLMacros.ATA_ATTRIBUTE_POSITION,
							3,
							gl.FLOAT,
							false, 0, 0);
    gl.enableVertexAttribArray(WebGLMacros.ATA_ATTRIBUTE_POSITION);
    gl.bindBuffer(gl.ARRAY_BUFFER, null);
    
    //vbo for color
    Vbo_Color_OuterCircle = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, Vbo_Color_OuterCircle);
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(circleColor), gl.STATIC_DRAW);
    gl.vertexAttribPointer(WebGLMacros.ATA_ATTRIBUTE_COLOR,
							3,
							gl.FLOAT,
							false, 0, 0);
    gl.enableVertexAttribArray(WebGLMacros.ATA_ATTRIBUTE_COLOR);
    gl.bindBuffer(gl.ARRAY_BUFFER, null);
    
    gl.bindVertexArray(null);//done with outer circle vao
    
    
    //Triangle
    Vao_Triangle = gl.createVertexArray();
    gl.bindVertexArray(Vao_Triangle);
    //vbo for positions
    Vbo_Position_Triangle = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, Vbo_Position_Triangle);
    gl.bufferData(gl.ARRAY_BUFFER, triangleVertices, gl.STATIC_DRAW);
    gl.vertexAttribPointer(WebGLMacros.ATA_ATTRIBUTE_POSITION,
							2,
							gl.FLOAT,
							false, 0, 0);
    gl.enableVertexAttribArray(WebGLMacros.ATA_ATTRIBUTE_POSITION);
    gl.bindBuffer(gl.ARRAY_BUFFER, null);
    
    //vbo for color
    Vbo_Color_Triangle = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, Vbo_Color_Triangle);
    gl.bufferData(gl.ARRAY_BUFFER, triangleColor, gl.STATIC_DRAW);
    gl.vertexAttribPointer(WebGLMacros.ATA_ATTRIBUTE_COLOR,
							3,
							gl.FLOAT,
							false, 0, 0);
    gl.enableVertexAttribArray(WebGLMacros.ATA_ATTRIBUTE_COLOR);
    gl.bindBuffer(gl.ARRAY_BUFFER, null);
    
    gl.bindVertexArray(null);//done with triangle vao

    
    //Square
    Vao_Square = gl.createVertexArray();
    gl.bindVertexArray(Vao_Square);
    //vbo for positions
    Vbo_Position_Square = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, Vbo_Position_Square);
    gl.bufferData(gl.ARRAY_BUFFER, squareVertices, gl.STATIC_DRAW);
    gl.vertexAttribPointer(WebGLMacros.ATA_ATTRIBUTE_POSITION,
							3,
							gl.FLOAT,
							false, 0, 0);
    gl.enableVertexAttribArray(WebGLMacros.ATA_ATTRIBUTE_POSITION);
    gl.bindBuffer(gl.ARRAY_BUFFER, null);

    //vbo for color
    Vbo_Color_Square = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, Vbo_Color_Square);
    gl.bufferData(gl.ARRAY_BUFFER, triangleColor, gl.STATIC_DRAW);
    gl.vertexAttribPointer(WebGLMacros.ATA_ATTRIBUTE_COLOR,
							3,
							gl.FLOAT,
							false, 0, 0);
    gl.enableVertexAttribArray(WebGLMacros.ATA_ATTRIBUTE_COLOR);
    gl.bindBuffer(gl.ARRAY_BUFFER, null);

    gl.bindVertexArray(null);//done with square vao

    
    //horizontal line
    Vao_HorLine = gl.createVertexArray();
    gl.bindVertexArray(Vao_HorLine);
    //vbo for positions
    Vbo_Position_HorLine = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, Vbo_Position_HorLine);
    gl.bufferData(gl.ARRAY_BUFFER, horizontalLine, gl.STATIC_DRAW);
    gl.vertexAttribPointer(WebGLMacros.ATA_ATTRIBUTE_POSITION,
							3,
							gl.FLOAT,
							false, 0, 0);
    gl.enableVertexAttribArray(WebGLMacros.ATA_ATTRIBUTE_POSITION);
    gl.bindBuffer(gl.ARRAY_BUFFER, null);

    //vbo for color
    Vbo_Color_HorLine = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, Vbo_Color_HorLine);
    gl.bufferData(gl.ARRAY_BUFFER, horColor, gl.STATIC_DRAW);
    gl.vertexAttribPointer(WebGLMacros.ATA_ATTRIBUTE_COLOR,
							3,
							gl.FLOAT,
							false, 0, 0);
    gl.enableVertexAttribArray(WebGLMacros.ATA_ATTRIBUTE_COLOR);
    gl.bindBuffer(gl.ARRAY_BUFFER, null);

    gl.bindVertexArray(null);//done with horizontal vao


    //Vertical line
    Vao_VerLine = gl.createVertexArray();
    gl.bindVertexArray(Vao_VerLine);
    //vbo for positions
    Vbo_Position_VerLine = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, Vbo_Position_VerLine);
    gl.bufferData(gl.ARRAY_BUFFER, verticalLine, gl.STATIC_DRAW);
    gl.vertexAttribPointer(WebGLMacros.ATA_ATTRIBUTE_POSITION,
							3,
							gl.FLOAT,
							false, 0, 0);
    gl.enableVertexAttribArray(WebGLMacros.ATA_ATTRIBUTE_POSITION);
    gl.bindBuffer(gl.ARRAY_BUFFER, null);

    //vbo for color
    Vbo_Color_VerLine = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, Vbo_Color_VerLine);
    gl.bufferData(gl.ARRAY_BUFFER, verColor, gl.STATIC_DRAW);
    gl.vertexAttribPointer(WebGLMacros.ATA_ATTRIBUTE_COLOR,
							3,
							gl.FLOAT,
							false, 0, 0);
    gl.enableVertexAttribArray(WebGLMacros.ATA_ATTRIBUTE_COLOR);
    gl.bindBuffer(gl.ARRAY_BUFFER, null);

    gl.bindVertexArray(null);//done with vertical vao


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

    //Graph
	mat4.translate(modelViewMatrix_ata, modelViewMatrix_ata, [0.0, 0.0, -1.0]);

	mat4.multiply(modelViewProjectionMatrix_ata, perspectiveProjectionMatrix_ata, modelViewMatrix_ata);

	gl.uniformMatrix4fv(mvpUniform_ata, false, modelViewProjectionMatrix_ata);
	gl.bindVertexArray(Vao_Line);
	gl.drawArrays(gl.LINES, 0, 160);
	gl.bindVertexArray(null);


    //draw Outer circle
	modelViewMatrix_ata = mat4.identity(modelViewMatrix_ata);
	modelViewProjectionMatrix_ata = mat4.identity(modelViewProjectionMatrix_ata);

	mat4.translate(modelViewMatrix_ata, modelViewMatrix_ata, [0.0, 0.0, -3.0]);

	mat4.multiply(modelViewProjectionMatrix_ata, perspectiveProjectionMatrix_ata, modelViewMatrix_ata);

	gl.uniformMatrix4fv(mvpUniform_ata, false, modelViewProjectionMatrix_ata);

	gl.bindVertexArray(Vao_OuterCircle);
	gl.drawArrays(gl.LINE_LOOP, 0, 1000);
	gl.bindVertexArray(null);

    
	//draw Square
	modelViewMatrix_ata = mat4.identity(modelViewMatrix_ata);
	modelViewProjectionMatrix_ata = mat4.identity(modelViewProjectionMatrix_ata);
	
	mat4.translate(modelViewMatrix_ata, modelViewMatrix_ata, [0.0, 0.0, -3.0]);
	
	mat4.multiply(modelViewProjectionMatrix_ata, perspectiveProjectionMatrix_ata, modelViewMatrix_ata);
	
	gl.uniformMatrix4fv(mvpUniform_ata, false, modelViewProjectionMatrix_ata);
	gl.bindVertexArray(Vao_Square);
	gl.drawArrays(gl.LINE_LOOP, 0, 4);
	gl.bindVertexArray(null);
	

    //draw Circle
	tx = ((a * x1) + (b * x2) + (c * x3)) / summation;
	ty = ((a * yone) + (b * y2) + (c * y3)) / summation;

	modelViewMatrix_ata = mat4.identity(modelViewMatrix_ata);
	modelViewProjectionMatrix_ata = mat4.identity(modelViewProjectionMatrix_ata);

	mat4.translate(modelViewMatrix_ata, modelViewMatrix_ata, [tx, ty, -3.0]);

	mat4.multiply(modelViewProjectionMatrix_ata, perspectiveProjectionMatrix_ata, modelViewMatrix_ata);

	gl.uniformMatrix4fv(mvpUniform_ata, false, modelViewProjectionMatrix_ata);
	gl.bindVertexArray(Vao_Circle);
	gl.drawArrays(gl.LINE_LOOP, 0, 1000);
	gl.bindVertexArray(null);

    
    //draw Triangle
	modelViewMatrix_ata = mat4.identity(modelViewMatrix_ata);
	modelViewProjectionMatrix_ata = mat4.identity(modelViewProjectionMatrix_ata);

	mat4.translate(modelViewMatrix_ata, modelViewMatrix_ata, [0.0, 0.0, -3.0]);

	mat4.multiply(modelViewProjectionMatrix_ata, perspectiveProjectionMatrix_ata, modelViewMatrix_ata);

	gl.uniformMatrix4fv(mvpUniform_ata, false, modelViewProjectionMatrix_ata);
	gl.bindVertexArray(Vao_Triangle);
	gl.drawArrays(gl.LINE_LOOP, 0, 3);
	gl.bindVertexArray(null);

    
    //draw horizontal line
	var modelViewMatrix_ata = mat4.create();
	var modelViewProjectionMatrix_ata = mat4.create();
	var angleInRadian = degreeToRadian(gAngle_ata);

	mat4.translate(modelViewMatrix_ata, modelViewMatrix_ata, [0.0, 0.0, -1.0]);

	mat4.multiply(modelViewProjectionMatrix_ata, perspectiveProjectionMatrix_ata, modelViewMatrix_ata);

	gl.uniformMatrix4fv(mvpUniform_ata, false, modelViewProjectionMatrix_ata);
	gl.bindVertexArray(Vao_HorLine);
	gl.drawArrays(gl.LINES, 0, 2);
	gl.bindVertexArray(null);


    //draw vertical line
	modelViewMatrix_ata = mat4.identity(modelViewMatrix_ata);
	modelViewProjectionMatrix_ata = mat4.identity(modelViewProjectionMatrix_ata);

	mat4.translate(modelViewMatrix_ata, modelViewMatrix_ata, [0.0, 0.0, -1.0]);

	mat4.multiply(modelViewProjectionMatrix_ata, perspectiveProjectionMatrix_ata, modelViewMatrix_ata);

	gl.uniformMatrix4fv(mvpUniform_ata, false, modelViewProjectionMatrix_ata);
	gl.bindVertexArray(Vao_VerLine);
	gl.drawArrays(gl.LINES, 0, 2);
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