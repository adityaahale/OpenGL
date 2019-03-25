#include<Windows.h>
#include<stdio.h>
#include<gl/GL.h>
#include<gl/GLU.h>
#define _USED_MATH_DEFINES 1
#include<math.h>
#include"Header.h"

#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glu32.lib")

#define WIN_WIDTH  800
#define WIN_HEIGHT 600
#define PI 3.141592653

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
int initialize(void);
void display(void);
void spin(void);
void translatemy(void);
void DrawI(void);
void DrawN(void);
void DrawD(void);
void DrawA(void);
void TriLine(void);
void PlaneBody(void);
void PlaneWings(void);
void smoke(void);
void SmokeAnimation(void);
void IAF(void);

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
const int numPoints = 1000;
GLfloat rot = 0.0f, itrans = -8.0f, ntrans = 6.5f, iitrans = -6.5f, atrans = 8.0;
GLfloat xOrColor = 0.0f, yOrColor = 0.0f, zOrColor = 0.0f, xGrColor = 0.0f, yGrColor = 0.0f, zGrColor = 0.0f;
GLfloat xTritrans = -6.5f, Topangle = 0.0f, Bottomangle = 0.0f, xIAFtrans = -8.3f,
SOrange[3] = { 1.0f, 0.6f, 0.2f },
EOrange[3] = { 1.0f, 0.6f, 0.2f },
SWhite[3] = { 1.0f,1.0f,1.0f },
EWhite[3] = { 1.0f,1.0f,1.0f },
SGreen[3] = { 0.07058f, 0.54f, 0.0274f },
EGreen[3] = { 0.07058f, 0.54f, 0.0274f },
xPlanetrans = -8.3f, xPlane1trans = -8.3f, xPlane2trans = -8.3f, yPlane1trans = 2.0, yPlane2trans = -2.0;
int gbTristable = 0;

float colourArray[100][3] = { 1.0f,0.0f,0.0f,
							0.0f,1.0f,0.0f,
							0.0f,0.0f,1.0f,
							1.0f,1.0f,0.0f,
							0.0f,1.0f,1.0f,
							1.0f,0.0f,1.0f,
							0.5f,0.5f,0.5f,
							1.0f,1.0f,1.0f,
							1.0f,0.5f,0.0f,
							0.25f,0.75f,0.5f };
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	MSG msg;
	TCHAR AppName[] = TEXT("OGL_Graph");
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

	hwnd = CreateWindowEx(WS_EX_APPWINDOW, AppName, TEXT("OGL_Lines"), WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE, 0, 0, 800, 600, NULL, NULL, hInstance, NULL);

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
			//spin();
			translatemy();
		}
	}

}


LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	//void display(void);
	void resize(int, int);
	void ToggleScreen(void);
	void uninitialize(void);
	//PlaySound(MAKEINTRESOURCE(IDW_DC),NULL,SND_RESOURCE|SND_ASYNC|SND_NODEFAULT);
	//PlaySound(TEXT("PURAB AUR PACHHIM- Dulhan chali01.wav"),NULL,SND_FILENAME|SND_ASYNC|SND_NODEFAULT);
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
	//void resize(int, int);
	PlaySound(MAKEINTRESOURCE(IDW_DC), NULL, SND_RESOURCE | SND_ASYNC | SND_NODEFAULT);
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
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	ToggleScreen();
	//resize(WIN_WIDTH, WIN_HEIGHT);
	return 0;
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);

	//smoke
	glLoadIdentity();
	glTranslatef(xTritrans, 0.0f, -6.0f);//x=2.0f
	glLineWidth(3.0f);
	smoke();

	//IIIIIIIIIIIIII
	glLoadIdentity();
	glTranslatef(itrans, 0.0f, -6.0f);
	//glTranslatef(-2.5f, 0.0f, -6.0f);
	glLineWidth(4.0f);
	DrawI();

	//AAAAAAAAAAAAAA
	glLoadIdentity();
	glTranslatef(atrans, 0.0f, -6.0f);
	//glTranslatef(2.0f, 0.0f, -6.0f);
	glLineWidth(4.0f);
	DrawA();

	//NNNNNNNNNNNNNNNNNNN
	glLoadIdentity();
	glTranslatef(-1.5f, ntrans, -6.0f);
	//glTranslatef(-1.5f, 0.0f, -6.0f);
	glLineWidth(4.0f);
	DrawN();


	//2nd IIIIIIIIIIIIIIIIIII
	glLoadIdentity();
	glTranslatef(1.0f, iitrans, -6.0f);
	glLineWidth(4.0f);
	//glBegin(GL_LINES);
	DrawI();

	//DDDDDDDDDDDDDDDDDD

	glLoadIdentity();
	glTranslatef(0.2f, 0.0f, -6.0f);
	glLineWidth(4.0f);
	//glBegin(GL_LINES);
	DrawD();


	//TriFlag
	glLoadIdentity();
	glTranslatef(xTritrans, 0.0f, -6.0f);//x=2.0f
	glLineWidth(3.0f);
	TriLine();

	//glEnd();
	//Plane
	glLoadIdentity();
	glTranslatef(xPlanetrans, 0.078f, -8.0f);
	PlaneBody();
	glLoadIdentity();
	glTranslatef(xPlanetrans, 0.078f, -8.0f);
	PlaneWings();
	glLoadIdentity();
	glTranslatef(xIAFtrans, 0.078, -8.0f);
	glLineWidth(1.0f);
	IAF();


	//TopPlane
	glLoadIdentity();
	glTranslatef(xPlane1trans, yPlane1trans, -8.0f);
	glRotatef(Topangle, 1.0f, 1.0f, 1.0f);
	PlaneBody();
	glLoadIdentity();
	glTranslatef(xPlane1trans, yPlane1trans, -8.0f);
	glRotatef(Topangle, 1.0f, 1.0f, 1.0f);
	PlaneWings();
	glLoadIdentity();
	glTranslatef(xPlane1trans, yPlane1trans, -8.0f);
	glRotatef(Topangle, 1.0f, 1.0f, 1.0f);
	glLineWidth(1.0f);
	IAF();

	//BottomPlane
	glLoadIdentity();
	glTranslatef(xPlane2trans, yPlane2trans, -8.0f);
	glRotatef(Bottomangle, 1.0f, 1.0f, 1.0f);
	PlaneBody();
	glLoadIdentity();
	glTranslatef(xPlane2trans, yPlane2trans, -8.0f);
	glRotatef(Bottomangle, 1.0f, 1.0f, 1.0f);
	PlaneWings();
	glLoadIdentity();
	glTranslatef(xPlane2trans, yPlane2trans, -8.0f);
	glRotatef(Bottomangle, 1.0f, 1.0f, 1.0f);
	glLineWidth(1.0f);
	IAF();



	SwapBuffers(ghdc);
}
void translatemy(void)
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
		//1.0,0.6,0.2--orange		0.07058,0.54,0.0274--green
		if (xOrColor < 1.0)
		{
			//xOrColor = xOrColor + 0.08;
			xOrColor = xOrColor + 0.002;
		}
		if (yOrColor < 0.6)
		{
			//yOrColor = yOrColor + 0.08;
			yOrColor = yOrColor + 0.002;
		}
		if (zOrColor < 0.2)
		{
			zOrColor = zOrColor + 0.002;
			//zOrColor = zOrColor + 0.08;
		}

		if (xGrColor < 0.07058)
		{
			//xGrColor = xGrColor + 0.08;
			xGrColor = xGrColor + 0.002;
		}

		if (yGrColor < 0.54)
		{
			//yGrColor = yGrColor + 0.08;
			yGrColor = yGrColor + 0.002;
		}

		if (zGrColor < 0.0274)
		{
			//zGrColor = zGrColor + 0.08;
			zGrColor = zGrColor + 0.002;
		}
	}

	if (xOrColor >= 1.0)
	{

		if (xTritrans < 2.0)
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

		//}
		/*if (Bottomangle < 0.0)
		{

		}*/

	}
	SmokeAnimation();

}
void spin(void)
{
	rot = rot + 0.1f;
	if (rot >= 360)
	{
		rot = 0.0f;
	}
}

void SmokeAnimation(void)
{
	if (xTritrans > 1.5)
	{
		EOrange[0] = EOrange[0] - 0.006;
		EOrange[1] = EOrange[1] - 0.006;
		EOrange[2] = EOrange[2] - 0.006;
		if (EOrange[2] < 0.0)
		{
			SOrange[0] = SOrange[0] - 0.007;
			SOrange[1] = SOrange[1] - 0.007;
			SOrange[2] = SOrange[2] - 0.007;

		}
	}

	if (xTritrans > 1.5)
	{
		EWhite[0] = EWhite[0] - 0.0065;
		EWhite[1] = EWhite[1] - 0.0065;
		EWhite[2] = EWhite[2] - 0.0065;
		if (EWhite[2] < 0.0)
		{
			SWhite[0] = SWhite[0] - 0.007;
			SWhite[1] = SWhite[1] - 0.007;
			SWhite[2] = SWhite[2] - 0.007;

		}
	}

	if (xTritrans > 1.5)
	{
		EGreen[0] = EGreen[0] - 0.006;
		EGreen[1] = EGreen[1] - 0.006;
		EGreen[2] = EGreen[2] - 0.006;
		if (EGreen[2] < 0.0)
		{
			SGreen[0] = SGreen[0] - 0.007;
			SGreen[1] = SGreen[1] - 0.007;
			SGreen[2] = SGreen[2] - 0.007;

		}
	}
}

void DrawI(void)
{
	glBegin(GL_LINES);
	glColor3f(1.0f, 0.6f, 0.2f);
	glVertex2f(0.0f, 1.0f);
	glColor3f(0.07058f, 0.54f, 0.0274f);
	glVertex2f(0.0f, -1.0f);
	glEnd();
}
void DrawN(void) {
	glBegin(GL_LINES);
	glColor3f(1.0f, 0.6f, 0.2f);
	glVertex2f(-0.5f, 1.0f);
	glColor3f(0.07058f, 0.54f, 0.0274f);
	glVertex2f(-0.5f, -1.0f);
	glColor3f(1.0f, 0.6f, 0.2f);
	glVertex2f(-0.5f, 1.0f);
	glColor3f(0.07058f, 0.54f, 0.0274f);
	glVertex2f(0.5f, -1.0f);
	glColor3f(0.07058f, 0.54f, 0.0274f);
	glVertex2f(0.5f, -1.0f);
	glColor3f(1.0f, 0.6f, 0.2f);
	glVertex2f(0.5f, 1.0f);
	glEnd();
}
void DrawD(void) {
	glBegin(GL_LINES);
	glColor3f(xOrColor, yOrColor, zOrColor);
	glVertex2f(-0.5f, 1.0f);
	glColor3f(xGrColor, yGrColor, zGrColor);
	glVertex2f(-0.5f, -1.0f);
	glColor3f(xOrColor, yOrColor, zOrColor);
	glVertex2f(-0.7f, 1.0f);
	glVertex2f(0.3f, 1.0f);
	glColor3f(xGrColor, yGrColor, zGrColor);
	//glColor3f(0.07058f, 0.54f, 0.0274f);
	glVertex2f(-0.7f, -1.0f);
	glVertex2f(0.3, -1.0f);
	glColor3f(xOrColor, yOrColor, zOrColor);
	glVertex2f(0.3f, 1.0f);
	glColor3f(xGrColor, yGrColor, zGrColor);
	glVertex2f(0.3f, -1.0f);
	glEnd();
}
void DrawA(void) {
	glBegin(GL_LINES);
	glColor3f(1.0f, 0.6f, 0.2f);
	glVertex2f(0.0f, 1.0f);
	glColor3f(0.07058f, 0.54f, 0.0274f);
	glVertex2f(-0.5f, -1.0f);
	glColor3f(1.0f, 0.6f, 0.2f);
	glVertex2f(0.0f, 1.0f);
	glColor3f(0.07058f, 0.54f, 0.0274f);
	glVertex2f(0.5f, -1.0f);
	glEnd();

}

void TriLine(void)
{
	glBegin(GL_LINES);
	glColor3f(1.0f, 0.6f, 0.2f);
	glVertex2f(-0.23, 0.08f);
	glVertex2f(0.23, 0.08f);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(-0.23, 0.05f);
	glVertex2f(0.23, 0.05f);
	glColor3f(0.07058f, 0.54f, 0.0274f);
	glVertex2f(-0.23, 0.02f);
	glVertex2f(0.23, 0.02f);
	glEnd();
}

void smoke(void)
{
	glBegin(GL_LINES);
	glColor3f(SOrange[0], SOrange[1], SOrange[2]);
	glVertex2f(-0.23, 0.08f);

	glColor3f(EOrange[0], EOrange[1], EOrange[2]);
	glVertex2f(-4.0, 0.08f);

	glColor3f(SWhite[0], SWhite[1], SWhite[2]);
	glVertex2f(-0.23f, 0.05f);

	glColor3f(EWhite[0], EWhite[1], EWhite[2]);
	glVertex2f(-4.0f, 0.05f);

	glColor3f(SGreen[0], SGreen[1], SGreen[2]);
	glVertex2f(-0.23f, 0.02f);

	glColor3f(EGreen[0], EGreen[1], EGreen[2]);
	glVertex2f(-4.0f, 0.02f);

	glEnd();
}

void PlaneBody(void) {

	glBegin(GL_QUADS);
	glColor3f((float)186 / 255, (float)226 / 255, (float)238 / 255);
	glVertex2f(-0.2, 0.05f);
	glVertex2f(-0.2, -0.05f);
	glVertex2f(0.4, -0.05f);
	glVertex2f(0.4, 0.05f);
	glEnd();
}
void PlaneWings(void)
{
	glBegin(GL_TRIANGLES);

	glVertex2f(0.5f, 0.0f);
	glVertex2f(0.4f, 0.05f);
	glVertex2f(0.4f, -0.05f);


	glEnd();

	glBegin(GL_TRIANGLES);

	glVertex2f(0.2f, 0.05f);
	glVertex2f(-0.3f, 0.4f);
	glVertex2f(0.0f, 0.05f);


	glEnd();

	glBegin(GL_TRIANGLES);

	glVertex2f(0.2f, -0.05f);
	glVertex2f(-0.3f, -0.4f);
	glVertex2f(0.0f, -0.05f);


	glEnd();

	glBegin(GL_TRIANGLES);

	glVertex2f(-0.1f, 0.05f);
	glVertex2f(-0.4f, 0.3f);
	glVertex2f(-0.2f, 0.05f);


	glEnd();

	glBegin(GL_TRIANGLES);

	glVertex2f(-0.1f, -0.05f);
	glVertex2f(-0.4f, -0.3f);
	glVertex2f(-0.2f, -0.05f);


	glEnd();
}

void IAF(void)
{
	glBegin(GL_LINES);
	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex2f(0.01f, 0.04f);
	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex2f(0.01f, -0.04f);
	glEnd();

	//AAA
	glBegin(GL_LINES);
	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex2f(0.05f, 0.04f);
	glVertex2f(0.03f, -0.04f);
	glVertex2f(0.05f, 0.04f);
	glVertex2f(0.07f, -0.04f);
	glEnd();

	glBegin(GL_LINES);
	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex2f(0.09f, 0.04f);
	glVertex2f(0.09f, -0.04f);
	glVertex2f(0.09f, 0.04f);
	glVertex2f(0.12f, 0.04);
	glVertex2f(0.09f, 0.01f);
	glVertex2f(0.12f, 0.01f);
	glEnd();
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