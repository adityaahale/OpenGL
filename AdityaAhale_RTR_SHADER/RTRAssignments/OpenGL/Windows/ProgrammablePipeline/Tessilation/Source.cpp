#include <windows.h>
#include <stdio.h> 
#include "vmath.h"

#include <gl\glew.h>

#include <gl/GL.h>

using namespace vmath;

#pragma comment(lib,"glew32.lib")
#pragma comment(lib,"opengl32.lib")

#define WIN_WIDTH_ATA 800
#define WIN_HEIGHT_ATA 600
#define PI 3.141592653


enum
{
	ATA_ATTRIBUTE_POSITION = 0,
	ATA_ATTRIBUTE_COLOR,
	ATA_ATTRIBUTE_NORMAL,
	ATA_ATTRIBUTE_TEXTURE0,
};


LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

//Global variable declarations
FILE *gpFile = NULL;

HWND ghwnd = NULL;
HDC ghdc = NULL;
HGLRC ghrc = NULL;

DWORD dwStyle;
WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };

bool gbActiveWindowAta = false;
bool gbEscapeKeyIsPressedAta = false;
bool gbFullscreenAta = false;

GLuint VertexShaderObjectAta;
GLuint TessellationControlShaderObjectAta;
GLuint TessellationEvaluationShaderObjectAta;
GLuint FragmentShaderObjectAta;
GLuint ShaderProgramObjectAta;

GLuint Vao_VerLine;
GLuint Vbo_Position_VerLine;
GLuint NumberOfSegmentsUniformAta;
GLuint NumberOfStripsUniformAta;
GLuint Vbo_Color_VerLine;
GLuint LineColorUniformAta;

GLuint MVPUniformAta;

mat4 PerspectiveProjectionMatrixAta;
unsigned int NumberOfLineSegmentsAta;

GLfloat angleTriAta = 0.0f, angleSqAta = 0.0f;
int height, width;



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
		TEXT("Tessellation Shader"),
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
		100,
		100,
		WIN_WIDTH_ATA,
		WIN_HEIGHT_ATA,
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

			if (gbActiveWindowAta == true)
			{
				if (gbEscapeKeyIsPressedAta == true)
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
			gbActiveWindowAta = true;
		else
			gbActiveWindowAta = false;
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
			gbEscapeKeyIsPressedAta = true;
			break;
		case 0x46:
			if (gbFullscreenAta == false)
			{
				ToggleFullscreen();
				gbFullscreenAta = true;
			}
			else
			{
				ToggleFullscreen();
				gbFullscreenAta = false;
			}
			break;
		case VK_UP:
			NumberOfLineSegmentsAta++;
			if (NumberOfLineSegmentsAta >= 50)
				NumberOfLineSegmentsAta = 50;
			break;
		case VK_DOWN:
			NumberOfLineSegmentsAta--;
			if (NumberOfLineSegmentsAta <= 0)
				NumberOfLineSegmentsAta = 1;
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
	if (gbFullscreenAta == false)
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
	VertexShaderObjectAta = glCreateShader(GL_VERTEX_SHADER);

	const GLchar *vertexShaderSourceCodeAta =
		"#version 430 core" \
		"\n" \
		"in vec2 vPosition;" \
		"void main(void)" \
		"{" \
		"gl_Position = vec4(vPosition, 0.0, 1.0);" \
		"}";
	glShaderSource(VertexShaderObjectAta, 1, (const GLchar **)&vertexShaderSourceCodeAta, NULL);

	glCompileShader(VertexShaderObjectAta);
	GLint iInfoLogLengthAta = 0;
	GLint iShaderCompiledStatusAta = 0;
	char *szInfoLogAta = NULL;
	glGetShaderiv(VertexShaderObjectAta, GL_COMPILE_STATUS, &iShaderCompiledStatusAta);
	if (iShaderCompiledStatusAta == GL_FALSE)
	{
		glGetShaderiv(VertexShaderObjectAta, GL_INFO_LOG_LENGTH, &iInfoLogLengthAta);
		if (iInfoLogLengthAta > 0)
		{
			szInfoLogAta = (char *)malloc(iInfoLogLengthAta);
			if (szInfoLogAta != NULL)
			{
				GLsizei writtenAta;
				glGetShaderInfoLog(VertexShaderObjectAta, iInfoLogLengthAta, &writtenAta, szInfoLogAta);
				fprintf(gpFile, "Vertex Shader Compilation Log : %s\n", szInfoLogAta);
				free(szInfoLogAta);
				uninitialize();
				exit(0);
			}
		}
	}
	
	//TESSELLATION CONTROL SHADER
	TessellationControlShaderObjectAta = glCreateShader(GL_TESS_CONTROL_SHADER);
	const GLchar *tessellationControlShaderSourceCodeAta =         
		"#version 430" \
		"\n" \
		"layout(vertices=4) out;"\
		"uniform int numberOfSegments;" \
		"uniform int numberOfStrips;" \
		"void main(void)" \
		"{" \
		"gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;" \
		"gl_TessLevelOuter[0] = float(numberOfStrips);" \
		"gl_TessLevelOuter[1] = float(numberOfSegments);" \
		"}";

	glShaderSource(TessellationControlShaderObjectAta, 1, (const GLchar**)&tessellationControlShaderSourceCodeAta, NULL); //NULL is for NULL terminated source code string

																						  
	glCompileShader(TessellationControlShaderObjectAta);
	iInfoLogLengthAta = 0;
	iShaderCompiledStatusAta = 0;
	szInfoLogAta = NULL;
	glGetShaderiv(TessellationControlShaderObjectAta, GL_COMPILE_STATUS, &iShaderCompiledStatusAta);
	if (iShaderCompiledStatusAta == GL_FALSE)
	{
		glGetShaderiv(TessellationControlShaderObjectAta, GL_INFO_LOG_LENGTH, &iInfoLogLengthAta);
		if (iInfoLogLengthAta > 0)
		{
			szInfoLogAta = (char *)malloc(iInfoLogLengthAta);
			if (szInfoLogAta != NULL)
			{
				GLsizei writtenAta;
				glGetShaderInfoLog(TessellationControlShaderObjectAta, iInfoLogLengthAta, &writtenAta, szInfoLogAta);
				fprintf(gpFile, "Tessellation Control Shader Compilation Log : %s\n", szInfoLogAta);
				free(szInfoLogAta);
				uninitialize();
				exit(0);
			}
		}
	}

	//TESSELLATION EVALUATION SHADER
	TessellationEvaluationShaderObjectAta = glCreateShader(GL_TESS_EVALUATION_SHADER);
	const GLchar *tessellationEvaluationShaderSourceCodeAta =
		"#version 430" \
		"\n" \
		"layout(isolines) in;"\
		"uniform mat4 u_mvp_matrix;"
		"void main(void)" \
		"{" \
		"float u = gl_TessCoord.x;" \
		"vec3 p0 = gl_in[0].gl_Position.xyz;"\
		"vec3 p1 = gl_in[1].gl_Position.xyz;"\
		"vec3 p2 = gl_in[2].gl_Position.xyz;"\
		"vec3 p3 = gl_in[3].gl_Position.xyz;"\
		"float u1 = (1.0 - u);" \
		"float u2 = u * u;" \
		"float b3 = u2 * u;" \
		"float b2 = 3.0 * u2* u1;" \
		"float b1 = 3.0 * u * u1 * u1; " \
		"float b0 = u1 * u1 * u1; " \
		"vec3 p = p0 * b0 + p1*b1 + p2*b2 + p3*b3;" \
		"gl_Position = u_mvp_matrix * vec4(p, 1.0);"\
		"}";

	glShaderSource(TessellationEvaluationShaderObjectAta, 1, (const GLchar**)&tessellationEvaluationShaderSourceCodeAta, NULL); //NULL is for NULL terminated source code string

																													 //compile vertex shader
	glCompileShader(TessellationEvaluationShaderObjectAta);
	iInfoLogLengthAta = 0;
	iShaderCompiledStatusAta = 0;
	szInfoLogAta = NULL;
	glGetShaderiv(TessellationEvaluationShaderObjectAta, GL_COMPILE_STATUS, &iShaderCompiledStatusAta);
	if (iShaderCompiledStatusAta == GL_FALSE)
	{
		glGetShaderiv(TessellationEvaluationShaderObjectAta, GL_INFO_LOG_LENGTH, &iInfoLogLengthAta);
		if (iInfoLogLengthAta > 0)
		{
			szInfoLogAta = (char *)malloc(iInfoLogLengthAta);
			if (szInfoLogAta != NULL)
			{
				GLsizei writtenAta;
				glGetShaderInfoLog(TessellationEvaluationShaderObjectAta, iInfoLogLengthAta, &writtenAta, szInfoLogAta);
				fprintf(gpFile, "Tessellation Evaluation Shader Compilation Log : %s\n", szInfoLogAta);
				free(szInfoLogAta);
				uninitialize();
				exit(0);
			}
		}
	}

	//FRAGMENT SHADER
	FragmentShaderObjectAta = glCreateShader(GL_FRAGMENT_SHADER);

	const GLchar *fragmentShaderSourceCodeAta =
		"#version 430" \
		"\n" \
		"out vec4 FragColor;" \
		"uniform vec4 lineColor;" \
		"void main(void)" \
		"{" \
		"FragColor = lineColor;" \
		"}";
	glShaderSource(FragmentShaderObjectAta, 1, (const GLchar **)&fragmentShaderSourceCodeAta, NULL);

	glCompileShader(FragmentShaderObjectAta);
	glGetShaderiv(FragmentShaderObjectAta, GL_COMPILE_STATUS, &iShaderCompiledStatusAta);
	if (iShaderCompiledStatusAta == GL_FALSE)
	{
		glGetShaderiv(FragmentShaderObjectAta, GL_INFO_LOG_LENGTH, &iInfoLogLengthAta);
		if (iInfoLogLengthAta > 0)
		{
			szInfoLogAta = (char *)malloc(iInfoLogLengthAta);
			if (szInfoLogAta != NULL)
			{
				GLsizei writtenAta;
				glGetShaderInfoLog(FragmentShaderObjectAta, iInfoLogLengthAta, &writtenAta, szInfoLogAta);
				fprintf(gpFile, "Fragment Shader Compilation Log : %s\n", szInfoLogAta);
				free(szInfoLogAta);
				uninitialize();
				exit(0);
			}
		}
	}

	//SHADER PROGRAM
	ShaderProgramObjectAta = glCreateProgram();

	glAttachShader(ShaderProgramObjectAta, VertexShaderObjectAta);
	//Tessellation shaders
	glAttachShader(ShaderProgramObjectAta, TessellationControlShaderObjectAta);
	glAttachShader(ShaderProgramObjectAta, TessellationEvaluationShaderObjectAta);
	
	glAttachShader(ShaderProgramObjectAta, FragmentShaderObjectAta);

	glBindAttribLocation(ShaderProgramObjectAta, ATA_ATTRIBUTE_POSITION, "vPosition");
	

	glLinkProgram(ShaderProgramObjectAta);
	GLint iShaderProgramLinkStatus = 0;
	glGetProgramiv(ShaderProgramObjectAta, GL_LINK_STATUS, &iShaderProgramLinkStatus);
	if (iShaderProgramLinkStatus == GL_FALSE)
	{
		glGetProgramiv(ShaderProgramObjectAta, GL_INFO_LOG_LENGTH, &iInfoLogLengthAta);
		if (iInfoLogLengthAta > 0)
		{
			szInfoLogAta = (char *)malloc(iInfoLogLengthAta);
			if (szInfoLogAta != NULL)
			{
				GLsizei writtenAta;
				glGetProgramInfoLog(ShaderProgramObjectAta, iInfoLogLengthAta, &writtenAta, szInfoLogAta);
				fprintf(gpFile, "Shader Program Link Log : %s\n", szInfoLogAta);
				free(szInfoLogAta);
				uninitialize();
				exit(0);
			}
		}
	}

	MVPUniformAta = glGetUniformLocation(ShaderProgramObjectAta, "u_mvp_matrix");
	NumberOfSegmentsUniformAta = glGetUniformLocation(ShaderProgramObjectAta, "numberOfSegments");
	NumberOfStripsUniformAta = glGetUniformLocation(ShaderProgramObjectAta, "numberOfStrips");
	LineColorUniformAta = glGetUniformLocation(ShaderProgramObjectAta, "lineColor");

	const GLfloat verticalLine[] = {-1.0f, -1.0f, -0.5f, 1.0f, 0.5f, -1.0f, 1.0f, 1.0f };
	
	//For Vertical Line
	glGenVertexArrays(1, &Vao_VerLine);
	glBindVertexArray(Vao_VerLine);

	glGenBuffers(1, &Vbo_Position_VerLine);
	glBindBuffer(GL_ARRAY_BUFFER, Vbo_Position_VerLine);
	glBufferData(GL_ARRAY_BUFFER,8 * sizeof(float), verticalLine, GL_STATIC_DRAW);

	glVertexAttribPointer(ATA_ATTRIBUTE_POSITION, 2, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(ATA_ATTRIBUTE_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);



	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
	glDisable(GL_CULL_FACE);
	glLineWidth(3.0f);
	//glDisable(GL_FRONT_FACE);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	PerspectiveProjectionMatrixAta = mat4::identity();

	NumberOfLineSegmentsAta = 1;

	resize(WIN_WIDTH_ATA, WIN_HEIGHT_ATA);
}

void display(void)
{
	//GLuint ShaderProgramObjectAta;
	//code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(ShaderProgramObjectAta);

	mat4 modelViewMatrixAta;
	mat4 modelViewProjectionMatrixAta;
	mat4 rotationMatrix;
	mat4 translationMatrixAta;

	// OpenGL Drawing

	modelViewMatrixAta = mat4::identity();
	modelViewProjectionMatrixAta = mat4::identity();
	rotationMatrix = mat4::identity();
	translationMatrixAta = mat4::identity();

	glBindVertexArray(Vao_VerLine);
	translationMatrixAta = translate(0.0f, 0.0f, -5.0f);
	modelViewMatrixAta = modelViewMatrixAta * translationMatrixAta;
	modelViewProjectionMatrixAta = PerspectiveProjectionMatrixAta * modelViewMatrixAta;

	glUniformMatrix4fv(MVPUniformAta, 1, GL_FALSE, modelViewProjectionMatrixAta);
	glUniform1i(NumberOfSegmentsUniformAta, NumberOfLineSegmentsAta);
	TCHAR str[255];
	wsprintf(str, TEXT("Segments: %d"), NumberOfLineSegmentsAta);
	SetWindowText(ghwnd, str);
	glUniform1i(NumberOfStripsUniformAta, 1);

	if (NumberOfLineSegmentsAta == 1)
	{
		glUniform4fv(LineColorUniformAta, 1, vmath::vec4(0.0f, 1.0f, 0.0f, 1.0f));
	}
	else if (NumberOfLineSegmentsAta > 1 && NumberOfLineSegmentsAta < 45)
	{
		glUniform4fv(LineColorUniformAta, 1, vmath::vec4(1.0f, 1.0f, 0.0f, 1.0f));
	}
	else if(NumberOfLineSegmentsAta>45 && NumberOfLineSegmentsAta <=50)
	{
		glUniform4fv(LineColorUniformAta, 1, vmath::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	}
	glPatchParameteri(GL_PATCH_VERTICES, 4);
	// Draw either by glDrawTriangles() or glDrawArrays() or glDrawElements()
	glDrawArrays(GL_PATCHES, 0, 4);

	glBindVertexArray(0);



	glUseProgram(0);

	SwapBuffers(ghdc);
}

void update(void)
{
	angleTriAta = angleTriAta + 0.1f;
	if (angleTriAta >= 360)
	{
		angleTriAta = 0.0f;
	}

	angleSqAta = angleSqAta + 0.1f;
	if (angleSqAta >= 360)
	{
		angleSqAta = 0.0f;
	}
}

void resize(int width, int height)
{
	//code
	if (height == 0)
		height = 1;
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);


	PerspectiveProjectionMatrixAta = perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);


}

void uninitialize(void)
{
	//code
	//GLuint ShaderProgramObjectAta;
	if (gbFullscreenAta == true)
	{
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);

		ShowCursor(TRUE);

	}



	if (Vbo_Position_VerLine)
	{
		glDeleteBuffers(1, &Vbo_Position_VerLine);
		Vbo_Position_VerLine = 0;
	}

	if (Vao_VerLine)
	{
		glDeleteVertexArrays(1, &Vao_VerLine);
		Vao_VerLine = 0;
	}


	if (ShaderProgramObjectAta)
	{
		GLsizei ShaderCount;
		glUseProgram(ShaderProgramObjectAta);
		glGetProgramiv(ShaderProgramObjectAta, GL_ATTACHED_SHADERS, &ShaderCount);

		GLuint *pShaders = (GLuint*)malloc((sizeof(GLuint)*ShaderCount));
		if (pShaders) {
			glGetAttachedShaders(ShaderProgramObjectAta, ShaderCount, &ShaderCount, pShaders);

			for (int ShaderNumber = 0; ShaderNumber < ShaderCount; ShaderNumber++)
			{
				glDetachShader(ShaderProgramObjectAta, pShaders[ShaderNumber]);
				glDeleteShader(pShaders[ShaderNumber]);
				pShaders[ShaderNumber] = 0;
			}
			free(pShaders);
		}

		glDeleteProgram(ShaderProgramObjectAta);
		ShaderProgramObjectAta = 0;
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
