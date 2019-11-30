
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

GLuint Vao_I;
GLuint Vao_N;
GLuint Vao_D;
GLuint Vao_SecondI;
GLuint Vao_A;
GLuint Vao_OrangeL;
GLuint Vao_TriLine;
GLuint Vao_Plane;
GLuint Vao_IAF;
GLuint Vao_Smoke;
//GLuint Vao_OrangeL;

GLuint Vbo_Position_I;
GLuint Vbo_Position_D;
GLuint Vbo_Position_SecondI;
GLuint Vbo_Position_A;
GLuint Vbo_Position_N;
GLuint Vbo_Position_orangeLine;
GLuint Vbo_Position_triLine;
GLuint Vbo_Position;

GLuint Vbo_Color_I;
GLuint Vbo_Color_N;
GLuint Vbo_Color_D;
GLuint Vbo_Color_SecondI;
GLuint Vbo_Color_A;
GLuint Vbo_Color_orangeLine;
GLuint Vbo_Color_TriLine;
GLuint Vbo_Color;
GLuint MVPUniform;
GLuint alphaUniform;

mat4 PerspectiveProjectionMatrix;

GLfloat angleTri = 0.0f, angleSq = 0.0f;
GLfloat rot = 0.0f, itrans = -8.0f, ntrans = 7.0f, iitrans = -7.0, atrans = 8.0;
//GLfloat xOrColor = 0.0f, yOrColor = 0.0f, zOrColor = 0.0f, xGrColor = 0.0f, yGrColor = 0.0f, zGrColor = 0.0f;
GLfloat xTritrans = -6.5f,xFlagtrans = -6.5f, Topangle = 0.0f, Bottomangle = 0.0f, xIAFtrans = -8.3f,
xPlanetrans = -8.3f, xPlane1trans = -8.3f, xPlane2trans = -8.3f, yPlane1trans = 2.0, yPlane2trans = -2.0;

GLfloat alphaBlend = 0.0f;
int height, width;



int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{

	void initialize(void);
	void uninitialize(void);
	void display(void);
	void update(void);
	void translatemy(void);

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
		TEXT("Static India"),
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
			//update();
			display();
			translatemy();
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
		"out_color = vColor ;" \
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
		"uniform float alpha; " \
		"out vec4 FragColor;" \
		"void main(void)" \
		"{" \
		"FragColor = out_color;" \
		"FragColor.a= alpha;" \
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
	alphaUniform = glGetUniformLocation(ShaderProgramObject, "alpha");

	/*const GLfloat squareVertices[] =
	{ 1.0f, 1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f
	};*/

	const GLfloat I[] = {
		0.0f,1.0f,0.0f,
		0.0f,-1.0f,0.0f
	};

	const GLfloat N[] = {
		-0.5f, 1.0f,0.0f,
	-0.5f, -1.0f,0.0f,
	-0.5f, 1.0f,0.0f,
	0.5f, -1.0f,0.0f,
	0.5f, -1.0f,0.0f,
	0.5f, 1.0f,0.0f,
	0.5f,1.0f,0.0f
	};

	const GLfloat D[] = {
		-0.5f, 1.0f,0.0f,
	-0.5f, -1.0f,0.0f,
	-0.7f, 1.0f,0.0f,
	0.3f, 1.0f,0.0f,
	-0.7f, -1.0f,0.0f,
	0.3, -1.0f,0.0f,
	0.3f, 1.0f,0.0f,
	0.3f, -1.0f,0.0f };

	const GLfloat A[] = {
	0.0f, 1.0f,0.0f,
	-0.5f, -1.0f,0.0f,
	0.0f, 1.0f,0.0f,
	0.0f, 1.0f,0.0f,
	0.5f, -1.0f,0.0f,
	0.0f, 1.0f,0.0f
	};

	const GLfloat greenLine[]{
		0.0f
	};

	const GLfloat TriColor[] = {
		1.0f, 0.6f, 0.2f,
		0.07058f, 0.54f, 0.0274f,
		1.0f, 0.6f, 0.2f,
		1.0f, 0.6f, 0.2f,
		0.07058f, 0.54f, 0.0274f,
		0.07058f, 0.54f, 0.0274f,
		1.0f, 0.6f, 0.2f,
		0.07058f, 0.54f, 0.0274f
	};

	const GLfloat AColor[] = {
			1.0f, 0.6f, 0.2f,
	0.07058f, 0.54f, 0.0274f,
	1.0f, 0.6f, 0.2f,
	0.07058f, 0.54f, 0.0274f,
	0.07058f, 0.54f, 0.0274f,
	1.0f, 0.6f, 0.2f,
	0.07058f, 0.54f, 0.0274f
	};

	const GLfloat flagColor[] = {
		1.0f, 0.6f, 0.2f,
	1.0f, 0.6f, 0.2f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	0.07058f, 0.54f, 0.0274f,
	0.07058f, 0.54f, 0.0274f

	};

	GLfloat NColor[] = {
	1.0f, 0.6f, 0.2f,
	0.07058f, 0.54f, 0.0274f,
	1.0f, 0.6f, 0.2f,
	0.07058f, 0.54f, 0.0274f,
	0.07058f, 0.54f, 0.0274f,
	1.0f, 0.6f, 0.2f
	};

	GLfloat greenVertices[] = {
		-0.7f,0.7f,0.0f,
		-0.7f,-0.7f,0.0f,
		0.7f,-0.7f,0.0f,
		0.7f,0.7f,0.0f
	};

	GLfloat orangeVertices[] = {
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

	const GLfloat triVertices[]{
	-0.2f, 0.2f,0.0f,
0.2f, 0.2f,0.0f,
-0.2f, 0.15f,0.0f,
0.2f, 0.15f,0.0f,
-0.2f, 0.1f,0.0f,
0.2f, 0.1f,0.0f
	};
	/*const GLfloat squareColors[] = {
		0.0f,0.0f,1.0f,
		0.0f,0.0f,1.0f,
		0.0f,0.0f,1.0f,
		0.0f,0.0f,1.0f
	};*/


	//White Line
	glGenVertexArrays(1, &Vao_TriLine);
	glBindVertexArray(Vao_TriLine);

	glGenBuffers(1, &Vbo_Position_triLine);
	glBindBuffer(GL_ARRAY_BUFFER, Vbo_Position_triLine);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triVertices), triVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &Vbo_Color_TriLine);
	glBindBuffer(GL_ARRAY_BUFFER, Vbo_Color_TriLine);
	glBufferData(GL_ARRAY_BUFFER, sizeof(flagColor), flagColor, GL_STATIC_DRAW);

	glVertexAttribPointer(AMC_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	//glVertexAttrib3f(AMC_ATTRIBUTE_COLOR, 1.0f, 0.0f, 1.0f);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR);

	glBindBuffer(GL_ARRAY_BUFFER, 0);


	glBindVertexArray(0);


	const GLfloat plane[] = {

-0.2, 0.05f,
-0.2, -0.05f,
0.4, -0.05f,
0.4, 0.05f,
		-0.1f, 0.05f,
	-0.4f, 0.3f,
	-0.2f, 0.05f,


	-0.1f, -0.05f,
	-0.4f, -0.3f,
	-0.2f, -0.05f,


	0.5f, 0.0f,
	0.4f, 0.05f,
	0.4f, -0.05f,

	0.2f, 0.05f,
	-0.3f, 0.4f,
	0.0f, 0.05f,

	0.0f, -0.05f,
	-0.3f, -0.4f,
	0.2f, -0.05f
	
	};
	const GLfloat planeColor[] = {
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
	
	//plane 

	glGenVertexArrays(1, &Vao_Plane);
	glBindVertexArray(Vao_Plane);

	glGenBuffers(1, &Vbo_Position);
	glBindBuffer(GL_ARRAY_BUFFER, Vbo_Position);
	glBufferData(GL_ARRAY_BUFFER, sizeof(plane), plane, GL_STATIC_DRAW);

	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 2, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//glGenBuffers(1, &Vbo_Color);
	//glBindBuffer(GL_ARRAY_BUFFER, Vbo_Color);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(planeColor), planeColor, GL_STATIC_DRAW);

	glVertexAttribPointer(AMC_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glVertexAttrib3f(AMC_ATTRIBUTE_COLOR, (float)186 / 255, (float)226 / 255, (float)238 / 255);

	//glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR);

	glBindBuffer(GL_ARRAY_BUFFER, 0);


	glBindVertexArray(0);

	const GLfloat IAF[] = {
		0.01f, 0.04f,
	0.01f, -0.04f,

	0.05f, 0.04f,
	0.03f, -0.04f,
	0.05f, 0.04f,
	0.07f, -0.04f,


	0.09f, 0.04f,
	0.09f, -0.04f,
	0.09f, 0.04f,
	0.12f, 0.04,
	0.09f, 0.01f,
	0.12f, 0.01f,

	};

	const GLfloat IAFColor[] = {
		0.0f,0.0f,0.0f,
		0.0f,0.0f,0.0f,
		0.0f,0.0f,0.0f,
		0.0f,0.0f,0.0f,
		0.0f,0.0f,0.0f,
		0.0f,0.0f,0.0f,
		0.0f,0.0f,0.0f,
		0.0f,0.0f,0.0f,
		0.0f,0.0f,0.0f,
		0.0f,0.0f,0.0f,
		0.0f,0.0f,0.0f,
		0.0f,0.0f,0.0f,
		0.0f,0.0f,0.0f,
		0.0f,0.0f,0.0f,
		0.0f,0.0f,0.0f,
		0.0f,0.0f,0.0f,
		0.0f,0.0f,0.0f,
		0.0f,0.0f,0.0f,
		0.0f,0.0f,0.0f,
		0.0f,0.0f,0.0f,
		0.0f,0.0f,0.0f,
		0.0f,0.0f,0.0f,
		0.0f,0.0f,0.0f,
		0.0f,0.0f,0.0f,
		0.0f,0.0f,0.0f,
		0.0f,0.0f,0.0f,
		0.0f,0.0f,0.0f,
		0.0f,0.0f,0.0f,
		0.0f,0.0f,0.0f,
		0.0f,0.0f,0.0f,
		0.0f,0.0f,0.0f,
	};

	GLfloat smoke[] = {
		-0.23, 0.08f,
-4.0, 0.08f,
-0.23f, 0.05f,
-4.0f, 0.05f,
-0.23f, 0.02f,
-4.0f, 0.02f
	};
	
	//Smoke
	glGenVertexArrays(1, &Vao_Smoke);
	glBindVertexArray(Vao_Smoke);

	glGenBuffers(1, &Vbo_Position);
	glBindBuffer(GL_ARRAY_BUFFER, Vbo_Position);
	glBufferData(GL_ARRAY_BUFFER, sizeof(smoke), smoke, GL_STATIC_DRAW);

	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 2, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &Vbo_Color);
	glBindBuffer(GL_ARRAY_BUFFER, Vbo_Color);
	glBufferData(GL_ARRAY_BUFFER, sizeof(flagColor), flagColor, GL_STATIC_DRAW);

	glVertexAttribPointer(AMC_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	//glVertexAttrib3f(AMC_ATTRIBUTE_COLOR, 1.0f, 0.0f, 1.0f);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR);

	glBindBuffer(GL_ARRAY_BUFFER, 0);



	//IAF
	glGenVertexArrays(1, &Vao_IAF);
	glBindVertexArray(Vao_IAF);

	glGenBuffers(1, &Vbo_Position);
	glBindBuffer(GL_ARRAY_BUFFER, Vbo_Position);
	glBufferData(GL_ARRAY_BUFFER, sizeof(IAF), IAF, GL_STATIC_DRAW);

	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 2, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &Vbo_Color);
	glBindBuffer(GL_ARRAY_BUFFER, Vbo_Color);
	glBufferData(GL_ARRAY_BUFFER, sizeof(IAFColor), IAFColor, GL_STATIC_DRAW);

	glVertexAttribPointer(AMC_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	//glVertexAttrib3f(AMC_ATTRIBUTE_COLOR, (float)186 / 255, (float)226 / 255, (float)238 / 255);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR);

	glBindBuffer(GL_ARRAY_BUFFER, 0);


	//2nd I
	glGenVertexArrays(1, &Vao_SecondI);
	glBindVertexArray(Vao_SecondI);

	glGenBuffers(1, &Vbo_Position_SecondI);
	glBindBuffer(GL_ARRAY_BUFFER, Vbo_Position_SecondI);
	glBufferData(GL_ARRAY_BUFFER, sizeof(I), I, GL_STATIC_DRAW);

	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &Vbo_Color_SecondI);
	glBindBuffer(GL_ARRAY_BUFFER, Vbo_Color_SecondI);
	glBufferData(GL_ARRAY_BUFFER, sizeof(TriColor), TriColor, GL_STATIC_DRAW);

	glVertexAttribPointer(AMC_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	//glVertexAttrib3f(AMC_ATTRIBUTE_COLOR, 1.0f, 0.0f, 1.0f);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR);

	glBindBuffer(GL_ARRAY_BUFFER, 0);


	glBindVertexArray(0);


	//A
	glGenVertexArrays(1, &Vao_A);
	glBindVertexArray(Vao_A);

	glGenBuffers(1, &Vbo_Position_A);
	glBindBuffer(GL_ARRAY_BUFFER, Vbo_Position_A);
	glBufferData(GL_ARRAY_BUFFER, sizeof(A), A, GL_STATIC_DRAW);

	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &Vbo_Color_A);
	glBindBuffer(GL_ARRAY_BUFFER, Vbo_Color_A);
	glBufferData(GL_ARRAY_BUFFER, sizeof(AColor), AColor, GL_STATIC_DRAW);

	glVertexAttribPointer(AMC_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	//glVertexAttrib3f(AMC_ATTRIBUTE_COLOR, 1.0f, 0.0f, 1.0f);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR);

	glBindBuffer(GL_ARRAY_BUFFER, 0);


	glBindVertexArray(0);

	//For D
	glGenVertexArrays(1, &Vao_D);
	glBindVertexArray(Vao_D);

	glGenBuffers(1, &Vbo_Position_D);
	glBindBuffer(GL_ARRAY_BUFFER, Vbo_Position_D);
	glBufferData(GL_ARRAY_BUFFER, sizeof(D), D, GL_STATIC_DRAW);

	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &Vbo_Color_D);
	glBindBuffer(GL_ARRAY_BUFFER, Vbo_Color_D);
	glBufferData(GL_ARRAY_BUFFER, sizeof(TriColor), TriColor, GL_STATIC_DRAW);

	glVertexAttribPointer(AMC_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	//glVertexAttrib3f(AMC_ATTRIBUTE_COLOR, 0.0f, 0.0f, 1.0f);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//I
	glGenVertexArrays(1, &Vao_I);
	glBindVertexArray(Vao_I);

	glGenBuffers(1, &Vbo_Position_I);
	glBindBuffer(GL_ARRAY_BUFFER, Vbo_Position_I);
	glBufferData(GL_ARRAY_BUFFER, sizeof(I), I, GL_STATIC_DRAW);

	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &Vbo_Color_I);
	glBindBuffer(GL_ARRAY_BUFFER, Vbo_Color_I);
	glBufferData(GL_ARRAY_BUFFER, sizeof(TriColor), TriColor, GL_STATIC_DRAW);

	glVertexAttribPointer(AMC_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	//glVertexAttrib3f(AMC_ATTRIBUTE_COLOR, 1.0f, 1.0f, 0.0f);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	//N
	glGenVertexArrays(1, &Vao_N);
	glBindVertexArray(Vao_N);

	glGenBuffers(1, &Vbo_Position_N);
	glBindBuffer(GL_ARRAY_BUFFER, Vbo_Position_N);
	glBufferData(GL_ARRAY_BUFFER, sizeof(N), N, GL_STATIC_DRAW);

	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &Vbo_Color_N);
	glBindBuffer(GL_ARRAY_BUFFER, Vbo_Color_N);
	glBufferData(GL_ARRAY_BUFFER, sizeof(NColor), NColor, GL_STATIC_DRAW);

	glVertexAttribPointer(AMC_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	//glVertexAttrib3f(AMC_ATTRIBUTE_COLOR, 1.0f, 1.0f, 0.0f);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);



	glClearDepth(1.0f);
	//glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
	glDisable(GL_CULL_FACE);
	//glDisable(GL_FRONT_FACE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	PerspectiveProjectionMatrix = mat4::identity();
	ToggleFullscreen();
	//resize(WIN_WIDTH, WIN_HEIGHT);
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

	

	glLineWidth(3.0f);


	//glBindVertexArray(Vao_SecondI);
	glBindVertexArray(Vao_I);
	modelViewMatrix = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();
	rotationMatrix = mat4::identity();
	translationMatrix = mat4::identity();

	translationMatrix = translate(1.0f, iitrans, -6.0f);
	modelViewMatrix = modelViewMatrix * translationMatrix;
	modelViewProjectionMatrix = PerspectiveProjectionMatrix * modelViewMatrix;

	glUniformMatrix4fv(MVPUniform, 1, GL_FALSE, modelViewProjectionMatrix);
	glUniform1f(alphaUniform, 1.0f);

	glDrawArrays(GL_LINES, 0, 2);

	glBindVertexArray(0);

	glBindVertexArray(Vao_TriLine);
	modelViewMatrix = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();
	rotationMatrix = mat4::identity();
	translationMatrix = mat4::identity();

	translationMatrix = translate(xFlagtrans, 0.0f, -6.0f);
	modelViewMatrix = modelViewMatrix * translationMatrix;
	modelViewProjectionMatrix = PerspectiveProjectionMatrix * modelViewMatrix;

	glUniformMatrix4fv(MVPUniform, 1, GL_FALSE, modelViewProjectionMatrix);
	glUniform1f(alphaUniform, 1.0f);

	glDrawArrays(GL_LINES, 0, 6);
	//glDrawArrays(GL_LINES, 2, 6);
	//glDrawArrays(GL_LINES, 4, 6);

	glBindVertexArray(0);


	glBindVertexArray(Vao_A);
	//glLineWidth(3.0f);
	modelViewMatrix = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();
	rotationMatrix = mat4::identity();
	translationMatrix = mat4::identity();


	translationMatrix = translate(atrans, 0.0f, -6.0f);
	modelViewMatrix = modelViewMatrix * translationMatrix;
	modelViewProjectionMatrix = PerspectiveProjectionMatrix * modelViewMatrix;

	glUniformMatrix4fv(MVPUniform, 1, GL_FALSE, modelViewProjectionMatrix);
	glUniform1f(alphaUniform, 1.0f);

	glDrawArrays(GL_LINES, 0, 6);
	//glDrawArrays(GL_LINE_LOOP, 3, 6);
	//glDrawArrays(GL_LINES, 0, 3);

	glBindVertexArray(0);


	
	glLineWidth(3.0f);


	glBindVertexArray(Vao_I);
	//glLineWidth(3.0f);
	modelViewMatrix = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();
	rotationMatrix = mat4::identity();
	translationMatrix = mat4::identity();


	translationMatrix = translate(itrans, 0.0f, -6.0f);
	modelViewMatrix = modelViewMatrix * translationMatrix;
	modelViewProjectionMatrix = PerspectiveProjectionMatrix * modelViewMatrix;

	glUniformMatrix4fv(MVPUniform, 1, GL_FALSE, modelViewProjectionMatrix);
	glUniform1f(alphaUniform, 1.0f);

	glDrawArrays(GL_LINES, 0, 3);

	glBindVertexArray(0);


	glBindVertexArray(Vao_N);
	modelViewMatrix = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();
	rotationMatrix = mat4::identity();
	translationMatrix = mat4::identity();


	translationMatrix = translate(-1.5f, ntrans, -6.0f);
	modelViewMatrix = modelViewMatrix * translationMatrix;
	modelViewProjectionMatrix = PerspectiveProjectionMatrix * modelViewMatrix;

	glUniformMatrix4fv(MVPUniform, 1, GL_FALSE, modelViewProjectionMatrix);
	glUniform1f(alphaUniform, 1.0f);

	glDrawArrays(GL_LINE_STRIP, 0, 7);

	glBindVertexArray(0);


	glBindVertexArray(Vao_D);
	//glLineWidth(1.0f);
	modelViewMatrix = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();
	rotationMatrix = mat4::identity();
	translationMatrix = mat4::identity();
	//MultiplyMatrices
	//modelViewMatrix = translate(0.0f, 0.0f, 0.0f);
	translationMatrix = translate(0.2f, 0.0f, -6.0f);
	modelViewMatrix = modelViewMatrix * translationMatrix;
	modelViewProjectionMatrix = PerspectiveProjectionMatrix * modelViewMatrix;

	glUniformMatrix4fv(MVPUniform, 1, GL_FALSE, modelViewProjectionMatrix);
	glUniform1f(alphaUniform,alphaBlend);

	glDrawArrays(GL_LINES, 0, 2);
	glDrawArrays(GL_LINES, 2, 4);
	glDrawArrays(GL_LINES, 4, 8);

	glBindVertexArray(0);

	glLineWidth(3.0f);
	glBindVertexArray(Vao_Smoke);
	modelViewMatrix = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();
	rotationMatrix = mat4::identity();
	translationMatrix = mat4::identity();

	translationMatrix = translate(xTritrans, 0.0f, -6.0f);
	modelViewMatrix = modelViewMatrix * translationMatrix;
	modelViewProjectionMatrix = PerspectiveProjectionMatrix * modelViewMatrix;

	glUniformMatrix4fv(MVPUniform, 1, GL_FALSE, modelViewProjectionMatrix);
	glUniform1f(alphaUniform, 1.0f);

	glDrawArrays(GL_LINES, 0, 6);
	//glDrawArrays(GL_LINES, 2, 6);
	//glDrawArrays(GL_LINES, 4, 6);

	glBindVertexArray(0);

	
	//PLANE1
	glBindVertexArray(Vao_Plane);
	//glLineWidth(3.0f);
	modelViewMatrix = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();
	rotationMatrix = mat4::identity();
	translationMatrix = mat4::identity();


	translationMatrix = translate(xPlanetrans, 0.078f, -8.0f);
	modelViewMatrix = modelViewMatrix * translationMatrix;
	modelViewProjectionMatrix = PerspectiveProjectionMatrix * modelViewMatrix;

	glUniformMatrix4fv(MVPUniform, 1, GL_FALSE, modelViewProjectionMatrix);
	glUniform1f(alphaUniform, 1.0f);

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glDrawArrays(GL_TRIANGLES, 4, 21);
	//glDrawArrays(GL_TRIANGLES, 3, 6);
	//glDrawArrays(GL_TRIANGLES, 6, 9);
	//glDrawArrays(GL_LINE_LOOP, 3, 6);
	//glDrawArrays(GL_LINES, 0, 3);

	glBindVertexArray(0);

	//TopPLANE2
	glBindVertexArray(Vao_Plane);
	//glLineWidth(3.0f);
	modelViewMatrix = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();
	rotationMatrix = mat4::identity();
	translationMatrix = mat4::identity();


	translationMatrix = translate(xPlane1trans, yPlane1trans, -8.0f);
	modelViewMatrix = modelViewMatrix * translationMatrix;
	modelViewProjectionMatrix = PerspectiveProjectionMatrix * modelViewMatrix;

	glUniformMatrix4fv(MVPUniform, 1, GL_FALSE, modelViewProjectionMatrix);
	glUniform1f(alphaUniform, 1.0f);

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glDrawArrays(GL_TRIANGLES, 4, 21);
	//glDrawArrays(GL_TRIANGLES, 3, 6);
	//glDrawArrays(GL_TRIANGLES, 6, 9);
	//glDrawArrays(GL_LINE_LOOP, 3, 6);
	//glDrawArrays(GL_LINES, 0, 3);

	glBindVertexArray(0);

	//bottomPLANE3
	glBindVertexArray(Vao_Plane);
	//glLineWidth(3.0f);
	modelViewMatrix = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();
	rotationMatrix = mat4::identity();
	translationMatrix = mat4::identity();


	translationMatrix = translate(xPlane2trans, yPlane2trans, -8.0f);
	modelViewMatrix = modelViewMatrix * translationMatrix;
	modelViewProjectionMatrix = PerspectiveProjectionMatrix * modelViewMatrix;

	glUniformMatrix4fv(MVPUniform, 1, GL_FALSE, modelViewProjectionMatrix);
	glUniform1f(alphaUniform, 1.0f);

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glDrawArrays(GL_TRIANGLES, 4, 21);
	//glDrawArrays(GL_TRIANGLES, 3, 6);
	//glDrawArrays(GL_TRIANGLES, 6, 9);
	//glDrawArrays(GL_LINE_LOOP, 3, 6);
	//glDrawArrays(GL_LINES, 0, 3);

	glBindVertexArray(0);


	//IAF_PLANE1
	glBindVertexArray(Vao_IAF);
	glLineWidth(1.0f);
	modelViewMatrix = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();
	rotationMatrix = mat4::identity();
	translationMatrix = mat4::identity();


	translationMatrix = translate(xIAFtrans, 0.078f, -8.0f);
	modelViewMatrix = modelViewMatrix * translationMatrix;
	modelViewProjectionMatrix = PerspectiveProjectionMatrix * modelViewMatrix;

	glUniformMatrix4fv(MVPUniform, 1, GL_FALSE, modelViewProjectionMatrix);
	glUniform1f(alphaUniform, 1.0f);

	//glDrawArrays(GL_LINE_LOOP, 3, 6);
	glDrawArrays(GL_LINES, 0, 12);
//	glUniform1f(alphaUniform, 1.0f);

	glBindVertexArray(0);

	//IAF_PLANE2
	glBindVertexArray(Vao_IAF);
	glLineWidth(1.0f);
	modelViewMatrix = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();
	rotationMatrix = mat4::identity();
	translationMatrix = mat4::identity();


	translationMatrix = translate(xPlane1trans, yPlane1trans, -8.0f);
	modelViewMatrix = modelViewMatrix * translationMatrix;
	modelViewProjectionMatrix = PerspectiveProjectionMatrix * modelViewMatrix;

	glUniformMatrix4fv(MVPUniform, 1, GL_FALSE, modelViewProjectionMatrix);
	glUniform1f(alphaUniform, 1.0f);

	//glDrawArrays(GL_LINE_LOOP, 3, 6);
	glDrawArrays(GL_LINES, 0, 12);

	glBindVertexArray(0);
	
	//IAF_PLANE3
	glBindVertexArray(Vao_IAF);
	glLineWidth(1.0f);
	modelViewMatrix = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();
	rotationMatrix = mat4::identity();
	translationMatrix = mat4::identity();


	translationMatrix = translate(xPlane2trans, yPlane2trans, -6.0f);
	modelViewMatrix = modelViewMatrix * translationMatrix;
	modelViewProjectionMatrix = PerspectiveProjectionMatrix * modelViewMatrix;

	glUniformMatrix4fv(MVPUniform, 1, GL_FALSE, modelViewProjectionMatrix);
	glUniform1f(alphaUniform, 1.0f);

	//glDrawArrays(GL_LINE_LOOP, 3, 6);
	glDrawArrays(GL_LINES, 0, 12);

	glBindVertexArray(0);

	glUseProgram(0);

	SwapBuffers(ghdc);
}

void update(void)
{
	if (itrans <= -2.5)
	{
		//itrans = itrans + 0.0002;
		itrans = itrans + 0.02;
		//ytrans = ytrans + 0.001;
	}

	if (itrans >= -2.5)
	{
		if (atrans > 2.0)
		{
			atrans = atrans - 0.0002;
			//yctrans = yctrans + 0.001;

		}

	}

	if (atrans < 2.0)
	{
		if (ntrans >= 0.0)
		{
			ntrans = ntrans - 0.0002;
		}
	}

	if (ntrans < 0.0)
	{
		if (iitrans <= 0.0)
		{
			iitrans = iitrans + 0.0002;
		}
	}

	if (iitrans > 0.0)
	{
		alphaBlend = alphaBlend + 0.001f;

	}

	if (alphaBlend > 1.0)
	{
		if (xTritrans < 8.0)
		{
			xTritrans = xTritrans + 0.0003;

		}
		xPlanetrans = xPlanetrans + 0.0004;
		xPlane1trans = xPlane1trans + 0.0004;
		xPlane2trans = xPlane2trans + 0.0004;
		xIAFtrans = xIAFtrans + 0.0004;
		//Topangle = Topangle + 0.008;
		//Bottomangle = Bottomangle - 0.008;

		if (yPlane1trans > 0.078)
		{
			yPlane1trans = yPlane1trans - 0.0002;

		}
		if (yPlane2trans < 0.078)
		{
			yPlane2trans = yPlane2trans + 0.0002;
		}

		if (xPlanetrans > 3.0)
		{
			xPlane1trans = xPlane1trans + 0.0001;
			yPlane1trans = yPlane1trans - 0.0007;

			Topangle = Topangle - 0.008;
			if (Topangle < -45.0)
			{
				Topangle = -45.0;
			}
		}

		if (xPlanetrans > 3.0)
		{
			xPlane2trans = xPlane2trans + 0.0001;
			yPlane2trans = yPlane2trans + 0.0007;

			Bottomangle = Bottomangle + 0.008;
			if (Bottomangle > 45.0)
			{
				Bottomangle = 45.0;
			}
		}

	}

	if (xTritrans > 1.5)
	{
		xFlagtrans = 2.0f;
	}
/*	if (xOrColor >= 1.0)
	{

	}*/

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

void translatemy(void) {
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
			xFlagtrans = 2.0f;
		}

		//}
		/*if (Bottomangle < 0.0)
		{

		}*/
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

	if (Vao_SecondI)
	{
		glDeleteVertexArrays(1, &Vao_SecondI);
		Vao_SecondI = 0;
	}

	if (Vao_D)
	{
		glDeleteVertexArrays(1, &Vao_D);
		Vao_D = 0;
	}

	if (Vbo_Position_A)
	{
		glDeleteBuffers(1, &Vbo_Position_A);
		Vbo_Position_A = 0;
	}

	if (Vbo_Position_SecondI)
	{
		glDeleteBuffers(1, &Vbo_Position_SecondI);
		Vbo_Position_SecondI = 0;
	}

	if (Vbo_Color_N)
	{
		glDeleteBuffers(1, &Vbo_Color_N);
		Vbo_Color_N = 0;
	}

	if (Vao_I)
	{
		glDeleteVertexArrays(1, &Vao_I);
		Vao_I = 0;
	}

	if (Vbo_Position_I)
	{
		glDeleteBuffers(1, &Vbo_Position_I);
		Vbo_Position_I = 0;
	}

	if (Vbo_Position_D)
	{
		glDeleteBuffers(1, &Vbo_Position_D);
		Vbo_Position_D = 0;
	}

	if (Vbo_Position_orangeLine)
	{
		glDeleteBuffers(1, &Vbo_Position_orangeLine);
		Vbo_Position_orangeLine = 0;
	}


	if (Vbo_Color_I)
	{
		glDeleteBuffers(1, &Vbo_Color_I);
		Vbo_Color_I = 0;

	}

	if (Vbo_Color_TriLine)
	{
		glDeleteBuffers(1, &Vbo_Color_TriLine);
		Vbo_Color_TriLine = 0;
	}

	if (Vbo_Color_orangeLine)
	{
		glDeleteBuffers(1, &Vbo_Color_orangeLine);
		Vbo_Color_orangeLine = 0;
	}

	if (Vbo_Color)
	{
		glDeleteBuffers(1, &Vbo_Color);
		Vbo_Color = 0;
	}

	if (Vao_A)
	{
		glDeleteVertexArrays(1, &Vao_A);
		Vao_A = 0;
	}

	if (Vao_N)
	{
		glDeleteVertexArrays(1, &Vao_N);
		Vao_N = 0;
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
