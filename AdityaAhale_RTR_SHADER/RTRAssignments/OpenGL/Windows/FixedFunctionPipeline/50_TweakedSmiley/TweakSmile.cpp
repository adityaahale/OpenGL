#include<Windows.h>
#include<stdio.h>
#include<gl/GL.h>
#include<gl/GLU.h>
#include "smile.h"
#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glu32.lib")

#define WIN_WIDTH  800
#define WIN_HEIGHT 600

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
int initialize(void);
void display(void);
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
GLfloat Q1x=0.0f,Q1y=0.0f,Q2x=0.0f,Q2y=0.0f,Q3x=0.0f,Q3y=0.0,Q4x=0.0,Q4y=0.0;
GLuint Texture_Smile;
int pressKey = 0;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	MSG msg;
	TCHAR AppName[] = TEXT("OGL_3D");
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

	hwnd = CreateWindowEx(WS_EX_APPWINDOW, AppName, TEXT("Tweaked Smiley"), WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE, 0, 0, 800, 600, NULL, NULL, hInstance, NULL);

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
		case 0x31:
		case VK_NUMPAD1:
			pressKey = 1;
			break;

		case 0x32:
		case VK_NUMPAD2:
			pressKey = 2;
			break;

		case 0x33:
		case VK_NUMPAD3:
			pressKey = 3;
			break;

		case 0x34:
		case VK_NUMPAD4:
			pressKey = 4;
			//glDisable(GL_TEXTURE_2D);
			break;

		default:
			glDisable(GL_TEXTURE_2D);
			pressKey = 0;
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
	BOOL LoadGLTextures(GLuint *, TCHAR[]);
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
	LoadGLTextures(&Texture_Smile, MAKEINTRESOURCE(IDBITMAP_SMILE));
	
	//enable textures here
	glEnable(GL_TEXTURE_2D);

	resize(WIN_WIDTH, WIN_HEIGHT);
	return 0;
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

		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, bmp.bmWidth, bmp.bmHeight, GL_BGR_EXT, GL_UNSIGNED_BYTE, bmp.bmBits);

		DeleteObject(hBitmap);

	}
	return (bStatus);
	glDeleteTextures(1, texture);
}

void display(void)
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -6.0f);
	glBindTexture(GL_TEXTURE_2D, Texture_Smile);

	if (pressKey == 1) {
		glEnable(GL_TEXTURE_2D);
		Q1x = 1.0f, Q1y = 1.0f, Q2x = 0.0f, Q2y = 1.0f, Q4x = 1.0f, Q4y = 0.0f;
		glBegin(GL_QUADS);
		glTexCoord2f(Q1x, Q1y);
		glVertex3f(1.0f, 1.0f, 0.0f);
		glTexCoord2f(Q2x, Q2y);
		glVertex3f(-1.0f, 1.0f, 0.0f);
		glTexCoord2f(Q2x, Q3y);
		glVertex3f(-1.0f, -1.f, 0.0f);
		glTexCoord2f(Q4x, Q4y);
		glVertex3f(1.0f, -1.0f, 0.0f);
		glEnd();
		//pressKey = 0;
	}
	else if (pressKey == 2) {
		glEnable(GL_TEXTURE_2D);
		Q1x = 0.5f, Q1y = 0.5f, Q2x = 0.0f, Q2y = 0.5f, Q4x = 0.5f, Q4y = 0.0f;
		glBegin(GL_QUADS);
		glTexCoord2f(Q1x, Q1y);
		glVertex3f(1.0f, 1.0f, 0.0f);
		glTexCoord2f(Q2x, Q2y);
		glVertex3f(-1.0f, 1.0f, 0.0f);
		glTexCoord2f(Q2x, Q3y);
		glVertex3f(-1.0f, -1.f, 0.0f);
		glTexCoord2f(Q4x, Q4y);
		glVertex3f(1.0f, -1.0f, 0.0f);
		glEnd();
		//pressKey = 0;
	}
	else if (pressKey == 3)
	{
		glEnable(GL_TEXTURE_2D);
		Q1x = 2.0f, Q1y = 2.0f, Q2x = 0.0f, Q2y = 2.0f, Q4x = 2.0f, Q4y = 0.0f;
		glBegin(GL_QUADS);
		glTexCoord2f(Q1x, Q1y);
		glVertex3f(1.0f, 1.0f, 0.0f);
		glTexCoord2f(Q2x, Q2y);
		glVertex3f(-1.0f, 1.0f, 0.0f);
		glTexCoord2f(Q2x, Q3y);
		glVertex3f(-1.0f, -1.f, 0.0f);
		glTexCoord2f(Q4x, Q4y);
		glVertex3f(1.0f, -1.0f, 0.0f);
		glEnd();
		//pressKey = 0;
	}
	else if (pressKey == 4) {
		glEnable(GL_TEXTURE_2D);
		//Q1x = 0.5f, Q1y = 0.5f, Q2x = 0.5f, Q2y =0.5f,Q3x=0.5f,Q3y=0.5f,Q4x = 0.5f, Q4y = 0.5f;
		glBegin(GL_QUADS);
		glTexCoord2f(0.5f,0.5f);
		glVertex3f(1.0f, 1.0f, 0.0f);
		glTexCoord2f(0.5f, 0.5f);
		glVertex3f(-1.0f, 1.0f, 0.0f);
		glTexCoord2f(0.5f, 0.5f);
		glVertex3f(-1.0f, -1.f, 0.0f);
		glTexCoord2f(0.5f, 0.5f);
		glVertex3f(1.0f, -1.0f, 0.0f);
		glEnd();
		//glDisable(GL_TEXTURE_2D);
		//pressKey = 0;
	}
	else if(pressKey==0){
		glDisable(GL_TEXTURE_2D);
		glColor3f(1.0f, 1.0f, 1.0f);
		glBegin(GL_QUADS);
		glVertex3f(1.0f, 1.0f, 0.0f);
		glVertex3f(-1.0f, 1.0f, 0.0f);
		glVertex3f(-1.0f, -1.f, 0.0f);
		glVertex3f(1.0f, -1.0f, 0.0f);
		glEnd();
	}


	SwapBuffers(ghdc);
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
}