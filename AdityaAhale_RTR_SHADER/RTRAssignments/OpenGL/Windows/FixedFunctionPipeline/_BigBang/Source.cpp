#include<Windows.h>
#include<stdio.h>
#include<gl/GL.h>
#include<gl/GLU.h>
#include "mytex.h"
#include "Header.h"
#include<math.h>
#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glu32.lib")

#define WIN_WIDTH  800
#define WIN_HEIGHT 600

struct stack {
	int stk[5];
	int top = -1;
};
typedef struct stack STACK;
STACK s;

//GLuint fontOffset;
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
int initialize(void);
void display(void);
void update(void);
void printString(char*);
int push(void);
int pop(void);
int height, width;
HWND ghwnd;
bool bDone = false;
bool gbActiveWindow = false;
bool gbEscKeyPressed = false;
bool gbFullScreen = false;
bool isgamePaused = true;
bool gameReset = false;
bool flag = false;
bool flag1 = false;
bool isGameStart = false;
bool isGameOver = false;
bool help = false;
DWORD dwStyle;
WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };
HDC ghdc;
HGLRC ghrc = NULL;
FILE *gpFile = NULL;
int count;
GLfloat rot = 0.0f;
GLfloat tri = 0.0f;
GLuint Texture_earth;
GLuint Texture_Stone;
GLuint Texture_Moon;
GLuint Texture_mars;
GLUquadric *quadric = NULL;
GLUquadric *quadricTex = NULL;
GLUquadric *quadricObstacle[5];
GLUquadric *quadricPerson = NULL;
GLUquadric *quadricLifes[5];
GLdouble radius = 0.5;
GLdouble radiusPerson = 0.1;
GLdouble radiusLife = 0.1;
GLdouble radiusSmallObs = 0.4;
GLfloat tx , ty , tpersony, tpersonx, tx5 , ty5 , tx6, ty6 , currentty, currenttx;
GLfloat tx1 , ty1, ty2 , tx2, ty3 , tx3, ty4, tx4,speed,counter;
GLfloat lifey1, lifey2 , lifey3, lifey4 , lifey5;
GLfloat Health[3];


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

	hwnd = CreateWindowEx(WS_EX_APPWINDOW, AppName, TEXT("Texture"), WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE, 0, 0, 800, 600, NULL, NULL, hInstance, NULL);

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
	ShowWindow(hwnd, SW_MAXIMIZE);
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
			if (isgamePaused == false)
			{
				update();
			}
			
		}
	}

}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
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
			
			break;
		case 0x41:
		case VK_LEFT:
			if (tpersonx < -4.0 || isgamePaused == true)
			{
				tpersonx = tpersonx + 0.0f;
			}
			else {
				tpersonx = tpersonx - 0.19f;
			}
			break;

		case 0x44:
		case VK_RIGHT:
			if (tpersonx > 4.0 || isgamePaused == true)
			{
				tpersonx = tpersonx + 0.0f;
			}
			else {
				tpersonx = tpersonx + 0.19f;
			}
			break;

		case 0x50:
			if (isgamePaused == false)
			{
				isgamePaused = true;
			}
			else {
				isgamePaused = false;
			}
			break;

		case 0x52:
			gameReset = true;
			break;

		case 0x48:
			if (help == false)
			{
				help = true;
			}
			else {
				help = false;
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
	void ToggleScreen(void);
	void resize(int, int);
	void postUpdate(void);
	void makeRasterFont(void);
	postUpdate();
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
	LoadGLTextures(&Texture_earth, MAKEINTRESOURCE(IDBITMAP_EARTH));
	LoadGLTextures(&Texture_Stone, MAKEINTRESOURCE(IDBITMAP_STONE));
	LoadGLTextures(&Texture_Moon, MAKEINTRESOURCE(IDBITMAP_MOON));
	LoadGLTextures(&Texture_mars, MAKEINTRESOURCE(IDBITMAP_MARS));
	
	//enable textures here
	makeRasterFont();
	glEnable(GL_TEXTURE_2D);

	for (int i = 0; i < 4; i++) {
		push();
	}

	for (int i = 0; i < 5; i++)
	{
		quadricObstacle[i] = gluNewQuadric();
		quadricLifes[i] = gluNewQuadric();
	}
	quadricPerson = gluNewQuadric();
	Health[0] = 0.0f;
	Health[1] = 1.0f;
	Health[2] = 0.0f;
	
	tx = 0.0f, ty = -6.0f, tpersony = 1.85f, tpersonx = 0.0f, tx5 = -3.7f, ty5 = -12.0f, tx6 = 3.7f, ty6 = -12.0f;
	tx1 = -2.5f, ty1 = -6.0f, ty2 = -6.0f, tx2 = 2.5f, ty3 = -10.0f, tx3 = -1.3f, ty4 = -10.0f, tx4 = 1.3f;
	lifey1 = 2.7f, lifey2 = 2.7f, lifey3 = 2.7f, lifey4 = 2.7f, lifey5 = 2.7f;
	speed = 0.08f;
	resize(WIN_WIDTH, WIN_HEIGHT);
	ToggleScreen();
	return 0;
}

int push(void) {
	
	s.top = s.top + 1;
	//s.stk[s.top] = num;
	//lifey1 = 5.0f;
	return 1;
}
int pop() {
	//int num;
	if (s.top == -1) {

		lifey5 = 5.0f;
		PlaySound(MAKEINTRESOURCE(IDWM_DC), NULL, SND_RESOURCE | SND_ASYNC | SND_NODEFAULT);
		isgamePaused = true;
		isGameOver = true;
		return 1;
	}
	else if (s.top == 3) {
		//num = s.stk[s.top];
		s.top = s.top - 1;
		lifey1 = 5.0f;
		flag = true;
		Health[0] = 0.0f;
		Health[1] = 1.0f;
		Health[2] = 0.0f;
		PlaySound(MAKEINTRESOURCE(IDWM_DC), NULL, SND_RESOURCE | SND_ASYNC | SND_NODEFAULT);
		return 1;
	}
	else if (s.top == 2) {
		s.top = s.top - 1;
		lifey2 = 5.0f;
		flag = true;
		Health[0] = 1.0f;
		Health[1] = 1.0f;
		Health[2] = 0.0f;
		PlaySound(MAKEINTRESOURCE(IDWM_DC), NULL, SND_RESOURCE | SND_ASYNC | SND_NODEFAULT);
		return 1;
	}
	else if (s.top == 1) {
		s.top = s.top - 1;
		lifey3 = 5.0f;
		flag = true;
		PlaySound(MAKEINTRESOURCE(IDWM_DC), NULL, SND_RESOURCE | SND_ASYNC | SND_NODEFAULT);
		return 1;
	}
	else if (s.top == 0) {
		s.top = s.top - 1;
		lifey4 = 5.0f;
		flag = true;
		Health[0] = 1.0f;
		Health[1] = 0.0f;
		Health[2] = 0.0f;
		PlaySound(MAKEINTRESOURCE(IDWM_DC), NULL, SND_RESOURCE | SND_ASYNC | SND_NODEFAULT);
		return 1;
	}
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

	void Obstacles(void);
	void lifes(void);
	void ScoreChange(void);
	GLdouble distance, totalRadius, distance1, distance2, distance3, distance4, distance5, distance6, totalRadius2;

	if (gameReset == true) {
		tx = 0.0f, ty = -6.0f, tpersony = 1.85f, tpersonx = 0.0f, tx5 = -3.9f, ty5 = -12.0f, tx6 = 3.9f, ty6 = -12.0f;
		tx1 = -2.5f, ty1 = -6.0f, ty2 = -6.0f, tx2 = 2.5f, ty3 = -10.0f, tx3 = -1.3f, ty4 = -10.0f, tx4 = 1.3f;
		lifey1 = 2.7f, lifey2 = 2.7f, lifey3 = 2.7f, lifey4 = 2.7f, lifey5 = 2.7f, speed = 0.08f,tri=0.0f;
		counter = 0.0f;
		isgamePaused = true;
		isGameOver = false;
		flag = false;
		flag1 = false;
		s.top = -1;
		for (int i = 0; i < 4; i++) {
			push();
		}
		Health[0] = 0.0f;
		Health[1] = 1.0f;
		Health[2] = 0.0f;
	
		

		gameReset = false;
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	distance = sqrt((tpersonx - tx)*(tpersonx - tx) + (tpersony - ty)*(tpersony - ty));
	distance1 = sqrt((tpersonx - tx1)*(tpersonx - tx1) + (tpersony - ty1)*(tpersony - ty1));
	distance2 = sqrt((tpersonx - tx2)*(tpersonx - tx2) + (tpersony - ty2)*(tpersony - ty2));
	distance3 = sqrt((tpersonx - tx3)*(tpersonx - tx3) + (tpersony - ty3)*(tpersony - ty3));
	distance4 = sqrt((tpersonx - tx4)*(tpersonx - tx4) + (tpersony - ty4)*(tpersony - ty4));
	distance5 = sqrt((tpersonx - tx5)*(tpersonx - tx5) + (tpersony - ty5)*(tpersony - ty5));
	distance6 = sqrt((tpersonx - tx6)*(tpersonx - tx6) + (tpersony - ty6)*(tpersony - ty6));
	
	totalRadius = radius + radiusPerson;
	totalRadius2 = radiusSmallObs + radiusPerson;

	glMatrixMode(GL_MODELVIEW);
	glDisable(GL_TEXTURE_2D);
	glLoadIdentity();
	glTranslatef(-2.0f, 2.2f, -7.0f);
	glColor3f(1.0f, 1.0f, 1.0f);
	glRasterPos2f(-3.0f, 0.5f);
	printString((char*)"SCORE :");
	
	ScoreChange();
	glRasterPos2f(-3.1f, 0.32f);
	printString((char*)"----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------");

	if (isgamePaused == true) {
		glRasterPos2f(-2.2f, 0.5f);
		printString((char*)"PLAY");
	}
	else {
		glRasterPos2f(-2.2f, 0.5f);
		printString((char*)"PAUSE");
	}
	if (help == true) {
		glRasterPos2f(-1.6f, 0.5f);
		printString((char*)"P-Play/Pause  R-Restart  Move- A/D/Left/Right  Esc-Quit");
	}
	else {
		glRasterPos2f(-1.6f, 0.5f);
		printString((char*)"H-HELP");
	}
	

	if (isGameOver == true) {
		glColor3f((GLfloat)rand() / RAND_MAX, (GLfloat)rand() / RAND_MAX, (GLfloat)rand() / RAND_MAX);
		glRasterPos2f(1.7f, 0.5f);
		printString((char*)"GAME OVER");
	}
	else {
		glColor3f((GLfloat)rand() / RAND_MAX, (GLfloat)rand() / RAND_MAX, (GLfloat)rand() / RAND_MAX);
		glRasterPos2f(1.7f, 0.5f);
		printString((char*)"BIG-BANG");
	}
	
		
	

	glLoadIdentity();
	glTranslatef(0.0f, 0.0, -8.0);

	glBegin(GL_QUADS);

	
	//glColor3f(Health[0], Health[1], Health[2]);
	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex2f(-6.0f, 6.0f);
	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex2f(-6.0f, 0.0f);
	//glColor3f(0.0f, 0.0f, 0.0f);
	glColor3f(Health[0], Health[1], Health[2]);
	glVertex2f(0.0f, 0.0f);
	//glColor3f(Health[0], Health[1], Health[2]);
	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex2f(0.0f, 6.0f);
	glEnd();

	glLoadIdentity();
	glTranslatef(0.0f, 0.0, -8.0);

	glBegin(GL_QUADS);

	//glColor3f(1.0f, 1.0f, 1.0f);
	//glColor3f(Health[0], Health[1], Health[2]);
	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex2f(0.0f, 6.0f);
	glColor3f(Health[0], Health[1], Health[2]);
	//glColor3f(0.0f, 0.0f, 0.0f);
	glVertex2f(0.0f, 0.0f);
	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex2f(6.0f, 0.0f);
	//glColor3f(Health[0], Health[1], Health[2]);
	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex2f(6.0f, 6.0f);

	glEnd();


	glLoadIdentity();
	glTranslatef(0.0f, 0.0, -8.0);

	glBegin(GL_QUADS);

	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex2f(-6.0f, 0.0f);
	//glColor3f(Health[0], Health[1], Health[2]);
	glVertex2f(-6.0f, -6.0f);
	//glColor3f(1.0f, 1.0f, 1.0f);
	//glColor3f(Health[0], Health[1], Health[2]);
	glVertex2f(0.0f, -6.0f);
	//glColor3f(0.0f, 0.0f, 0.0f);
	glColor3f(Health[0], Health[1], Health[2]);
	glVertex2f(0.0f, 0.0f);


	glEnd();

	glLoadIdentity();
	glTranslatef(0.0f, 0.0, -8.0);

	glBegin(GL_QUADS);

	//glColor3f(0.0f, 0.0f, 0.0f);
	glColor3f(Health[0], Health[1], Health[2]);
	glVertex2f(0.0f, 0.0f);
	glColor3f(0.0f, 0.0f, 0.0f);
	//glColor3f(Health[0], Health[1], Health[2]);
	glVertex2f(0.0f, -6.0f);
	glColor3f(Health[0], Health[1], Health[2]);
	glVertex2f(6.0f, -6.0f);
	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex2f(6.0f, 0.0f);


	glEnd();


	if (flag1 == true)
	{
		//currentty = ty;
		glLoadIdentity();
		glTranslatef(currenttx + 3.5f, currentty, -4.0f);
		glPointSize(3.0f);
		for (float i = 1.0; i < 6.0; i += 0.4f)
		{
			for (float j = 1.0; j > -6.0; j -= 0.4f)
			{
				glBegin(GL_POINTS);

				glColor3f((GLfloat)rand() / RAND_MAX, (GLfloat)rand() / RAND_MAX, (GLfloat)rand() / RAND_MAX);
				glVertex2f(-i, j);

				glEnd();

			}
		}
	}

	//-------------------------Texture Drawing Below-----------------------------------


	glColor3f(1.0f, 1.0f, 1.0f);
	glEnable(GL_TEXTURE_2D);


	glLoadIdentity();
	glTranslatef(tpersonx, tpersony, -6.0f);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBindTexture(GL_TEXTURE_2D, Texture_Moon);
	gluQuadricTexture(quadricPerson, TRUE);
	gluSphere(quadricPerson, radiusPerson, 100, 100);


	glLoadIdentity();
	glTranslatef(5.0f, lifey1, -7.0);
	//glRotatef(0.0f, 1.0f, 0.0f, 0.0f);
	glRotatef(tri, 0.0f, 1.0f, 0.0f);
	lifes();

	glLoadIdentity();
	glTranslatef(4.5f, lifey2, -7.0);
	glRotatef(tri, 0.0f, 1.0f, 0.0f);
	lifes();

	glLoadIdentity();
	glTranslatef(4.0f, lifey3, -7.0);
	glRotatef(tri, 0.0f, 1.0f, 0.0f);
	lifes();

	glLoadIdentity();
	glTranslatef(3.5f, lifey4, -7.0);
	glRotatef(tri, 0.0f, 1.0f, 0.0f);
	lifes();

	glLoadIdentity();
	glTranslatef(3.0f, lifey5, -7.0);
	glRotatef(tri, 0.0f, 1.0f, 0.0f);
	lifes();

	glLoadIdentity();
	glTranslatef(tx, ty, -6.0f);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	if (distance < totalRadius && flag == false)
	{
		
		flag1 = true;
		pop();
	}
	Obstacles();

	glLoadIdentity();
	glTranslatef(tx, ty, -6.0f);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	if (distance < totalRadius && flag == false)
	{
		flag1 = true;
		pop();
	}
	Obstacles();

	glLoadIdentity();
	glTranslatef(tx1, ty1, -6.0f);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	if (distance1 < totalRadius && flag == false)
	{
		flag1 = true;
		pop();
	}
	Obstacles();

	glLoadIdentity();
	glTranslatef(tx2, ty2, -6.0f);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	if (distance2 < totalRadius && flag == false)
	{
		flag1 = true;
		pop();
	}
	Obstacles();

	glLoadIdentity();
	glTranslatef(tx3, ty3, -6.0f);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	if (distance3 < totalRadius && flag == false)
	{
		flag1 = true;
		pop();
	}
	Obstacles();

	glLoadIdentity();
	glTranslatef(tx4, ty4, -6.0f);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	if (distance4 < totalRadius && flag == false)
	{
		flag1 = true;
		pop();
	}
	Obstacles();

	glLoadIdentity();
	glTranslatef(tx5, ty5, -6.0f);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	if (distance5 < totalRadius2 && flag == false)
	{
		flag1 = true;
		pop();
	}
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBindTexture(GL_TEXTURE_2D, Texture_mars);
	gluQuadricTexture(quadricObstacle[1], TRUE);
	gluSphere(quadricObstacle[1], radiusSmallObs, 50, 50);

	glLoadIdentity();
	glTranslatef(tx6, ty6, -6.0f);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	if (distance6 < totalRadius2 && flag == false)
	{
		flag1 = true;
		pop();
	}
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBindTexture(GL_TEXTURE_2D, Texture_mars);
	gluQuadricTexture(quadricObstacle[1], TRUE);
	gluSphere(quadricObstacle[1], radiusSmallObs, 50, 50);


	SwapBuffers(ghdc);
}


void ScoreChange(void)
{
	if (counter < 200) {
		glRasterPos2f(-2.6f, 0.5f);
		printString((char*)"000");
	}
	else if (counter > 200 && counter < 400) {
		glRasterPos2f(-2.6f, 0.5f);
		printString((char*)"100");
	}
	else if (counter > 400 && counter < 600) {
		glRasterPos2f(-2.6f, 0.5f);
		printString((char*)"200");
	}
	else if (counter > 600 && counter < 800) {
		glRasterPos2f(-2.6f, 0.5f);
		printString((char*)"300");
	}
	else if (counter > 800 && counter < 1000) {
		glRasterPos2f(-2.6f, 0.5f);
		printString((char*)"400");
	}
	else if (counter > 1000 && counter < 1200) {
		glRasterPos2f(-2.6f, 0.5f);
		printString((char*)"500");
	}
	else if (counter > 1000 && counter < 1300) {
		glRasterPos2f(-2.6f, 0.5f);
		printString((char*)"600");
	}
	else if (counter > 1300 && counter < 1600) {
		glRasterPos2f(-2.6f, 0.5f);
		printString((char*)"700");
	}
	else if (counter > 1600 && counter < 1900) {
		glRasterPos2f(-2.6f, 0.5f);
		printString((char*)"800");
	}
	else if (counter > 1900 && counter < 2300) {
		glRasterPos2f(-2.6f, 0.5f);
		printString((char*)"900");
	}
	else if (counter > 2300 && counter < 2800) {
		glRasterPos2f(-2.6f, 0.5f);
		printString((char*)"1000");
	}
	else {
		glRasterPos2f(-2.6f, 0.5f);
		printString((char*)"1500");
	}
}

void postUpdate(void)
{
	if (flag1 == true)
	{
		currenttx = tx;
		currentty = ty;
	}
}

void Obstacles(void) {

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


	glBindTexture(GL_TEXTURE_2D, Texture_Stone);
	gluQuadricTexture(quadricObstacle[0], TRUE);
	gluSphere(quadricObstacle[0], radius, 50, 50);
}

void lifes(void) {

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


	glBindTexture(GL_TEXTURE_2D, Texture_earth);
	gluQuadricTexture(quadricLifes[0], TRUE);
	gluSphere(quadricLifes[0], radiusLife, 100, 100);
}

void update(void)
{
	counter = counter + 0.5f;
	if (counter > 1200 && counter < 1800) {
		speed = 0.1f;
		radius = radius - 0.01;
		if (radius < 0.1) {
			radius = 0.5f;
		}
	}
	else if (counter > 1800) {
		radius = 0.5f;
		speed = 0.2f;
	}
	
	else if (counter > 900 && counter < 1200) {
		speed = 0.09f;
	}
	ty = ty + speed;
	if (ty > 10)
	{
		ty = -8.0f;
		flag = false;
		flag1 = false;
	}
	ty1 = ty1 + speed;
	if (ty1 > 10)
	{
		ty1 = -8.0f;
		flag = false;
	}
	ty2 = ty2 + speed;
	if (ty2 > 10)
	{
		ty2 = -8.0f;
		flag = false;
	}
	ty3 = ty3 + speed;
	if (ty3 > 10)
	{
		ty3 = -12.0f;
		flag = false;
	}
	ty4 = ty4 + speed;
	if (ty4 > 10)
	{
		ty4 = -12.0f;
		flag = false;
	}

	ty5 = ty5 + speed;
	if (ty5 > 10)
	{
		ty5 = -14.0f;
		flag = false;
	}

	ty6 = ty6 + speed;
	if (ty6 > 10)
	{
		ty6 = -14.0f;
		flag = false;
	}

	tri = tri + 0.5f;

	if (tri >= 360.0f)
	{
		tri = 0.0f;
	}

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

	if (quadricObstacle)
	{
		for (int i = 0; i < 23; i++)
		{
			gluDeleteQuadric(quadricObstacle[i]);
			quadricObstacle[i] = NULL;
		}
		
	}
	if (quadricLifes)
	{
		for (int i = 0; i < 23; i++)
		{
			gluDeleteQuadric(quadricLifes[i]);
			quadricLifes[i] = NULL;
		}
		
	}

	if (quadricPerson)
	{
		gluDeleteQuadric(quadricPerson);
		quadricPerson = NULL;
	}
}