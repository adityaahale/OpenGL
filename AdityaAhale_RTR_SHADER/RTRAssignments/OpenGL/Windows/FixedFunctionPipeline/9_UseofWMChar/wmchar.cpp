#include <windows.h>

//global function declarations
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);



//Winmain()
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	//variable declaration
	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szClassName[] = TEXT("MyApp");

	//code
	//initializing of WNDCLASSEX
	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.lpfnWndProc = WndProc;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.lpszClassName = szClassName;
	wndclass.lpszMenuName = NULL;
	wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	//register above class
	RegisterClassEx(&wndclass);

	//Create Window
	hwnd = CreateWindow(szClassName,
		TEXT("My Window-Aditya Ahale"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		NULL,
		NULL,
		hInstance,
		NULL);

	ShowWindow(hwnd, iCmdShow);
	UpdateWindow(hwnd);

	//Message Loop
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return((int)msg.wParam);
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	//code
	switch (iMsg)
	{
	case WM_CREATE:
		MessageBox(hwnd, TEXT("A Window is going to be created TRY pressing Esc,F,Right & Left mouse button"), TEXT("My Message"), MB_OK);
		break;
	case WM_CHAR:
		switch (wParam)
		{
		case 'f':

			//process f
			MessageBox(hwnd, TEXT("Fullscreen key is press"), TEXT("My Message"), MB_OK);

			break;
		case 0x1B:
			MessageBox(hwnd, TEXT("Escape key is pressed, Program will exit now"), TEXT("My Message"), MB_OK);
			DestroyWindow(hwnd);
			break;
		default:
			/*TCHAR ch = TCHAR(wParam);
			//atof(ch);
			MessageBox(hwnd, (LPCWSTR)ch, TEXT("My Message"), MB_OK);*/
			break;
		}

	case WM_LBUTTONDOWN:
		MessageBox(hwnd, TEXT("You have Clicked Left Mouse Button"), TEXT("My Message"), MB_OK);
		break;

	case WM_RBUTTONDOWN:
		MessageBox(hwnd, TEXT("You have Clicked Right Mouse Button"), TEXT("My Message"), MB_OK);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	}

	return(DefWindowProc(hwnd, iMsg, wParam, lParam));
}