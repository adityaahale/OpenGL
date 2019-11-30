#include <windows.h>
#include <stdio.h> 
#include "vmath.h"
#include "Sphere.h"
#include <gl\glew.h>

#include <gl/GL.h>

using namespace vmath;

#pragma comment(lib,"glew32.lib")
#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"Sphere.lib")

#define WIN_WIDTH 800
#define WIN_HEIGHT 600



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
bool gbLighting = false;

float sphere_vertices[1146];
float sphere_normals[1146];
float sphere_textures[764];
unsigned short sphere_elements[2280];
unsigned int gNumVertices, gNumElements;

GLuint VertexShaderObject;
GLuint FragmentShaderObject;
GLuint ShaderProgramObject;

GLuint Vao_Pyramid;
GLuint Vao_Sphere;
GLuint Vbo_Position_Pyramid;
GLuint Vbo_Position_Sphere;
GLuint Vbo_Color_Pyramid;
GLuint Vbo_Normal_Sphere;
GLuint Vbo_sphere_element;
GLuint MUniform;
GLuint VUniform;
GLuint PUniform;
GLuint laUniform;
GLuint kaUniform;
GLuint ldUniform;
GLuint kdUniform;
GLuint lsUniform;
GLuint ksUniform;
GLuint materialShinyUniform;
GLuint lightPostionUniform;
GLuint lkeyPressedUniform;

float lightAmbient[] = { 0.0f,0.0f,0.0f,0.0f};
float lightDiffuse[] = { 1.0f,1.0f,1.0f,1.0f};
float lightSpecular[] = { 1.0f,1.0f,1.0f,1.0f};
float lightPosition[] = { 100.0f,100.0f,100.0f,1.0f };

float materialAmbient[] = { 0.0f,0.0f,0.0f,0.0f};
float materialDiffuse[] = { 1.0f,1.0f,1.0f,1.0f};
float materialSpecular[] = { 1.0f,1.0f,1.0f,1.0f};
float materialShininess = 50.0f;


mat4 PerspectiveProjectionMatrix;
mat4 modelMatrix;
mat4 viewMatrix;

GLfloat anglePy = 0.0f, angleCu = 0.0f;

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
		TEXT("OpenGL 3D Rotating Shapes"),
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

		case 0x4C:
			if (gbLighting == false)
			{

				gbLighting = true;
			}
			else
			{
				gbLighting = false;
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
		"in vec3 vNormal;" \
		"uniform mat4 u_m_matrix;" \
		"uniform mat4 u_v_matrix;" \
		"uniform mat4 u_projection_matrix;" \
		"uniform int u_lkeypress;" \
		"uniform vec3 u_la;" \
		"uniform vec3 u_ld;" \
		"uniform vec3 u_ls;" \
		"uniform vec3 u_ka;" \
		"uniform vec3 u_kd;" \
		"uniform vec3 u_ks;" \
		"uniform float u_material_shin;" \
		"uniform vec4 u_light_position;" \
		"out vec3 phong_ads_light;" \
		"void main(void)" \
		"{" \

		"if (u_lkeypress == 1)" \
		"{" \
		"vec4 eye_coordinate = u_v_matrix * u_m_matrix * vPosition;" \
		"vec3 tnorm = normalize(mat3(u_v_matrix * u_m_matrix) * vNormal);" \
		"vec3 light_direction = normalize(vec3(u_light_position - eye_coordinate));" \
		"float tn_dot_lightd = max(dot(light_direction,tnorm),0.0);" \
		"vec3 reflection_vector=reflect(-light_direction, tnorm);" \
		"vec3 viewer_vector=normalize(vec3(-eye_coordinate.xyz));" \
		"vec3 ambient = u_la * u_ka;" \
		"vec3 diffuse = u_ld * u_kd * tn_dot_lightd;" \
		"vec3 specular = u_ls * u_ks * pow(max(dot(reflection_vector,viewer_vector),0.0),u_material_shin);" \
		"phong_ads_light = ambient + diffuse + specular;" \
		"}" \

		"else" \
		"{" \
		"phong_ads_light = vec3(1.0,1.0,1.0);" \
		"}" \

		"gl_Position = u_projection_matrix * u_v_matrix * u_m_matrix * vPosition;" \
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
		"in vec3 phong_ads_light;" \
		"out vec4 FragColor;" \
		"void main(void)" \
		"{" \
		"FragColor = vec4(phong_ads_light,1.0);" \
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
	glBindAttribLocation(ShaderProgramObject, AMC_ATTRIBUTE_NORMAL, "vNormal");

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


	MUniform = glGetUniformLocation(ShaderProgramObject, "u_m_matrix");
	VUniform = glGetUniformLocation(ShaderProgramObject, "u_v_matrix");
	PUniform = glGetUniformLocation(ShaderProgramObject, "u_projection_matrix");
	laUniform = glGetUniformLocation(ShaderProgramObject, "u_la");
	kaUniform = glGetUniformLocation(ShaderProgramObject, "u_ka");	
	ldUniform = glGetUniformLocation(ShaderProgramObject, "u_ld");
	kdUniform = glGetUniformLocation(ShaderProgramObject, "u_kd");
	lsUniform = glGetUniformLocation(ShaderProgramObject, "u_ls");
	ksUniform = glGetUniformLocation(ShaderProgramObject, "u_ks");
	materialShinyUniform= glGetUniformLocation(ShaderProgramObject, "u_material_shin");
	lightPostionUniform = glGetUniformLocation(ShaderProgramObject, "u_light_position");
	lkeyPressedUniform = glGetUniformLocation(ShaderProgramObject, "u_lkeypress");


	getSphereVertexData(sphere_vertices, sphere_normals, sphere_textures, sphere_elements);
	gNumVertices = getNumberOfSphereVertices();
	gNumElements = getNumberOfSphereElements();


	//For Sphere
	glGenVertexArrays(1, &Vao_Sphere);
	glBindVertexArray(Vao_Sphere);

	glGenBuffers(1, &Vbo_Position_Sphere);
	glBindBuffer(GL_ARRAY_BUFFER, Vbo_Position_Sphere);
	glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_vertices), sphere_vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//sphere normals
	glGenBuffers(1, &Vbo_Normal_Sphere);
	glBindBuffer(GL_ARRAY_BUFFER, Vbo_Normal_Sphere);
	glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_normals), sphere_normals, GL_STATIC_DRAW);

	glVertexAttribPointer(AMC_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(AMC_ATTRIBUTE_NORMAL);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//element vbo
	glGenBuffers(1, &Vbo_sphere_element);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Vbo_sphere_element);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sphere_elements), sphere_elements, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	//glEnable(GL_CULL_FACE);
	glDisable(GL_CULL_FACE);
	//glDisable(GL_FRONT_FACE);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	PerspectiveProjectionMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	modelMatrix = mat4::identity();

	resize(WIN_WIDTH, WIN_HEIGHT);
}

void display(void)
{
	//GLuint ShaderProgramObject;
	//code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(ShaderProgramObject);


	mat4 rotX, rotY, rotZ, rotationMatrix;
	mat4 translationMatrix;
	mat4 scaleMatrix;
	// OpenGL Drawing

	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	rotX = mat4::identity();
	rotY = mat4::identity();
	rotZ = mat4::identity();
	translationMatrix = mat4::identity();
	rotationMatrix = mat4::identity();
	scaleMatrix = mat4::identity();

	translationMatrix = translate(0.0f, 0.0f, -3.0f);
	rotationMatrix = rotate(angleCu, 0.0f, 1.0f, 0.0f);
	//scaleMatrix = scale(0.75f, 0.75f, 0.75f);
	modelMatrix = translationMatrix;

	
	
	if (gbLighting == true)
	{
		//GLfloat lightpos[] = { 1.0f, -1.0f, 2.0f, 1.0f };
		glUniform1i(lkeyPressedUniform, 1);
		
		glUniform3fv(laUniform, 1,lightAmbient);
		glUniform3fv(kaUniform, 1, materialAmbient);
		glUniform3fv(ldUniform, 1,lightDiffuse);
		glUniform3fv(kdUniform, 1, materialDiffuse);
		glUniform3fv(lsUniform, 1,lightSpecular);
		glUniform3fv(ksUniform, 1, materialSpecular);
		
		glUniform4fv(lightPostionUniform,1, lightPosition);
		glUniform1f(materialShinyUniform, materialShininess);
	}
	else
	{
		glUniform1i(lkeyPressedUniform, 0);
	}

	glUniformMatrix4fv(MUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(VUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(PUniform, 1, GL_FALSE, PerspectiveProjectionMatrix);

	glBindVertexArray(Vao_Sphere);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Vbo_sphere_element);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);

	glBindVertexArray(0);

	glUseProgram(0);

	SwapBuffers(ghdc);
}

void update(void)
{
	anglePy = anglePy + 0.1f;
	if (anglePy >= 360)
	{
		anglePy = 0.0f;
	}

	angleCu = angleCu + 0.1f;
	if (angleCu >= 360)
	{
		angleCu = 0.0f;
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

	if (Vbo_Color_Pyramid)
	{
		glDeleteBuffers(1, &Vbo_Color_Pyramid);
		Vbo_Color_Pyramid = 0;
	}


	if (Vbo_Position_Pyramid)
	{
		glDeleteBuffers(1, &Vbo_Position_Pyramid);
		Vbo_Position_Pyramid = 0;
	}

	if (Vao_Pyramid)
	{
		glDeleteVertexArrays(1, &Vao_Pyramid);
		Vao_Pyramid = 0;
	}

	if (Vbo_Color_Pyramid)
	{
		glDeleteBuffers(1, &Vbo_Color_Pyramid);
		Vbo_Color_Pyramid = 0;
	}

	if (Vbo_Position_Sphere)
	{
		glDeleteBuffers(1, &Vbo_Position_Sphere);
		Vbo_Position_Sphere = 0;
	}

	if (Vbo_sphere_element)
	{
		glDeleteBuffers(1, &Vbo_sphere_element);
		Vbo_sphere_element = 0;
	}

	if (Vao_Sphere)
	{
		glDeleteVertexArrays(1, &Vao_Sphere);
		Vao_Sphere = 0;
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
