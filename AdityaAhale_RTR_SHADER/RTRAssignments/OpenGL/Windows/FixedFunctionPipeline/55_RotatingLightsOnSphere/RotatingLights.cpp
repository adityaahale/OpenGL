#include<Windows.h>
#include<stdio.h>
#include<gl/GL.h>
#include<gl/GLU.h>

#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glu32.lib")

#define WIN_WIDTH  800
#define WIN_HEIGHT 600
#define LENGTH 4

struct MyLight
{
	GLfloat LightAmbient[LENGTH];
	GLfloat LightDiffuse[LENGTH];
	GLfloat LightSpecular[LENGTH];
	GLfloat LightPosition[LENGTH];
	GLfloat lightAngle0;
	GLfloat lightAngle1;
	GLfloat lightAngle2;
};

struct MyLight mylights[3];

bool bLight = false;
GLfloat LightAmbientzero[] = { 0.0f,0.0f,0.0f,1.0f };
GLfloat LightDiffusezero[] = { 1.0f,0.0f, 0.0f,1.0f };
GLfloat LightPositionzero[] = { 0.0f,0.0f,0.0f,1.0f };
GLfloat LightSpecularzero[] = { 1.0f,0.0f,0.0f,1.0f };

GLfloat LightAmbientone[] = { 0.0f,0.0f,0.0f,1.0f };
GLfloat LightDiffuseone[] = { 0.0f,1.0f,0.0f,1.0f };
GLfloat LightPositionone[] = { 0.0f,0.0f,0.0f,1.0f };
GLfloat LightSpecularone[] = { 0.0f,1.0f,0.0f,1.0f };

GLfloat LightAmbienttwo[] = { 0.0f,0.0f,0.0f,1.0f };
GLfloat LightDiffusetwo[] = { 0.0f,0.0f,1.0f,1.0f };
GLfloat LightPositiontwo[] = { 0.0f,0.0f,0.0f,1.0f };
GLfloat LightSpeculartwo[] = { 0.0f,0.0f,1.0f,1.0f };

//mylights[1].LightAmbient=LightAmbientzero;

GLfloat MaterialAmbient[] = { 0.2f,0.2f,0.2f,1.0f };
GLfloat MaterialDiffuse[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat MaterialShininess[] = { 128.0f };
GLfloat MaterialSpecular[] = { 1.0f,1.0f,1.0f,1.0f };



LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
int initialize(void);
void display(void);
void spin(void);
int height, width;
HWND ghwnd;
bool bDone = false;
bool gbActiveWindow = false;
bool gbEscKeyPressed = false;
bool gbFullScreen = false;
DWORD dwStyle;
WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };
HDC ghdc;
HGLRC ghrc = NULL;
FILE *gpFile = NULL;
int count;

GLfloat tri = 0.0f;
GLUquadric *quadric = NULL;


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	MSG msg;
	TCHAR AppName[] = TEXT("PyramidLight");
	WNDCLASSEX wndclass;
	HWND hwnd;
	int iRet = 0;
	if (fopen_s(&gpFile, "log.txt", "w") != 0)
	{
		MessageBox(NULL, TEXT("log file cannot created"), TEXT("Error"), MB_OK);
		exit(0);
	}
	else {
		fprintf(gpFile, "log file created");
	}
	wndclass.cbClsExtra = 0;
	wndclass.cbSize = sizeof(wndclass);
	wndclass.cbWndExtra = 0;
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hInstance = hInstance;
	wndclass.lpfnWndProc = WndProc;
	wndclass.lpszClassName = AppName;
	wndclass.lpszMenuName = 0;
	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;

	RegisterClassEx(&wndclass);

	hwnd = CreateWindowEx(WS_EX_APPWINDOW, AppName, TEXT("OGL_Rotation"), WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE, 0, 0, 800, 600, NULL, NULL, hInstance, NULL);

	ghwnd = hwnd;

	iRet = initialize();
	if (iRet == -1)
	{
		fprintf(gpFile, "Choose Pixel Format Failed");
		DestroyWindow(hwnd);
	}
	else if (iRet == -2)
	{
		fprintf(gpFile, "Set Pixel Format Failed");
		DestroyWindow(hwnd);
	}
	else if (iRet == -3)
	{
		fprintf(gpFile, "WGL Created Context Failed");
		DestroyWindow(hwnd);
	}
	else if (iRet == -4)
	{
		fprintf(gpFile, "WGL Make Current Failed");
		DestroyWindow(hwnd);
	}
	else {
		fprintf(gpFile, "Initialization Succeded");
	}
	ShowWindow(hwnd, SW_SHOW);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);

	while (bDone == false)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
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
			if (gbActiveWindow == true)
			{
				if (gbEscKeyPressed == true)
					bDone = true;
			}
			display();
			spin();
		}
	}

}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	//void display(void);
	void resize(int, int);
	void ToggleScreen(void);
	void uninitialize(void);

	switch (iMsg)
	{
	case WM_ACTIVATE:
		if (HIWORD(wParam) == 0)
			gbActiveWindow = true;
		else
			gbActiveWindow = false;
		break;
	case WM_ERASEBKGND:
		return (0);
		break;
	case WM_SIZE:
		resize(LOWORD(lParam), HIWORD(lParam));
		break;
	case WM_KEYDOWN:

		switch (wParam)
		{
		case VK_ESCAPE:
			if (gbEscKeyPressed == false)
				gbEscKeyPressed = true;
			break;
		case 0x46:
			if (gbFullScreen == false)
			{
				ToggleScreen();
				gbFullScreen = true;
			}
			else
			{
				ToggleScreen();
				gbFullScreen = false;
			}
			break;
		case 0x4C:
			if (bLight == false)
			{
				bLight = true;
				glEnable(GL_LIGHTING);
			}
			else
			{
				bLight = false;
				glDisable(GL_LIGHTING);
			}
			break;
		default:

			break;
		}
	case WM_LBUTTONDOWN:
		break;
	case WM_SETFOCUS:
		gbActiveWindow = true;
		break;
	case WM_KILLFOCUS:
		gbActiveWindow = true;
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

void ToggleScreen(void)
{
	MONITORINFO mi;

	if (gbFullScreen == false)
	{
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
		if (dwStyle & WS_OVERLAPPEDWINDOW)
		{
			mi.cbSize = sizeof(MONITORINFO);

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
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);
		ShowCursor(TRUE);
	}
}

int initialize(void)
{
	void resize(int, int);

	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex;

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
		return -1;
	}
	if (SetPixelFormat(ghdc, iPixelFormatIndex, &pfd) == false)
	{
		return -2;
	}

	ghrc = wglCreateContext(ghdc);
	if (ghrc == NULL)
	{
		return -3;
	}

	if (wglMakeCurrent(ghdc, ghrc) == false)
	{
		return -4;
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	glShadeModel(GL_SMOOTH);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	for (int i = 0; i < LENGTH; i++) {
		mylights[0].LightAmbient[i] = LightAmbientzero[i];
		mylights[0].LightDiffuse[i] = LightDiffusezero[i];
		mylights[0].LightPosition[i] = LightPositionzero[i];
		mylights[0].LightSpecular[i] = LightSpecularzero[i];
		mylights[1].LightAmbient[i] = LightAmbientone[i];
		mylights[1].LightDiffuse[i] = LightDiffuseone[i];
		mylights[1].LightPosition[i] = LightPositionone[i];
		mylights[1].LightSpecular[i] = LightSpecularone[i];
		mylights[2].LightAmbient[i] = LightAmbienttwo[i];
		mylights[2].LightDiffuse[i] = LightDiffusetwo[i];
		mylights[2].LightPosition[i] = LightPositiontwo[i];
		mylights[2].LightSpecular[i] = LightSpeculartwo[i];

	}
	mylights[0].lightAngle0 = 0.0f;
	mylights[1].lightAngle0 = 0.0f;
	mylights[2].lightAngle0 = 0.0f;


	glLightfv(GL_LIGHT0, GL_AMBIENT, mylights[0].LightAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, mylights[0].LightDiffuse);
	//glLightfv(GL_LIGHT0, GL_POSITION, mylights[0].LightPosition);
	glLightfv(GL_LIGHT0, GL_SPECULAR, mylights[0].LightSpecular);

	glLightfv(GL_LIGHT1, GL_AMBIENT, mylights[1].LightAmbient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, mylights[1].LightDiffuse);
	//glLightfv(GL_LIGHT1, GL_POSITION, mylights[1].LightPosition);
	glLightfv(GL_LIGHT1, GL_SPECULAR, mylights[1].LightSpecular);

	glLightfv(GL_LIGHT2, GL_AMBIENT, mylights[2].LightAmbient);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, mylights[2].LightDiffuse);
	//glLightfv(GL_LIGHT2, GL_POSITION, mylights[2].LightPosition);
	glLightfv(GL_LIGHT2, GL_SPECULAR, mylights[2].LightSpecular);


	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glEnable(GL_LIGHT2);

	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, MaterialShininess);
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	resize(WIN_WIDTH, WIN_HEIGHT);
	return 0;
}

void display(void)
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glMatrixMode(GL_MODELVIEW);
	
	glLoadIdentity();
	
	glPushMatrix();
	gluLookAt(0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	
	//red
	glPushMatrix();
	glRotatef(mylights[0].lightAngle0, 1.0f, 0.0f, 0.0f);
	mylights[0].LightPosition[1] = mylights[0].lightAngle0;
	glLightfv(GL_LIGHT0, GL_POSITION, mylights[0].LightPosition);
	glPopMatrix();
	
	//green
	glPushMatrix();
	glRotatef(mylights[1].lightAngle1, 0.0f, 1.0f, 0.0f);
	mylights[1].LightPosition[0] = mylights[1].lightAngle1;
	glLightfv(GL_LIGHT1, GL_POSITION, mylights[1].LightPosition);
	glPopMatrix();

	//blue
	glPushMatrix();
	glRotatef(mylights[2].lightAngle2, 0.0f, 0.0f, 1.0f);
	mylights[2].LightPosition[0] = mylights[2].lightAngle2;
	glLightfv(GL_LIGHT2, GL_POSITION, mylights[2].LightPosition);
	glPopMatrix();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	quadric = gluNewQuadric();
	gluSphere(quadric, 0.2, 100, 100);
	glPopMatrix();





	SwapBuffers(ghdc);
}


void spin(void)
{
	tri = tri + 0.1f;

	if (tri >= 360.0f)
	{
		tri = 0.0f;
	}

	mylights[0].lightAngle0 = mylights[0].lightAngle0 + 0.5f;
	if (mylights[0].lightAngle0 > 360)
	{
		mylights[0].lightAngle0 = 0.0f;
	}
	
	mylights[1].lightAngle1 = mylights[1].lightAngle1 + 0.5f;
	if (mylights[1].lightAngle1 > 360)
	{
		mylights[1].lightAngle1 = 0.0f;
	}

	mylights[2].lightAngle2 = mylights[2].lightAngle2 + 0.5f;
	if (mylights[2].lightAngle2 > 360)
	{
		mylights[2].lightAngle2 = 0.0f;
	}
}

void resize(int width, int height)
{
	if (height == 0)
	{
		height = 1;
	}

	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void uninitialize()
{
	if (gbFullScreen == true)
	{
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);
		ShowCursor(TRUE);
	}
	if (wglGetCurrentContext() == ghrc)
	{
		wglMakeCurrent(NULL, NULL);
	}
	if (ghrc)
	{
		wglDeleteContext(ghrc);
		ghrc = NULL;
	}
	if (ghdc)
	{
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}
	if (gpFile)
	{
		fprintf(gpFile, "File Close");
		fclose(gpFile);
		gpFile = NULL;
	}
	if (quadric)
	{
		gluDeleteQuadric(quadric);
		quadric = NULL;
	}
}
