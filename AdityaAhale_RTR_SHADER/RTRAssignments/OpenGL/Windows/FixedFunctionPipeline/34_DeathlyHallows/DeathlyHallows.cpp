#include<Windows.h>
#include<stdio.h>
#include<gl/GL.h>
#include<gl/GLU.h>
#define _USED_MATH_DEFINES 1
#include<math.h>

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
GLfloat rot = 0.0f,xtrans=-3.0f,ytrans=-3.0f,xctrans=3.0,yctrans=-3.0,yltrans=3.0;
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
			spin();
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
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	resize(WIN_WIDTH, WIN_HEIGHT);
	return 0;
}

void display(void)
{
	/*KUNDALI
	color-white and plan no graph
	border la line width
	??
	*/

	/* concentric Circle
	10 circle
	multi-color 1.0 0.5 0.0 --orange
	0.5 0.5 0.5
	*/

	/*concentric rectangle
	  concentric square
	*/

	/*in circle
	  hint: use math
	*/

	/*graph circle==>rectangle/square==>triangle==>circle ---yellow
	*/

	/*
	h/w==> 1)spinnig triangle 2)rectangle pitching
	deathly hallows
	spinning_deathly hallows
	to spin rect and tri
	*/

	glClear(GL_COLOR_BUFFER_BIT);
	float radius;			//1,-1	0,1  -1,-1 
	float area;
	float a, b, c, tx, ty;
	float x1 = -1.0f, x2 = 1.0f, x3 = 0.0f, yone = -1.0f, y2 = -1.0f, y3 = 1.0f;
	float s, summation;
	c = sqrtf((x2 - x1)*(x2 - x1) + (y2 - yone)*(y2 - yone)); //These are opposite lengths/sides of vertices
	a = sqrtf((x3 - x2)*(x3 - x2) + (y3 - y2)*(y3 - y2));
	b = sqrtf((x1 + x3)*(x1 - x3) + (yone - y3)*(yone - y3));
	summation = a + b + c;
	s = (float)(a + b + c) / 2.0f;
	tx = ((a*x1) + (b*x2) + (c*x3)) / summation;
	ty = ((a*yone) + (b*y2) + (c*y3)) / summation;
	area = sqrtf(s*(s - a)*(s - b)*(s - c));
	radius = area / s;
	
	glLoadIdentity();
	glTranslatef(xtrans, ytrans, -4.0f);
	//glTranslatef(0.0f, 0.0f, -4.0f);
	

	
	glRotatef(rot, 0.0f, 1.0f, 0.0f);
	glBegin(GL_LINES);
	glVertex2f(x1, yone);
	glVertex2f(x2, y2);
	glVertex2f(x2, y2);
	glVertex2f(x3, y3);
	glVertex2f(x3, y3);
	glVertex2f(x1, yone);
	glEnd();


	glLoadIdentity();
	//glTranslatef(xctrans, yctrans, -4.0f);
	glTranslatef(tx+xctrans, ty-xctrans, -4.0f);
	
	glRotatef(rot, 0.0f, 1.0f, 0.0f);
	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < numPoints; i++)
	{
		GLfloat angle = 2.0*PI*i / numPoints;
		//glColor3f(colourArray[coun][0], colourArray[coun][1], colourArray[coun][2]);
		glVertex3f(cos(angle)*radius, sin(angle)*radius, 0.0f);

	}
	glEnd();

	glLoadIdentity();
	glTranslatef(0.0f, yltrans, -4.0f);
	//glTranslatef(0.0f, 0.0f, -4.0f);



	glRotatef(rot, 0.0f, 1.0f, 0.0f);
	glBegin(GL_LINES);
	glVertex2f(0.0f, 1.0);
	glVertex2f(0.0f, -1.0f);
	glEnd();


	SwapBuffers(ghdc);
}
void translatemy(void)
{
	if (xtrans <= 0.0)
	{
		xtrans = xtrans + 0.001;
		ytrans = ytrans + 0.001;
	}
	
	if (xtrans >= 0.0)
	{
		if (xctrans > 0.0)
		{
			xctrans = xctrans - 0.001;
			yctrans = yctrans + 0.001;
			
		}

		
	}

	if (xtrans > 0.0)
	{
		if (xctrans < 0.0)
		{
			if (yltrans >= 0.0)
			{
				yltrans = yltrans - 0.001;
			}
		}
	}
		

	
}
void spin(void)
{
	rot = rot + 0.1f;
	if (rot >= 360)
	{
		rot = 0.0f;
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
}