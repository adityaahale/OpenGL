#include <windows.h>
#include <stdio.h> 
#include "vmath.h"
#include"interleave.h"
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
bool gbLight;

GLuint VertexShaderObjectAta;
GLuint FragmentShaderObjectAta;
GLuint ShaderProgramObjectAta;

GLuint VaoAta;
GLuint Vbo_PositionAta;
GLuint Vbo_ColorAta;


GLuint MVPUniformAta;
GLuint samplerUniformAta; //for uniform(dynamic) texture data
GLuint textureMarbleStoneAta;
GLuint modelUniformAta, viewUniformAta, projectionUniformAta;
GLuint lightPositionUniformAta;
GLuint lKeyPressUniformAta;
GLuint La_uniform, Ld_uniform, Ls_uniform;
GLuint Ka_uniform, Kd_uniform, Ks_uniform;
GLuint material_shininess_uniform;

mat4 PerspectiveProjectionMatrixAta;


GLfloat angleTriAta = 0.0f, angleSqAta = 0.0f;
int height, width;

GLfloat lightAmbient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat lightDiffuse[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat lightSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat lightPosition[] = { 500.0f, 500.0f, 500.0f, 1.0f };

GLfloat material_ambient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat material_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat material_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat material_shininess = 50.0f;


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
			update();
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
		case 0x4C:
			if (gbLight == false)
			{

				gbLight = true;
			}
			else
			{
				gbLight = false;
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
	BOOL LoadGLTextures(GLuint *, TCHAR[] );
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
		"#version 410 core" \
		"\n" \
		"in vec4 vPosition;" \
		"in vec3 vNormal;" \
		"in vec2 vTexture0_Coord;" \
		"out vec2 out_texture0_coord;" \
		"uniform mat4 u_model_matrix;" \
		"uniform mat4 u_view_matrix;" \
		"uniform mat4 u_projection_matrix;" \
		"uniform vec4 u_light_position;" \
		"uniform int u_lighting_enabled;" \
		"out vec3 transformed_normals;" \
		"out vec3 light_direction;" \
		"out vec3 viewer_vector;" \
		"in vec4 vColor;" \
		"out vec4 out_color;"
		"void main(void)" \
		"{" \
		"if(u_lighting_enabled == 1)" \
		"{" \
		"vec4 eye_coordinates = u_view_matrix* u_model_matrix * vPosition;" \
		"transformed_normals = mat3(u_view_matrix*u_model_matrix) * vNormal;" \
		"light_direction = vec3(u_light_position) - eye_coordinates.xyz;" \
		"viewer_vector = -eye_coordinates.xyz;" \
		"}" \
		"gl_Position = u_projection_matrix * u_view_matrix * u_model_matrix * vPosition;" \
		"out_color = vColor;" \
		"out_texture0_coord = vTexture0_Coord;" \
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


	//FRAGMENT SHADER
	FragmentShaderObjectAta = glCreateShader(GL_FRAGMENT_SHADER);

	const GLchar *fragmentShaderSourceCodeAta =
		"#version 430" \
		"\n" \
		"in vec3 transformed_normals;" \
		"in vec3 light_direction;" \
		"in vec3 viewer_vector;" \
		"in vec2 out_texture0_coord;" \
		"in vec4 out_color;"
		"uniform vec3 u_La;" \
		"uniform vec3 u_Ld;" \
		"uniform vec3 u_Ls;" \
		"uniform vec3 u_Ka;" \
		"uniform vec3 u_Kd;" \
		"uniform vec3 u_Ks;" \
		"uniform float u_material_shininess;" \
		"uniform int u_lighting_enabled;" \
		"uniform sampler2D u_texture0_sampler;" \
		"out vec4 FragColor;" \
		"void main(void)" \
		"{" \
		"vec3 phong_ads_color;"\
		"if(u_lighting_enabled == 1)" \
		"{" \
		"vec3 normalized_transformed_normals = normalize(transformed_normals);" \
		"vec3 normalized_light_direction = normalize(light_direction);" \
		"vec3 normalized_viewer_vector = normalize(viewer_vector);" \
		"float tn_dot_ld = max(dot(normalized_transformed_normals, normalized_light_direction), 0.0);" \
		"vec3 ambient = u_La * u_Ka;" \
		"vec3 diffuse = u_Ld * u_Kd * tn_dot_ld;" \
		"vec3 reflection_vector = reflect(-normalized_light_direction, normalized_transformed_normals);" \
		"vec3 specular = u_Ls * u_Ks * pow(max(dot(reflection_vector, normalized_viewer_vector),0.0), u_material_shininess);" \
		"phong_ads_color = ambient + diffuse + specular;" \
		"}" \
		"else" \
		"{" \
		"phong_ads_color = vec3(1.0, 1.0, 1.0);" \
		"}" \
		"FragColor = texture(u_texture0_sampler, out_texture0_coord) * out_color * vec4(phong_ads_color, 1.0);" \
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

	glAttachShader(ShaderProgramObjectAta, FragmentShaderObjectAta);

	glBindAttribLocation(ShaderProgramObjectAta, ATA_ATTRIBUTE_POSITION, "vPosition");
	glBindAttribLocation(ShaderProgramObjectAta, ATA_ATTRIBUTE_TEXTURE0, "vTexture0_Coord");
	glBindAttribLocation(ShaderProgramObjectAta, ATA_ATTRIBUTE_NORMAL, "vNormal");
	glBindAttribLocation(ShaderProgramObjectAta, ATA_ATTRIBUTE_COLOR, "vColor");


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

	//MVPUniformAta = glGetUniformLocation(ShaderProgramObjectAta, "u_mvp_matrix");
	samplerUniformAta = glGetUniformLocation(ShaderProgramObjectAta, "u_texture0_sampler");
	modelUniformAta = glGetUniformLocation(ShaderProgramObjectAta, "u_model_matrix");
	viewUniformAta = glGetUniformLocation(ShaderProgramObjectAta, "u_view_matrix");
	projectionUniformAta = glGetUniformLocation(ShaderProgramObjectAta, "u_projection_matrix");
	lKeyPressUniformAta = glGetUniformLocation(ShaderProgramObjectAta, "u_lighting_enabled"); //L/1 key pressed or not
	La_uniform = glGetUniformLocation(ShaderProgramObjectAta, "u_La");
	Ld_uniform = glGetUniformLocation(ShaderProgramObjectAta, "u_Ld");
	Ls_uniform = glGetUniformLocation(ShaderProgramObjectAta, "u_Ls");
	lightPositionUniformAta = glGetUniformLocation(ShaderProgramObjectAta, "u_light_position");

	//material ambient color intensity
	Ka_uniform = glGetUniformLocation(ShaderProgramObjectAta, "u_Ka");
	Kd_uniform = glGetUniformLocation(ShaderProgramObjectAta, "u_Kd");
	Ks_uniform = glGetUniformLocation(ShaderProgramObjectAta, "u_Ks");
	//shininess of material
	material_shininess_uniform = glGetUniformLocation(ShaderProgramObjectAta, "u_material_shininess");

	//Preparation for draw

	const GLfloat cubeData[] = {
		// Top face
		1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		-1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		-1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,

		// Bottom face
		1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
		-1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		-1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
		1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,

		// Front face
		1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		-1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
		-1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
		1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,

		// Back face
		1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f,
		-1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		-1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
		1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f,

		// Right face
		1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f,
		1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,

		// Left face
		-1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
		-1.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
		-1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f
	};
	//For Vao
	glGenVertexArrays(1, &VaoAta);
	glBindVertexArray(VaoAta);
	//vbo for positions
	glGenBuffers(1, &Vbo_PositionAta);
	glBindBuffer(GL_ARRAY_BUFFER, Vbo_PositionAta);
	glBufferData(GL_ARRAY_BUFFER, 24*11*sizeof(float), cubeData, GL_STATIC_DRAW);

	glVertexAttribPointer(ATA_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 11*sizeof(float), (void*)(0*sizeof(float)));
	glEnableVertexAttribArray(ATA_ATTRIBUTE_POSITION);

	glVertexAttribPointer(ATA_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(ATA_ATTRIBUTE_NORMAL);

	glVertexAttribPointer(ATA_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(ATA_ATTRIBUTE_COLOR);

	glVertexAttribPointer(ATA_ATTRIBUTE_TEXTURE0, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(9 * sizeof(float)));
	glEnableVertexAttribArray(ATA_ATTRIBUTE_TEXTURE0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);


	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
	glDisable(GL_CULL_FACE);
	glLineWidth(3.0f);
	//glDisable(GL_FRONT_FACE);
	LoadGLTextures(&textureMarbleStoneAta, MAKEINTRESOURCE(IDBITMAP_MARBLE));
	glEnable(GL_TEXTURE_2D);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	
	PerspectiveProjectionMatrixAta = mat4::identity();

	resize(WIN_WIDTH_ATA, WIN_HEIGHT_ATA);
}

BOOL LoadGLTextures(GLuint *texture, TCHAR ImgResourceID[])
{
	HBITMAP hBitmap;
	BITMAP bmp;
	BOOL bStatus = FALSE;

	//glGenTextures(1, texture);
	hBitmap = (HBITMAP)LoadImage(GetModuleHandle(NULL), ImgResourceID, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
	if (hBitmap)
	{
		bStatus = TRUE;
		GetObject(hBitmap, sizeof(bmp), &bmp);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
		glGenTextures(1, texture);
		glBindTexture(GL_TEXTURE_2D, *texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		//gluBuild2DMipmaps(GL_TEXTURE_2D, 3, bmp.bmWidth, bmp.bmHeight, GL_BGR_EXT, GL_UNSIGNED_BYTE, bmp.bmBits);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bmp.bmWidth, bmp.bmHeight, 0, GL_BGR, GL_UNSIGNED_BYTE, bmp.bmBits);
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);

		DeleteObject(hBitmap);

	}
	return (bStatus);
	glDeleteTextures(1, texture);
}

void display(void)
{
	//GLuint ShaderProgramObjectAta;
	//code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(ShaderProgramObjectAta);
	if (gbLight == true)
	{
		glUniform1i(lKeyPressUniformAta, 1);
		//setting light's properties
		glUniform3fv(La_uniform, 1, lightAmbient);
		glUniform3fv(Ld_uniform, 1, lightDiffuse);
		glUniform3fv(Ls_uniform, 1, lightSpecular);

		glUniform4fv(lightPositionUniformAta, 1, lightPosition);

		//set material properties
		glUniform3fv(Ka_uniform, 1, material_ambient);
		glUniform3fv(Kd_uniform, 1, material_diffuse);
		glUniform3fv(Ks_uniform, 1, material_specular);
		glUniform1f(material_shininess_uniform, material_shininess);
	}
	else 
	{
		glUniform1i(lKeyPressUniformAta, 0);
	}
	//mat4 modelViewMatrixAta;
	mat4 modelMatrixAta;
	mat4 viewMatrixAta;
	mat4 modelViewProjectionMatrixAta;
	mat4 rotationMatrixAta;
	mat4 translationMatrixAta;

	// OpenGL Drawing

	modelMatrixAta = mat4::identity();
	viewMatrixAta = mat4::identity();
	modelViewProjectionMatrixAta = mat4::identity();
	rotationMatrixAta = mat4::identity();
	translationMatrixAta = mat4::identity();

	
	translationMatrixAta = translate(0.0f, 0.0f, -7.0f);
	rotationMatrixAta = rotate(angleSqAta, 1.0f, 0.0f, 0.0f);
	rotationMatrixAta *= rotate(angleSqAta, 0.0f, 1.0f, 0.0f);
	rotationMatrixAta *= rotate(angleSqAta, 0.0f, 0.0f, 1.0f);
	modelMatrixAta = modelMatrixAta * translationMatrixAta * rotationMatrixAta;
	
	//modelViewProjectionMatrixAta = PerspectiveProjectionMatrixAta * modelViewMatrixAta;

	glUniformMatrix4fv(modelUniformAta, 1, GL_FALSE, modelMatrixAta);
	glUniformMatrix4fv(viewUniformAta, 1, GL_FALSE, viewMatrixAta);
	glUniformMatrix4fv(projectionUniformAta, 1, GL_FALSE, PerspectiveProjectionMatrixAta);
	
	glBindVertexArray(VaoAta);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureMarbleStoneAta);
	glUniform1i(samplerUniformAta, 0);

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4); 
	glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 16, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 20, 4);

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



	if (Vbo_PositionAta)
	{
		glDeleteBuffers(1, &Vbo_PositionAta);
		Vbo_PositionAta = 0;
	}

	if (Vbo_ColorAta)
	{
		glDeleteBuffers(1, &Vbo_ColorAta);
		Vbo_ColorAta = 0;
	}

	if (VaoAta)
	{
		glDeleteVertexArrays(1, &VaoAta);
		VaoAta = 0;
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
