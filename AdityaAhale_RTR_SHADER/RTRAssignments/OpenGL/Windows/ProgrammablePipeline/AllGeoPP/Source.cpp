
#include <windows.h>
#include <stdio.h> 
#include "vmath.h"

#include <gl\glew.h>

#include <gl/GL.h>

using namespace vmath;

#pragma comment(lib,"glew32.lib")
#pragma comment(lib,"opengl32.lib")

#define WIN_WIDTH 800
#define WIN_HEIGHT 600
#define PI 3.141592653


enum
{
	AMC_ATTRIBUTE_POSITION = 0,
	AMC_ATTRIBUTE_COLOR,
	AMC_ATTRIBUTE_NORMAL,
	AMC_ATTRIBUTE_TEXTURE0,
};


LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

//Global variable declarations
FILE *gpFile = NULL;

HWND ghwnd = NULL;
HDC ghdc = NULL;
HGLRC ghrc = NULL;

DWORD dwStyle;
WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };

bool gbActiveWindow = false;
bool gbEscapeKeyIsPressed = false;
bool gbFullscreen = false;

GLuint VertexShaderObject;
GLuint FragmentShaderObject;
GLuint ShaderProgramObject;

GLuint Vao_HorLine;
GLuint Vao_VerLine;
GLuint Vao_Line;
GLuint Vao_Circle;
GLuint Vao_Triangle;
GLuint Vao_Square;
GLuint Vao_OuterCircle;

GLuint Vbo_Position_HorLine;
GLuint Vbo_Position_VerLine;
GLuint Vbo_Position_Line;
GLuint Vbo_Position_Circle;
GLuint Vbo_Position_OuterCircle;
GLuint Vbo_Position_Triangle;
GLuint Vbo_Position_Square;

GLuint Vbo_Color_HorLine;
GLuint Vbo_Color_VerLine;
GLuint Vbo_Color_Line;
GLuint Vbo_Color_Circle;
GLuint Vbo_Color_OuterCircle;
GLuint Vbo_Color_Triangle;
GLuint Vbo_Color_Square;
GLuint MVPUniform;

mat4 PerspectiveProjectionMatrix;

GLfloat angleTri = 0.0f, angleSq = 0.0f;
int height, width;

float radius;			//1,-1	0,1  -1,-1 
float area;
float a, b, c, tx, ty;
float x1 = 0.0f, x2 = -0.7f, x3 = 0.7f, yone = 0.7f, y2 = -0.7f, y3 = -0.7f;
float s, summation;


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{

	void initialize(void);
	void uninitialize(void);
	void display(void);
	void update(void);

	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szClassName[] = TEXT("OpenGL");
	bool bDone = false;

	//code
	if (fopen_s(&gpFile, "Log.txt", "w") != 0)
	{
		MessageBox(NULL, TEXT("Log File Can Not Be Created\nExitting ..."), TEXT("Error"), MB_OK | MB_TOPMOST | MB_ICONSTOP);
		exit(0);
	}
	else
	{
		fprintf(gpFile, "Log File Is Successfully Opened.\n");
	}

	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.lpfnWndProc = WndProc;
	wndclass.lpszClassName = szClassName;
	wndclass.lpszMenuName = NULL;

	RegisterClassEx(&wndclass);

	hwnd = CreateWindowEx(WS_EX_APPWINDOW,
		szClassName,
		TEXT("All Geometry"),
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
		100,
		100,
		WIN_WIDTH,
		WIN_HEIGHT,
		NULL,
		NULL,
		hInstance,
		NULL);

	ghwnd = hwnd;

	ShowWindow(hwnd, iCmdShow);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);

	initialize();

	while (bDone == false)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				bDone = true;
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			update();
			display();

			if (gbActiveWindow == true)
			{
				if (gbEscapeKeyIsPressed == true)
					bDone = true;
			}
		}
	}

	uninitialize();

	return((int)msg.wParam);
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	//function prototype
	void resize(int, int);
	void ToggleFullscreen(void);
	void uninitialize(void);

	//code
	switch (iMsg)
	{
	case WM_ACTIVATE:
		if (HIWORD(wParam) == 0)
			gbActiveWindow = true;
		else
			gbActiveWindow = false;
		break;
	case WM_ERASEBKGND:
		return(0);
	case WM_SIZE:
		resize(LOWORD(lParam), HIWORD(lParam));
		break;

	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			gbEscapeKeyIsPressed = true;
			break;
		case 0x46:
			if (gbFullscreen == false)
			{
				ToggleFullscreen();
				gbFullscreen = true;
			}
			else
			{
				ToggleFullscreen();
				gbFullscreen = false;
			}
			break;

		default:
			break;
		}
		break;
	case WM_LBUTTONDOWN:
		break;
	case WM_CLOSE:
		uninitialize();
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		break;
	}
	return(DefWindowProc(hwnd, iMsg, wParam, lParam));
}

void ToggleFullscreen(void)
{
	//variable declarations
	MONITORINFO mi;

	//code
	if (gbFullscreen == false)
	{
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
		if (dwStyle & WS_OVERLAPPEDWINDOW)
		{
			mi = { sizeof(MONITORINFO) };
			if (GetWindowPlacement(ghwnd, &wpPrev) && GetMonitorInfo(MonitorFromWindow(ghwnd, MONITORINFOF_PRIMARY), &mi))
			{
				SetWindowLong(ghwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
				SetWindowPos(ghwnd, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top, SWP_NOZORDER | SWP_FRAMECHANGED);
			}
		}
		ShowCursor(FALSE);
	}

	else
	{
		//code
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);

		ShowCursor(TRUE);
	}
}

void initialize(void)
{
	//GLuint ShaderProgramObject;
	//function prototypes
	void uninitialize(void);
	void resize(int, int);

	//variable declarations
	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex;

	//code
	ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));

	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cRedBits = 8;
	pfd.cGreenBits = 8;
	pfd.cBlueBits = 8;
	pfd.cAlphaBits = 8;
	pfd.cDepthBits = 32;

	ghdc = GetDC(ghwnd);

	iPixelFormatIndex = ChoosePixelFormat(ghdc, &pfd);
	if (iPixelFormatIndex == 0)
	{
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	if (SetPixelFormat(ghdc, iPixelFormatIndex, &pfd) == false)
	{
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	ghrc = wglCreateContext(ghdc);
	if (ghrc == NULL)
	{
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	if (wglMakeCurrent(ghdc, ghrc) == false)
	{
		wglDeleteContext(ghrc);
		ghrc = NULL;
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	GLenum glew_error = glewInit();
	if (glew_error != GLEW_OK)
	{
		wglDeleteContext(ghrc);
		ghrc = NULL;
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	//VERTEX SHADER
	VertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

	const GLchar *vertexShaderSourceCode =
		"#version 430 core" \
		"\n" \
		"in vec4 vPosition;" \
		"in vec4 vColor;" \
		"uniform mat4 u_mvp_matrix;" \
		"out vec4 out_color;" \
		"void main(void)" \
		"{" \
		"gl_Position = u_mvp_matrix * vPosition;" \
		"out_color = vColor;" \
		"}";
	glShaderSource(VertexShaderObject, 1, (const GLchar **)&vertexShaderSourceCode, NULL);

	glCompileShader(VertexShaderObject);
	GLint iInfoLogLength = 0;
	GLint iShaderCompiledStatus = 0;
	char *szInfoLog = NULL;
	glGetShaderiv(VertexShaderObject, GL_COMPILE_STATUS, &iShaderCompiledStatus);
	if (iShaderCompiledStatus == GL_FALSE)
	{
		glGetShaderiv(VertexShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (char *)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(VertexShaderObject, iInfoLogLength, &written, szInfoLog);
				fprintf(gpFile, "Vertex Shader Compilation Log : %s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				exit(0);
			}
		}
	}

	//FRAGMENT SHADER
	FragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);

	const GLchar *fragmentShaderSourceCode =
		"#version 430 core" \
		"\n" \
		"in vec4 out_color;" \
		"out vec4 FragColor;" \
		"void main(void)" \
		"{" \
		"FragColor = out_color;" \
		"}";
	glShaderSource(FragmentShaderObject, 1, (const GLchar **)&fragmentShaderSourceCode, NULL);

	glCompileShader(FragmentShaderObject);
	glGetShaderiv(FragmentShaderObject, GL_COMPILE_STATUS, &iShaderCompiledStatus);
	if (iShaderCompiledStatus == GL_FALSE)
	{
		glGetShaderiv(FragmentShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (char *)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(FragmentShaderObject, iInfoLogLength, &written, szInfoLog);
				fprintf(gpFile, "Fragment Shader Compilation Log : %s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				exit(0);
			}
		}
	}

	//SHADER PROGRAM
	ShaderProgramObject = glCreateProgram();

	glAttachShader(ShaderProgramObject, VertexShaderObject);

	glAttachShader(ShaderProgramObject, FragmentShaderObject);

	glBindAttribLocation(ShaderProgramObject, AMC_ATTRIBUTE_POSITION, "vPosition");
	glBindAttribLocation(ShaderProgramObject, AMC_ATTRIBUTE_COLOR, "vColor");

	glLinkProgram(ShaderProgramObject);
	GLint iShaderProgramLinkStatus = 0;
	glGetProgramiv(ShaderProgramObject, GL_LINK_STATUS, &iShaderProgramLinkStatus);
	if (iShaderProgramLinkStatus == GL_FALSE)
	{
		glGetProgramiv(ShaderProgramObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (char *)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(ShaderProgramObject, iInfoLogLength, &written, szInfoLog);
				fprintf(gpFile, "Shader Program Link Log : %s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				exit(0);
			}
		}
	}

	MVPUniform = glGetUniformLocation(ShaderProgramObject, "u_mvp_matrix");

	GLfloat lineVertices[8000];
	int k = 0;
	for (float i = -1.0f; i < 1; i += (float)2 / 40)
	{
		//glColor3f(0.0f, 0.0f, 1.0f);
		lineVertices[k] = 1.0f;
		k++;
		lineVertices[k] = i;
		k++;
		lineVertices[k] = 0.0f;
		k++;

		lineVertices[k] = -1.0f;
		k++;
		lineVertices[k] = i;
		k++;
		lineVertices[k] = 0.0f;
		k++;

		lineVertices[k] = i;
		k++;
		lineVertices[k] = -1.0f;
		k++;
		lineVertices[k] = 0.0f;
		k++;

		lineVertices[k] = i;
		k++;
		lineVertices[k] = 1.0f;
		k++;
		lineVertices[k] = 0.0f;
		k++;
	}
	
	c = sqrtf((x2 - x1)*(x2 - x1) + (y2 - yone)*(y2 - yone)); //These are opposite lengths/sides of vertices
	a = sqrtf((x3 - x2)*(x3 - x2) + (y3 - y2)*(y3 - y2));
	b = sqrtf((x1 - x3)*(x1 - x3) + (yone - y3)*(yone - y3));
	summation = a + b + c;
	s = (float)(a + b + c) / 2.0f;
	
	area = sqrtf(s*(s - a)*(s - b)*(s - c));
	radius = area / s;


	
	GLfloat circleVertices[3000];
	//GLfloat numberOfVertices = 1000 + 1;

	GLfloat doublePi = 2.0f * PI;

	GLfloat circleVerticesX[3000];
	GLfloat circleVerticesY[3000];
	GLfloat circleVerticesZ[3000];	
	GLfloat circleOVerticesX[3000];
	GLfloat circleOVerticesY[3000];
	GLfloat circleOVerticesZ[3000];

	//circleVerticesX[0] = x;
	//circleVerticesY[0] = y;
	//circleVerticesZ[0] = z;

	for (int i = 0; i < 3000; i++)
	{
		circleVerticesX[i] = (radius * cos(i * doublePi / 1000));
		circleVerticesY[i] = (radius * sin(i * doublePi / 1000));
		circleVerticesZ[i] = 0.0;
	}

	GLfloat allCircleVertices[3000 * 3];

	for (int i = 0; i < 3000; i++)
	{
		allCircleVertices[i * 3] = circleVerticesX[i];
		allCircleVertices[(i * 3) + 1] = circleVerticesY[i];
		allCircleVertices[(i * 3) + 2] = circleVerticesZ[i];
	}


	for (int i = 0; i < 3000; i++)
	{
		circleOVerticesX[i] = (1.0 * cos(i * doublePi / 1000));
		circleOVerticesY[i] = (1.0 * sin(i * doublePi / 1000));
		circleOVerticesZ[i] = 0.0;
	}

	GLfloat allOCircleVertices[3000 * 3];

	for (int i = 0; i < 3000; i++)
	{
		allOCircleVertices[i * 3] = circleOVerticesX[i];
		allOCircleVertices[(i * 3) + 1] = circleOVerticesY[i];
		allOCircleVertices[(i * 3) + 2] = circleOVerticesZ[i];
	}


	/*const GLfloat squareVertices[] =
	{ 1.0f, 1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f
	};*/

	const GLfloat horizontalLine[] = {
		-1.0f,0.0f,0.0f,
		1.0f,0.0f,0.0f
	};

	const GLfloat verticalLine[] = {
		0.0f,1.0f,0.0f,
		0.0f,-1.0f,0.0f
	};

	const GLfloat lineColor[] = {
		0.0f,0.0f,1.0f,
		0.0f,0.0f,1.0f
	};

	const GLfloat horColor[] = {
		1.0f,0.0f,0.0f,
		1.0f,0.0f,0.0f
	};

	const GLfloat verColor[] = {
		0.0f,1.0f,0.0f,
		0.0f,1.0f,0.0f
	};

	GLfloat circleColor[9000];
	for (int j = 0; j < 3000; j++)
	{
		circleColor[j * 3] = 1.0f;
		circleColor[(j * 3)+1] = 1.0f;
		circleColor[(j * 3)+2] = 0.0f;
	}

	GLfloat triangleVertices[] = {
/*-1.0f,-1.0f,
1.0f,-1.0f,
1.0f,-1.0f,
0.0f,1.0f,
0.0f,1.0f,
-1.0f,1.0f,
-1.0f,-1.0f,
1.0f,-1.0f,
1.0f,-1.0f,
0.0f,1.0f,
0.0f,1.0f,
-1.0f,1.0f
*//*
		-1.0f,-1.0f,
		1.0f,-1.0f,
		0.0f,1.0f,*/
	0.0f,0.7f,
	-0.7f,-0.7f,
	0.7f,-0.7f
	};

	GLfloat squareVertices[] = {
		-0.7f,0.7f,0.0f,
		-0.7f,-0.7f,0.0f,
		0.7f,-0.7f,0.0f,
		0.7f,0.7f,0.0f
	};

	GLfloat triangleColor[] = {
		1.0f,1.0f,0.0f,
		1.0f,1.0f,0.0f,
		1.0f,1.0f,0.0f,
		1.0f,1.0f,0.0f,
		1.0f,1.0f,0.0f,
		1.0f,1.0f,0.0f,
		1.0f,1.0f,0.0f,
		1.0f,1.0f,0.0f,
		1.0f,1.0f,0.0f,
		1.0f,1.0f,0.0f,
		1.0f,1.0f,0.0f,
		1.0f,1.0f,0.0f,
		1.0f,1.0f,0.0f,
		1.0f,1.0f,0.0f,
		1.0f,1.0f,0.0f,
		1.0f,1.0f,0.0f,
	};

	/*const GLfloat squareColors[] = {
		0.0f,0.0f,1.0f,
		0.0f,0.0f,1.0f,
		0.0f,0.0f,1.0f,
		0.0f,0.0f,1.0f
	};*/


	//OuterCircle
	glGenVertexArrays(1, &Vao_OuterCircle);
	glBindVertexArray(Vao_OuterCircle);

	glGenBuffers(1, &Vbo_Position_OuterCircle);
	glBindBuffer(GL_ARRAY_BUFFER, Vbo_Position_OuterCircle);
	glBufferData(GL_ARRAY_BUFFER, sizeof(allOCircleVertices), allOCircleVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &Vbo_Color_OuterCircle);
	glBindBuffer(GL_ARRAY_BUFFER, Vbo_Color_OuterCircle);
	glBufferData(GL_ARRAY_BUFFER, sizeof(circleColor), circleColor, GL_STATIC_DRAW);

	glVertexAttribPointer(AMC_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	//glVertexAttrib3f(AMC_ATTRIBUTE_COLOR, 1.0f, 0.0f, 1.0f);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR);

	glBindBuffer(GL_ARRAY_BUFFER, 0);


	glBindVertexArray(0);

	
	//Circle
	glGenVertexArrays(1, &Vao_Circle);
	glBindVertexArray(Vao_Circle);

	glGenBuffers(1, &Vbo_Position_Circle);
	glBindBuffer(GL_ARRAY_BUFFER, Vbo_Position_Circle);
	glBufferData(GL_ARRAY_BUFFER, sizeof(allCircleVertices), allCircleVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &Vbo_Color_Circle);
	glBindBuffer(GL_ARRAY_BUFFER, Vbo_Color_Circle);
	glBufferData(GL_ARRAY_BUFFER, sizeof(circleColor), circleColor, GL_STATIC_DRAW);

	glVertexAttribPointer(AMC_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	//glVertexAttrib3f(AMC_ATTRIBUTE_COLOR, 1.0f, 0.0f, 1.0f);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	
	glBindVertexArray(0);


	//Triangle
	glGenVertexArrays(1, &Vao_Triangle);
	glBindVertexArray(Vao_Triangle);

	glGenBuffers(1, &Vbo_Position_Triangle);
	glBindBuffer(GL_ARRAY_BUFFER, Vbo_Position_Triangle);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 2, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &Vbo_Color_Triangle);
	glBindBuffer(GL_ARRAY_BUFFER, Vbo_Color_Triangle);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangleColor), triangleColor, GL_STATIC_DRAW);

	glVertexAttribPointer(AMC_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	//glVertexAttrib3f(AMC_ATTRIBUTE_COLOR, 1.0f, 0.0f, 1.0f);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR);

	glBindBuffer(GL_ARRAY_BUFFER, 0);


	glBindVertexArray(0);


	//For Square
	glGenVertexArrays(1, &Vao_Square);
	glBindVertexArray(Vao_Square);

	glGenBuffers(1, &Vbo_Position_Square);
	glBindBuffer(GL_ARRAY_BUFFER, Vbo_Position_Square);
	glBufferData(GL_ARRAY_BUFFER, sizeof(squareVertices), squareVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &Vbo_Color_Square);
	glBindBuffer(GL_ARRAY_BUFFER, Vbo_Color_Square);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangleColor), triangleColor, GL_STATIC_DRAW);

	glVertexAttribPointer(AMC_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	//glVertexAttrib3f(AMC_ATTRIBUTE_COLOR, 1.0f, 0.0f, 1.0f);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR);

	glBindBuffer(GL_ARRAY_BUFFER, 0);


	glBindVertexArray(0);


	//For Graph
	glGenVertexArrays(1, &Vao_Line);
	glBindVertexArray(Vao_Line);

	glGenBuffers(1, &Vbo_Position_Line);
	glBindBuffer(GL_ARRAY_BUFFER, Vbo_Position_Line);
	glBufferData(GL_ARRAY_BUFFER, sizeof(lineVertices), lineVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	/*glGenBuffers(1, &Vbo_Color_Line);
	glBindBuffer(GL_ARRAY_BUFFER, Vbo_Color_Line);
	glBufferData(GL_ARRAY_BUFFER, sizeof(lineColor), lineColor, GL_STATIC_DRAW);
*/
//glVertexAttribPointer(AMC_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glVertexAttrib3f(AMC_ATTRIBUTE_COLOR, 0.0f, 0.0f, 1.0f);
	//glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR);

//	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//Horizontal Line
	glGenVertexArrays(1, &Vao_HorLine);
	glBindVertexArray(Vao_HorLine);

	glGenBuffers(1, &Vbo_Position_HorLine);
	glBindBuffer(GL_ARRAY_BUFFER, Vbo_Position_HorLine);
	glBufferData(GL_ARRAY_BUFFER, sizeof(horizontalLine), horizontalLine, GL_STATIC_DRAW);

	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &Vbo_Color_HorLine);
	glBindBuffer(GL_ARRAY_BUFFER, Vbo_Color_HorLine);
	glBufferData(GL_ARRAY_BUFFER, sizeof(horColor), horColor, GL_STATIC_DRAW);

	glVertexAttribPointer(AMC_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	//glVertexAttrib3f(AMC_ATTRIBUTE_COLOR, 1.0f, 1.0f, 0.0f);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	//For Vertical Line
	glGenVertexArrays(1, &Vao_VerLine);
	glBindVertexArray(Vao_VerLine);

	glGenBuffers(1, &Vbo_Position_VerLine);
	glBindBuffer(GL_ARRAY_BUFFER, Vbo_Position_VerLine);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticalLine), verticalLine, GL_STATIC_DRAW);

	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &Vbo_Color_VerLine);
	glBindBuffer(GL_ARRAY_BUFFER, Vbo_Color_VerLine);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verColor), verColor, GL_STATIC_DRAW);

	glVertexAttribPointer(AMC_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	//glVertexAttrib3f(AMC_ATTRIBUTE_COLOR, 1.0f, 1.0f, 0.0f);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);



	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
	glDisable(GL_CULL_FACE);
	//glDisable(GL_FRONT_FACE);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	PerspectiveProjectionMatrix = mat4::identity();

	resize(WIN_WIDTH, WIN_HEIGHT);
}

void display(void)
{
	//GLuint ShaderProgramObject;
	//code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(ShaderProgramObject);

	mat4 modelViewMatrix;
	mat4 modelViewProjectionMatrix;
	mat4 rotationMatrix;
	mat4 translationMatrix;

	// OpenGL Drawing

	glBindVertexArray(Vao_Circle);
	modelViewMatrix = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();
	rotationMatrix = mat4::identity();
	translationMatrix = mat4::identity();
	tx = ((a*x1) + (b*x2) + (c*x3)) / summation;
	ty = ((a*yone) + (b*y2) + (c*y3)) / summation;

	translationMatrix = translate(tx, ty, -3.0f);
	modelViewMatrix = modelViewMatrix * translationMatrix;
	modelViewProjectionMatrix = PerspectiveProjectionMatrix * modelViewMatrix;

	glUniformMatrix4fv(MVPUniform, 1, GL_FALSE, modelViewProjectionMatrix);

	glDrawArrays(GL_LINE_LOOP, 0, 3000);

	glBindVertexArray(0);

	glBindVertexArray(Vao_OuterCircle);
	modelViewMatrix = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();
	rotationMatrix = mat4::identity();
	translationMatrix = mat4::identity();
	
	translationMatrix = translate(0.0f, 0.0f, -3.0f);
	modelViewMatrix = modelViewMatrix * translationMatrix;
	modelViewProjectionMatrix = PerspectiveProjectionMatrix * modelViewMatrix;

	glUniformMatrix4fv(MVPUniform, 1, GL_FALSE, modelViewProjectionMatrix);

	glDrawArrays(GL_LINE_LOOP, 0, 3000);

	glBindVertexArray(0);




	glBindVertexArray(Vao_Triangle);
	//glLineWidth(3.0f);
	modelViewMatrix = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();
	rotationMatrix = mat4::identity();
	translationMatrix = mat4::identity();


	translationMatrix = translate(0.0f, 0.0f, -3.0f);
	modelViewMatrix = modelViewMatrix * translationMatrix;
	modelViewProjectionMatrix = PerspectiveProjectionMatrix * modelViewMatrix;

	glUniformMatrix4fv(MVPUniform, 1, GL_FALSE, modelViewProjectionMatrix);

	glDrawArrays(GL_LINE_LOOP, 0, 3);
	//glDrawArrays(GL_LINE_LOOP, 3, 6);
	//glDrawArrays(GL_LINES, 0, 3);

	glBindVertexArray(0);



	glBindVertexArray(Vao_Square);
	//glLineWidth(3.0f);
	modelViewMatrix = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();
	rotationMatrix = mat4::identity();
	translationMatrix = mat4::identity();


	translationMatrix = translate(0.0f, 0.0f, -3.0f);
	modelViewMatrix = modelViewMatrix * translationMatrix;
	modelViewProjectionMatrix = PerspectiveProjectionMatrix * modelViewMatrix;

	glUniformMatrix4fv(MVPUniform, 1, GL_FALSE, modelViewProjectionMatrix);

	glDrawArrays(GL_LINE_LOOP, 0, 4);
	//glDrawArrays(GL_LINE_LOOP, 3, 6);
	//glDrawArrays(GL_LINES, 0, 3);

	glBindVertexArray(0);


	glBindVertexArray(Vao_HorLine);
	glLineWidth(3.0f);
	modelViewMatrix = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();
	rotationMatrix = mat4::identity();
	translationMatrix = mat4::identity();

	
	translationMatrix = translate(0.0f, 0.0f, -1.0f);
	modelViewMatrix = modelViewMatrix * translationMatrix;
	modelViewProjectionMatrix = PerspectiveProjectionMatrix * modelViewMatrix;

	glUniformMatrix4fv(MVPUniform, 1, GL_FALSE, modelViewProjectionMatrix);

	glDrawArrays(GL_LINES, 0, 3);

	glBindVertexArray(0);


	glBindVertexArray(Vao_VerLine);
	modelViewMatrix = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();
	rotationMatrix = mat4::identity();
	translationMatrix = mat4::identity();


	translationMatrix = translate(0.0f, 0.0f, -1.0f);
	modelViewMatrix = modelViewMatrix * translationMatrix;
	modelViewProjectionMatrix = PerspectiveProjectionMatrix * modelViewMatrix;

	glUniformMatrix4fv(MVPUniform, 1, GL_FALSE, modelViewProjectionMatrix);

	glDrawArrays(GL_LINES, 0, 3);

	glBindVertexArray(0);


	glBindVertexArray(Vao_Line);
	glLineWidth(1.0f);
	modelViewMatrix = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();
	rotationMatrix = mat4::identity();
	translationMatrix = mat4::identity();
	//MultiplyMatrices
	modelViewMatrix = translate(0.0f, 0.0f, 0.0f);
	translationMatrix = translate(0.0f, 0.0f, -1.0f);
	modelViewMatrix = modelViewMatrix * translationMatrix;
	modelViewProjectionMatrix = PerspectiveProjectionMatrix * modelViewMatrix;

	glUniformMatrix4fv(MVPUniform, 1, GL_FALSE, modelViewProjectionMatrix);



	glDrawArrays(GL_LINES, 0, 200);

	glBindVertexArray(0);

	glUseProgram(0);

	SwapBuffers(ghdc);
}

void update(void)
{
	angleTri = angleTri + 0.1f;
	if (angleTri >= 360)
	{
		angleTri = 0.0f;
	}

	angleSq = angleSq + 0.1f;
	if (angleSq >= 360)
	{
		angleSq = 0.0f;
	}
}

void resize(int width, int height)
{
	//code
	if (height == 0)
		height = 1;
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);


	PerspectiveProjectionMatrix = perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);


}

void uninitialize(void)
{
	//code
	//GLuint ShaderProgramObject;
	if (gbFullscreen == true)
	{
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);

		ShowCursor(TRUE);

	}

	if (Vao_Circle)
	{
		glDeleteVertexArrays(1, &Vao_Circle);
		Vao_Circle = 0;
	}

	if (Vao_Line)
	{
		glDeleteVertexArrays(1, &Vao_Line);
		Vao_Line = 0;
	}

	if (Vbo_Position_Circle)
	{
		glDeleteBuffers(1, &Vbo_Position_Circle);
		Vbo_Position_Circle = 0;
	}

	if (Vbo_Position_Line)
	{
		glDeleteBuffers(1, &Vbo_Position_Line);
		Vbo_Position_Line = 0;
	}

	if (Vbo_Color_Line)
	{
		glDeleteBuffers(1, &Vbo_Color_Line);
		Vbo_Color_Line = 0;
	}

	if (Vao_HorLine)
	{
		glDeleteVertexArrays(1, &Vao_HorLine);
		Vao_HorLine = 0;
	}

	if (Vbo_Position_HorLine)
	{
		glDeleteBuffers(1, &Vbo_Position_HorLine);
		Vbo_Position_HorLine = 0;
	}

	if (Vbo_Position_VerLine)
	{
		glDeleteBuffers(1, &Vbo_Position_VerLine);
		Vbo_Position_VerLine = 0;
	}

	if (Vbo_Position_Triangle)
	{
		glDeleteBuffers(1, &Vbo_Position_Triangle);
		Vbo_Position_Triangle = 0;
	}


	if (Vbo_Color_HorLine)
	{
		glDeleteBuffers(1, &Vbo_Color_HorLine);
		Vbo_Color_HorLine = 0;

	}

	if (Vbo_Color_Circle)
	{
		glDeleteBuffers(1, &Vbo_Color_Circle);
		Vbo_Color_Circle = 0;
	}

	if (Vbo_Color_VerLine)
	{
		glDeleteBuffers(1, &Vbo_Color_VerLine);
		Vbo_Color_VerLine = 0;
	}

	if (Vbo_Color_Triangle)
	{
		glDeleteBuffers(1, &Vbo_Color_Circle);
		Vbo_Color_Circle = 0;
	}

	if (Vao_Triangle)
	{
		glDeleteVertexArrays(1, &Vao_Triangle);
		Vao_Triangle = 0;
	}

	if (Vao_VerLine)
	{
		glDeleteVertexArrays(1, &Vao_VerLine);
		Vao_VerLine = 0;
	}


	if (ShaderProgramObject)
	{
		GLsizei ShaderCount;
		glUseProgram(ShaderProgramObject);
		glGetProgramiv(ShaderProgramObject, GL_ATTACHED_SHADERS, &ShaderCount);

		GLuint *pShaders = (GLuint*)malloc((sizeof(GLuint)*ShaderCount));
		if (pShaders) {
			glGetAttachedShaders(ShaderProgramObject, ShaderCount, &ShaderCount, pShaders);

			for (int ShaderNumber = 0; ShaderNumber < ShaderCount; ShaderNumber++)
			{
				glDetachShader(ShaderProgramObject, pShaders[ShaderNumber]);
				glDeleteShader(pShaders[ShaderNumber]);
				pShaders[ShaderNumber] = 0;
			}
			free(pShaders);
		}

		glDeleteProgram(ShaderProgramObject);
		ShaderProgramObject = 0;
		glUseProgram(0);
	}

	wglMakeCurrent(NULL, NULL);

	wglDeleteContext(ghrc);
	ghrc = NULL;

	ReleaseDC(ghwnd, ghdc);
	ghdc = NULL;

	if (gpFile)
	{
		fprintf(gpFile, "Log File Is Successfully Closed.\n");
		fclose(gpFile);
		gpFile = NULL;
	}
}