#include<iostream>
#include<Windows.h>
#include<stdio.h> //for file IO
#include <stdlib.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include "XNAMath/xnamath.h"

#pragma warning(disable: 4838)
#pragma comment(lib,"d3d11.lib")
#pragma comment(lib, "D3dcompiler.lib")

#define WIN_WIDTH_ATA  800
#define WIN_HEIGHT_ATA 600

//global-function declarations
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

//global-variables declarations
int height, width;
bool bDoneAta = false;
bool gbActiveWindowAta = false;
bool gbEscKeyPressedATA = false;
bool gbFullScreenAta = false;

HWND ghwnd;
DWORD dwStyleAta;
WINDOWPLACEMENT wpPrevAta = { sizeof(WINDOWPLACEMENT) };
HDC ghdc;
HGLRC ghrc = NULL;
FILE *gpFileAta = NULL;
char gszLogFileNameAta[] = "Log.txt";

float gClearColorAta[4]; //RGBA
IDXGISwapChain *gpIDXGISwapChain = NULL;
ID3D11Device *gpID3D11Device = NULL;
ID3D11DeviceContext *gpID3D11DeviceContext = NULL;
ID3D11RenderTargetView *gpID3D11RenderTargetView = NULL;

ID3D11VertexShader *gpID3D11VertexShader = NULL;
ID3D11PixelShader *gpID3D11PixelShader = NULL;
ID3D11Buffer *gpID3D11Buffer_PyramidVertexBufferAta = NULL;
ID3D11Buffer *gpID3D11Buffer_CubeVertexBufferAta;
ID3D11Buffer *gpID3D11Buffer_PyramidColorBufferAta = NULL;
ID3D11Buffer *gpID3D11Buffer_CubeColorBufferAta = NULL;
ID3D11InputLayout *gpID3D11InputLayout = NULL;
ID3D11Buffer *gpID3D11Buffer_ConstantBuffer = NULL;
ID3D11RasterizerState *gpID3DRasterizerState = NULL;

ID3D11DepthStencilView *gpID3D11DepthStencilView = NULL;
float angleTrieAta;
float angleSquareAta;

bool gbLight = false;
bool gbAnimate = false;

struct CBUFFER
{
	XMMATRIX WorldMatrixAta;
	XMMATRIX ViewMatrixAta;
	XMMATRIX ProjectionMatrixAta;
	XMVECTOR laZero;
	XMVECTOR ldZero;
	XMVECTOR lsZero;
	XMVECTOR laOne;
	XMVECTOR ldOne;
	XMVECTOR lsOne;
	XMVECTOR ka;
	XMVECTOR kd;
	XMVECTOR ks;
	XMVECTOR lightZeroPosition;
	XMVECTOR lightOnePosition;
	unsigned int lKeyPressed;
	float materialShininess;
};

//light setting detail
float light0Ambient[] = { 0.0f, 0.0f, 0.0f, 0.0f };
float light0Diffuse[] = { 1.0f,0.0f,0.0f,0.0f };
float light0Specular[] = { 1.0f, 0.0f, 0.0f, 0.0f };
float light0Position[] = { 2.0f, 1.0f, 1.0f, 0.0f };

float light1Ambient[] = { 0.0f, 0.0f, 0.0f, 0.0f };
float light1Diffuse[] = { 0.0f,0.0f,1.0f,0.0f };
float light1Specular[] = { 0.0f, 0.0f, 1.0f, 0.0f };
float light1Position[] = { -2.0f, 1.0f, 1.0f, 0.0f };

float material_ambient[] = { 0.0f, 0.0f, 0.0f, 0.0f };
float material_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
float material_specular[] = { 1.0f, 1.0f, 1.0f, 0.0f };
float material_shininess = 50.0f;


XMMATRIX PerspectiveProjectionMatrixAta;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	//function declarations
	HRESULT initialize(void);
	void display(void);
	void update(void);
	void uninitialize(void);

	//variable declarations
	MSG msg;
	TCHAR AppName[] = TEXT("D3D");
	WNDCLASSEX wndclass;
	HWND hwnd;

	//code
	//create log file
	if (fopen_s(&gpFileAta, gszLogFileNameAta, "w") != 0)
	{
		MessageBox(NULL, TEXT("Cannot create log file"), TEXT("Error"), MB_OK);
		exit(0);
	}
	else {
		fprintf(gpFileAta, "log file created");
		fclose(gpFileAta);
	}

	//initialize WNDCLASSEX structure
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

	//register WNDCLASSEX structure
	if (!RegisterClassEx(&wndclass)) {
		MessageBox(NULL, TEXT("Failed to register wndclass. Exiting"), TEXT("Error"), MB_OK);
		exit(EXIT_FAILURE);
	}

	//Create Window
	hwnd = CreateWindowEx(WS_EX_APPWINDOW, AppName, TEXT("D3D"), WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE, 0, 0, 800, 600, NULL, NULL, hInstance, NULL);

	ghwnd = hwnd;

	ShowWindow(hwnd, iCmdShow);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);

	//initialize D3D
	HRESULT hr = initialize();
	if (FAILED(hr))
	{
		fopen_s(&gpFileAta, gszLogFileNameAta, "a+");
		fprintf_s(gpFileAta, "initialize() failed. Exiting \n");
		fclose(gpFileAta);
	}
	else {
		fopen_s(&gpFileAta, gszLogFileNameAta, "a+");
		fprintf_s(gpFileAta, "initialize() SUCCEEDED \n");
		fclose(gpFileAta);
	}

	//game loop
	while (bDoneAta == false)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				bDoneAta = true;
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			//render
			display();
			if (gbActiveWindowAta == true)
			{
				if (gbEscKeyPressedATA == true)
					bDoneAta = true;
				update();

			}


		}
	}
	uninitialize();
	return((int)msg.wParam);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	//function declarations
	//void display(void);
	HRESULT resize(int, int);
	void ToggleScreen(void);
	void uninitialize(void);

	//variable declarations
	HRESULT hr;


	switch (iMsg)
	{
	case WM_ACTIVATE:
		if (HIWORD(wParam) == 0)
			gbActiveWindowAta = true;
		else
			gbActiveWindowAta = false;
		break;
	case WM_ERASEBKGND:
		return (0);
		break;
	case WM_SIZE:
		if (gpID3D11DeviceContext)
		{
			hr = resize(LOWORD(lParam), HIWORD(lParam));
			if (FAILED(hr))
			{
				fopen_s(&gpFileAta, gszLogFileNameAta, "a+");
				fprintf_s(gpFileAta, "resize() failed. Exiting \n");
				fclose(gpFileAta);
				uninitialize();
			}
			else
			{
				fopen_s(&gpFileAta, gszLogFileNameAta, "a+");
				fprintf_s(gpFileAta, "initialize() SUCCEEDED. Exiting \n");
				fclose(gpFileAta);
			}
		}
		break;
	case WM_KEYDOWN:

		switch (wParam)
		{
		case VK_ESCAPE:	//case 27
			if (gbEscKeyPressedATA == false)
				gbEscKeyPressedATA = true;
			break;
		case 0x46:	// 'F' or 'f'
			if (gbFullScreenAta == false)
			{
				ToggleScreen();
				gbFullScreenAta = true;
			}
			else
			{
				ToggleScreen();
				gbFullScreenAta = false;
			}
			break;
		case 0x41:
			if (gbAnimate == false)
			{
				gbAnimate = true;
			}
			else
			{
				gbAnimate = false;
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
	case WM_LBUTTONDOWN:
		break;
	case WM_SETFOCUS:
		gbActiveWindowAta = true;
		break;
	case WM_KILLFOCUS:
		gbActiveWindowAta = true;
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
	//variable declarations
	MONITORINFO mi;

	//code
	if (gbFullScreenAta == false)
	{
		dwStyleAta = GetWindowLong(ghwnd, GWL_STYLE);
		if (dwStyleAta & WS_OVERLAPPEDWINDOW)
		{
			mi.cbSize = sizeof(MONITORINFO);

			if (GetWindowPlacement(ghwnd, &wpPrevAta) && GetMonitorInfo(MonitorFromWindow(ghwnd, MONITORINFOF_PRIMARY), &mi))
			{
				SetWindowLong(ghwnd, GWL_STYLE, dwStyleAta & ~WS_OVERLAPPEDWINDOW);
				SetWindowPos(ghwnd, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top, SWP_NOZORDER | SWP_FRAMECHANGED);
			}
		}
		ShowCursor(FALSE);
	}
	else
	{
		SetWindowLong(ghwnd, GWL_STYLE, dwStyleAta | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd, &wpPrevAta);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);
		ShowCursor(TRUE);
	}
}

HRESULT initialize(void)
{
	//function declarations
	HRESULT resize(int, int);
	void uninitialize();

	//variable declarations
	HRESULT hr;
	D3D_DRIVER_TYPE d3dDriverTypeAta;
	D3D_DRIVER_TYPE d3dDriverTypesAta[] = {
		D3D_DRIVER_TYPE_HARDWARE, D3D_DRIVER_TYPE_WARP, D3D_DRIVER_TYPE_REFERENCE
	};
	D3D_FEATURE_LEVEL d3dFeatureLevel_required = D3D_FEATURE_LEVEL_11_0;
	D3D_FEATURE_LEVEL d3dFeatureLevel_acquired = D3D_FEATURE_LEVEL_10_0;
	UINT createDeviceFlagsAta = 0;
	UINT numDriverTypesAta = 0;
	UINT numFeatureLevelsAta = 1;

	//code
	numDriverTypesAta = sizeof(d3dDriverTypesAta) / (d3dDriverTypesAta[0]);
	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
	ZeroMemory((void*)&dxgiSwapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
	dxgiSwapChainDesc.BufferCount = 1;
	dxgiSwapChainDesc.BufferDesc.Width = WIN_WIDTH_ATA;
	dxgiSwapChainDesc.BufferDesc.Height = WIN_HEIGHT_ATA;
	dxgiSwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	dxgiSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	dxgiSwapChainDesc.OutputWindow = ghwnd;
	dxgiSwapChainDesc.SampleDesc.Count = 1;
	dxgiSwapChainDesc.Windowed = TRUE;
	for (UINT driverTypeIndexAta = 0; driverTypeIndexAta < numDriverTypesAta; driverTypeIndexAta++)
	{
		d3dDriverTypeAta = d3dDriverTypesAta[driverTypeIndexAta];
		hr = D3D11CreateDeviceAndSwapChain(
			NULL,
			d3dDriverTypeAta,
			NULL,
			createDeviceFlagsAta,
			&d3dFeatureLevel_required,
			numFeatureLevelsAta,
			D3D11_SDK_VERSION,
			&dxgiSwapChainDesc,
			&gpIDXGISwapChain,
			&gpID3D11Device,
			&d3dFeatureLevel_acquired,
			&gpID3D11DeviceContext
		);
		if (SUCCEEDED(hr))
			break;
	}
	if (FAILED(hr))
	{
		fopen_s(&gpFileAta, gszLogFileNameAta, "a+");
		fprintf_s(gpFileAta, "D3D11CreateDeviceAndSwapChain() failed. Exiting \n");
		fclose(gpFileAta);
	}
	else {
		fopen_s(&gpFileAta, gszLogFileNameAta, "a+");
		fprintf_s(gpFileAta, "D3D11CreateDeviceAndSwapChain() SUCCEEDED \n");
		fprintf_s(gpFileAta, "The Chosen Driver is of ");
		if (d3dDriverTypeAta == D3D_DRIVER_TYPE_HARDWARE)
		{
			fprintf_s(gpFileAta, " Hardware Type.\n");
		}
		else if (d3dDriverTypeAta == D3D_DRIVER_TYPE_WARP)
		{
			fprintf_s(gpFileAta, " WARP Type.\n");
		}
		else if (d3dDriverTypeAta == D3D_DRIVER_TYPE_REFERENCE)
		{
			fprintf_s(gpFileAta, " Reference Type.\n");
		}
		else
		{
			fprintf_s(gpFileAta, "Unknown Type.\n");
		}
		fprintf_s(gpFileAta, "The Supported Highest Feature Level is:");
		if (d3dFeatureLevel_acquired == D3D_FEATURE_LEVEL_11_0)
		{
			fprintf_s(gpFileAta, "11.0\n");
		}
		else if (d3dFeatureLevel_acquired == D3D_FEATURE_LEVEL_10_1)
		{
			fprintf_s(gpFileAta, "10.1\n");
		}
		else if (d3dFeatureLevel_acquired == D3D_FEATURE_LEVEL_10_0)
		{
			fprintf_s(gpFileAta, "10.\n");
		}
		else
		{
			fprintf_s(gpFileAta, "Unknown Type.\n");
		}

		fclose(gpFileAta);
	}
	//Initialize shaders, input layouts, constant buffers etc.

	//***********VERTEX SHADER **********************

	const char *vertexShaderSourceCodeAta =
		"cbuffer ConstantBuffer" \
		"{" \
		"float4x4 worldMatrix;" \
		"float4x4 viewMatrix;" \
		"float4x4 projectionMatrix;" \
		"float4 laZero;" \
		"float4 ldZero;" \
		"float4 lsZero;" \
		"float4 laOne;" \
		"float4 ldOne;" \
		"float4 lsOne;" \
		"float4 ka;" \
		"float4 kd;" \
		"float4 ks;" \
		"float4 lightZeroPosition;" \
		"float4 lightOnePosition;" \
		"uint lKeyPressed;" \
		"float materialShininess;" \
		"}" \
		"struct vertex_output" \
		"{" \
		"float4 position:SV_POSITION;" \
		"float3 tNorm:NORMAL0;" \
		"float3 lightZeroDirection:NORMAL1;" \
		"float3 lightOneDirection:NORMAL2;" \
		"float3 viewerVector:NORMAL3;" \
		"};" \
		"vertex_output main(float4 pos: POSITION, float4 normal:NORMAL)"\
		"{"\
		"vertex_output output;" \
		"\n" \
		"if(lKeyPressed == 1)" \
		"{" \
		"float4 eyeCoords= mul(worldMatrix,pos);" \
		"eyeCoords= mul(viewMatrix,eyeCoords);" \
		"output.tNorm = mul((float3x3)mul(viewMatrix, worldMatrix),(float3)normal);" \
		"output.lightZeroDirection=lightZeroPosition.xyz-eyeCoords.xyz;" \
		"output.lightOneDirection=lightOnePosition.xyz-eyeCoords.xyz;" \
		"output.viewerVector = -eyeCoords.xyz;" \
		"}" \
		"output.position = mul(worldMatrix,pos);" \
		"output.position = mul(viewMatrix,output.position);" \
		"output.position = mul(projectionMatrix,output.position);" \
		"return(output);"\
		"}";

	ID3DBlob *pID3DBlob_vertexShaderCode = NULL;
	ID3DBlob *pID3DBlob_Error = NULL;
	hr = D3DCompile(vertexShaderSourceCodeAta, lstrlenA(vertexShaderSourceCodeAta) + 1,
		"VS",
		NULL,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main",
		"vs_5_0",
		0,
		0,
		&pID3DBlob_vertexShaderCode,
		&pID3DBlob_Error);
	if (FAILED(hr))
	{
		if (pID3DBlob_Error != NULL)
		{
			fopen_s(&gpFileAta, gszLogFileNameAta, "a+");
			fprintf_s(gpFileAta, "D3DCompile() Failed for vertex shader: %s \n",
				(char*)pID3DBlob_Error->GetBufferPointer());
			fclose(gpFileAta);
			pID3DBlob_Error->Release();
			pID3DBlob_Error = NULL;
			return(hr);
		}
	}
	else
	{
		fopen_s(&gpFileAta, gszLogFileNameAta, "a+");
		fprintf_s(gpFileAta, "D3DCompile() succeeded for vertex shader: \n");
		fclose(gpFileAta);
	}
	hr = gpID3D11Device->CreateVertexShader(pID3DBlob_vertexShaderCode->GetBufferPointer(),
		pID3DBlob_vertexShaderCode->GetBufferSize(), NULL, &gpID3D11VertexShader);
	if (FAILED(hr))
	{

		fopen_s(&gpFileAta, gszLogFileNameAta, "a+");
		fprintf_s(gpFileAta, "ID3D11Device::CreateVertexShader() Failed for vertex shader\n");
		fclose(gpFileAta);
		return(hr);
	}
	else
	{
		fopen_s(&gpFileAta, gszLogFileNameAta, "a+");
		fprintf_s(gpFileAta, "ID3D11Device::CreateVertexShader() succeeded for vertex shader: \n");
		fclose(gpFileAta);
	}
	gpID3D11DeviceContext->VSSetShader(gpID3D11VertexShader, 0, 0);

	//***********PIXEL SHADER **********************
	const char *pixelShaderSourceCodeAta =
		"cbuffer ConstantBuffer" \
		"{" \
		"float4x4 worldMatrix;" \
		"float4x4 viewMatrix;" \
		"float4x4 projectionMatrix;" \
		"float4 laZero;" \
		"float4 ldZero;" \
		"float4 lsZero;" \
		"float4 laOne;" \
		"float4 ldOne;" \
		"float4 lsOne;" \
		"float4 ka;" \
		"float4 kd;" \
		"float4 ks;" \
		"float4 lightZeroPosition;" \
		"float4 lightOnePosition;" \
		"uint lKeyPressed;" \
		"float materialShininess;" \
		"}" \
		"struct vertex_output" \
		"{" \
		"float4 position:SV_POSITION;" \
		"float3 tNorm:NORMAL0;" \
		"float3 lightZeroDirection:NORMAL1;" \
		"float3 lightOneDirection:NORMAL2;" \
		"float3 viewerVector:NORMAL3;" \
		"};" \
		"float4 main(vertex_output output) : SV_TARGET" \
		"{"\
		"float4 phong_ads_light;" \
		"if(lKeyPressed == 1)" \
		"{" \
		"float3 normalizedtNorm = normalize(output.tNorm);" \
		"float3 normalizedviewerVector = normalize(output.viewerVector);" \

		"float3 normalizedlightZeroDirection=normalize(output.lightZeroDirection);" \
		"float tNormDotLightZeroDir = max(dot(normalizedtNorm,normalizedlightZeroDirection),0.0);" \
		"float4 ambientZero = laZero * ka;" \
		"float4 diffuseZero = ldZero * kd * tNormDotLightZeroDir;" \
		"float3 reflectionVectorZero = reflect(-normalizedlightZeroDirection,normalizedtNorm);" \
		"float4 specularZero = lsZero * ks * pow(max(dot(reflectionVectorZero,normalizedviewerVector),0.0), materialShininess);" \

		"float3 normalizedlightOneDirection=normalize(output.lightOneDirection);" \
		"float tNormDotLightOneDir = max(dot(normalizedtNorm,normalizedlightOneDirection),0.0);" \
		"float4 ambientOne = laOne * ka;" \
		"float4 diffuseOne = ldOne * kd * tNormDotLightOneDir;" \
		"float3 reflectionVectorOne = reflect(-normalizedlightOneDirection,normalizedtNorm);" \
		"float4 specularOne = lsOne * ks * pow(max(dot(reflectionVectorOne,normalizedviewerVector),0.0), materialShininess);" \

		"phong_ads_light=ambientZero + diffuseZero + specularZero + ambientOne + diffuseOne + specularOne;" \
		"}" \
		"\n" \
		"else" \
		"{" \
		"phong_ads_light = float4(1.0, 1.0, 1.0, 1.0);" \
		"}" \
		"return(phong_ads_light);"\
		"}";

	ID3DBlob *pID3DBlob_pixelShaderCode = NULL;
	pID3DBlob_Error = NULL;
	hr = D3DCompile(pixelShaderSourceCodeAta, lstrlenA(pixelShaderSourceCodeAta) + 1,
		"PS",
		NULL,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main",
		"ps_5_0",
		0,
		0,
		&pID3DBlob_pixelShaderCode,
		&pID3DBlob_Error);
	if (FAILED(hr))
	{
		if (pID3DBlob_Error != NULL)
		{
			fopen_s(&gpFileAta, gszLogFileNameAta, "a+");
			fprintf_s(gpFileAta, "D3DCompile() Failed for pixel shader: %s \n",
				(char*)pID3DBlob_Error->GetBufferPointer());
			fclose(gpFileAta);
			pID3DBlob_Error->Release();
			pID3DBlob_Error = NULL;
			return(hr);
		}
	}
	else
	{
		fopen_s(&gpFileAta, gszLogFileNameAta, "a+");
		fprintf_s(gpFileAta, "D3DCompile() succeeded for pixel shader: \n");
		fclose(gpFileAta);
	}
	hr = gpID3D11Device->CreatePixelShader(pID3DBlob_pixelShaderCode->GetBufferPointer(),
		pID3DBlob_pixelShaderCode->GetBufferSize(), NULL, &gpID3D11PixelShader);
	if (FAILED(hr))
	{

		fopen_s(&gpFileAta, gszLogFileNameAta, "a+");
		fprintf_s(gpFileAta, "ID3D11Device::CreatePixelShader() Failed for vertex shader\n");
		fclose(gpFileAta);
		return(hr);
	}
	else
	{
		fopen_s(&gpFileAta, gszLogFileNameAta, "a+");
		fprintf_s(gpFileAta, "ID3D11Device::CreatePixelShader() succeeded for vertex shader: \n");
		fclose(gpFileAta);
	}
	gpID3D11DeviceContext->PSSetShader(gpID3D11PixelShader, 0, 0);
	pID3DBlob_pixelShaderCode->Release();
	pID3DBlob_pixelShaderCode = NULL;

	float triangleVerticesAta[] =
	{
		// Front face
		0.0f, 1.0f, 0.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,

		// Right face
		0.0f, 1.0f, 0.0f,
		1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, -1.0f,

		// Back face
		0.0f, 1.0f, 0.0f,
		-1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, 1.0f,

		// Left face
		0.0f, 1.0f, 0.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, 1.0f

	};
	//Create vertex buffer
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = sizeof(float) * ARRAYSIZE(triangleVerticesAta);
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	hr = gpID3D11Device->CreateBuffer(&bufferDesc, NULL,
		&gpID3D11Buffer_PyramidVertexBufferAta);
	if (FAILED(hr))
	{
		fopen_s(&gpFileAta, gszLogFileNameAta, "a+");
		fprintf_s(gpFileAta, "ID3D11Device::CreateBuffer() Failed for vertex buffer.\n");
		fclose(gpFileAta);
		return(hr);
	}
	else
	{
		fopen_s(&gpFileAta, gszLogFileNameAta, "a+");
		fprintf_s(gpFileAta, "ID3D11Device::CreateBuffer() succeeded for vertex buffer.\n");
		fclose(gpFileAta);
	}

	//Copy vertices into above buffer
	D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	ZeroMemory(&mappedSubresource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	gpID3D11DeviceContext->Map(gpID3D11Buffer_PyramidVertexBufferAta, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &mappedSubresource);
	memcpy(mappedSubresource.pData, triangleVerticesAta, sizeof(triangleVerticesAta));
	gpID3D11DeviceContext->Unmap(gpID3D11Buffer_PyramidVertexBufferAta, NULL);

	const float triangleColorsAta[] = {
		// Front face
		0.0f, 0.447214f, -0.894427f,
		0.0f, 0.447214f, -0.894427f,
		0.0f, 0.447214f, -0.894427f,

		// right
		0.894427f, 0.447214f, 0.0f,
		0.894427f, 0.447214f, 0.0f,
		0.894427f, 0.447214f, 0.0f,

		// back
		0.0f, 0.447214f, 0.894427f,
		0.0f, 0.447214f, 0.894427f,
		0.0f, 0.447214f, 0.894427f,

		// left
		-0.894427f, 0.447214f, 0.0f,
		-0.894427f, 0.447214f, 0.0f,
		-0.894427f, 0.447214f, 0.0f
	};

	//for color
	D3D11_BUFFER_DESC bufferDescColors;
	ZeroMemory(&bufferDescColors, sizeof(D3D11_BUFFER_DESC));
	bufferDescColors.Usage = D3D11_USAGE_DYNAMIC;
	bufferDescColors.ByteWidth = sizeof(float) * ARRAYSIZE(triangleColorsAta);
	bufferDescColors.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDescColors.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	hr = gpID3D11Device->CreateBuffer(&bufferDescColors, NULL,
		&gpID3D11Buffer_PyramidColorBufferAta);
	if (FAILED(hr))
	{
		fopen_s(&gpFileAta, gszLogFileNameAta, "a+");
		fprintf_s(gpFileAta, "ID3D11Device::CreateBuffer() Failed for color buffer.\n");
		fclose(gpFileAta);
		return(hr);
	}
	else
	{
		fopen_s(&gpFileAta, gszLogFileNameAta, "a+");
		fprintf_s(gpFileAta, "ID3D11Device::CreateBuffer() succeeded for color buffer.\n");
		fclose(gpFileAta);
	}
	//Copy color info into the color buffer	
	D3D11_MAPPED_SUBRESOURCE mappedSubresourceColorsTriangle;
	ZeroMemory(&mappedSubresourceColorsTriangle, sizeof(D3D11_MAPPED_SUBRESOURCE));
	gpID3D11DeviceContext->Map(gpID3D11Buffer_PyramidColorBufferAta, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresourceColorsTriangle);
	memcpy(mappedSubresourceColorsTriangle.pData, triangleColorsAta, sizeof(triangleColorsAta));
	gpID3D11DeviceContext->Unmap(gpID3D11Buffer_PyramidColorBufferAta, 0);


	//create and set input layout
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[2];
	inputElementDesc[0].SemanticName = "POSITION";
	inputElementDesc[0].SemanticIndex = 0;
	inputElementDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDesc[0].InputSlot = 0;
	inputElementDesc[0].AlignedByteOffset = 0;
	inputElementDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	inputElementDesc[0].InstanceDataStepRate = 0;

	inputElementDesc[1].SemanticName = "NORMAL";
	inputElementDesc[1].SemanticIndex = 0;
	inputElementDesc[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDesc[1].InputSlot = 1;
	inputElementDesc[1].AlignedByteOffset = 0;
	inputElementDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	inputElementDesc[1].InstanceDataStepRate = 0;


	hr = gpID3D11Device->CreateInputLayout(inputElementDesc, 2, pID3DBlob_vertexShaderCode->GetBufferPointer(),
		pID3DBlob_vertexShaderCode->GetBufferSize(), &gpID3D11InputLayout);
	if (FAILED(hr))
	{
		fopen_s(&gpFileAta, gszLogFileNameAta, "a+");
		fprintf_s(gpFileAta, "ID3D11Device::CreateInputLayout() Failed.\n");
		fclose(gpFileAta);
		return(hr);
	}
	else
	{
		fopen_s(&gpFileAta, gszLogFileNameAta, "a+");
		fprintf_s(gpFileAta, "ID3D11Device::CreateInputLayout() succeeded.\n");
		fclose(gpFileAta);
	}

	gpID3D11DeviceContext->IASetInputLayout(gpID3D11InputLayout);
	pID3DBlob_vertexShaderCode->Release();
	pID3DBlob_vertexShaderCode = NULL;

	//Define and set constant buffer
	D3D11_BUFFER_DESC bufferDesc_ConstantBuffer;
	ZeroMemory(&bufferDesc_ConstantBuffer, sizeof(D3D11_BUFFER_DESC));
	bufferDesc_ConstantBuffer.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc_ConstantBuffer.ByteWidth = sizeof(CBUFFER);
	bufferDesc_ConstantBuffer.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	hr = gpID3D11Device->CreateBuffer(&bufferDesc_ConstantBuffer, nullptr, &gpID3D11Buffer_ConstantBuffer);
	if (FAILED(hr))
	{
		fopen_s(&gpFileAta, gszLogFileNameAta, "a+");
		fprintf_s(gpFileAta, "ID3D11Device::CreateBuffer() Failed for constant buffer.\n");
		fclose(gpFileAta);
		return(hr);
	}
	else
	{
		fopen_s(&gpFileAta, gszLogFileNameAta, "a+");
		fprintf_s(gpFileAta, "ID3D11Device::CreateBuffer() succeeded for constant buffer.\n");
		fclose(gpFileAta);
	}

	gpID3D11DeviceContext->VSSetConstantBuffers(0, 1, &gpID3D11Buffer_ConstantBuffer);
	gpID3D11DeviceContext->PSSetConstantBuffers(0, 1, &gpID3D11Buffer_ConstantBuffer);
	//back face culling off

	D3D11_RASTERIZER_DESC rasterizerDesc;
	ZeroMemory(&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
	rasterizerDesc.AntialiasedLineEnable = FALSE;
	rasterizerDesc.CullMode = D3D11_CULL_NONE;
	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.DepthBiasClamp = 0.0f;
	rasterizerDesc.DepthClipEnable = TRUE;
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.FrontCounterClockwise = FALSE;
	rasterizerDesc.MultisampleEnable = FALSE;
	rasterizerDesc.ScissorEnable = FALSE;
	rasterizerDesc.SlopeScaledDepthBias = 0.0f;

	hr = gpID3D11Device->CreateRasterizerState(&rasterizerDesc, &gpID3DRasterizerState);
	if (FAILED(hr))
	{
		fopen_s(&gpFileAta, gszLogFileNameAta, "a+");
		fprintf_s(gpFileAta, "ID3D11Device::CreateRasterState() Failed for Culling.\n");
		fclose(gpFileAta);
		return(hr);
	}
	else
	{
		fopen_s(&gpFileAta, gszLogFileNameAta, "a+");
		fprintf_s(gpFileAta, "ID3D11Device::CreateRasterState() Succeeded for Culling..\n");
		fclose(gpFileAta);
	}

	gpID3D11DeviceContext->RSSetState(gpID3DRasterizerState);


	gClearColorAta[0] = 0.0f; //R
	gClearColorAta[1] = 0.0f; //G
	gClearColorAta[2] = 0.0f; //B
	gClearColorAta[3] = 1.0f; //A
	PerspectiveProjectionMatrixAta = XMMatrixIdentity();


	//call resize for first time. Mandotory part of initialization code
	hr = resize(WIN_WIDTH_ATA, WIN_HEIGHT_ATA);
	if (FAILED(hr))
	{
		fopen_s(&gpFileAta, gszLogFileNameAta, "a+");
		fprintf_s(gpFileAta, "resize() failed. Exiting \n");
		fclose(gpFileAta);
	}
	else
	{
		fopen_s(&gpFileAta, gszLogFileNameAta, "a+");
		fprintf_s(gpFileAta, "resize() Succeeded. \n");
		fclose(gpFileAta);
	}
	return(S_OK);

}

HRESULT resize(int width, int height)
{
	HRESULT hr = S_OK;

	//free any size dependant resources
	if (gpID3D11DepthStencilView)
	{
		gpID3D11DepthStencilView->Release();
		gpID3D11DepthStencilView = NULL;
	}
	if (gpID3D11RenderTargetView)
	{
		gpID3D11RenderTargetView->Release();
		gpID3D11RenderTargetView = NULL;
	}

	//resize the swap chain buffer accordingly
	gpIDXGISwapChain->ResizeBuffers(1,
		width,
		height,
		DXGI_FORMAT_R8G8B8A8_UNORM,
		0);

	//again get back buffer from swap chain
	ID3D11Texture2D *pID3D11Texture2D_BackBuffer = NULL;
	gpIDXGISwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pID3D11Texture2D_BackBuffer);

	//again get the render target view from d3d11 device using above buffer
	hr = gpID3D11Device->CreateRenderTargetView(pID3D11Texture2D_BackBuffer, NULL,
		&gpID3D11RenderTargetView);
	if (FAILED(hr))
	{
		fopen_s(&gpFileAta, gszLogFileNameAta, "a+");
		fprintf_s(gpFileAta, "D3D11Device::CreateRenderTargetView() failed. Exiting \n");
		fclose(gpFileAta);
	}
	else
	{
		fopen_s(&gpFileAta, gszLogFileNameAta, "a+");
		fprintf_s(gpFileAta, "D3D11Device::CreateRenderTargetView() Succeeded. \n");
		fclose(gpFileAta);
	}
	pID3D11Texture2D_BackBuffer->Release();
	pID3D11Texture2D_BackBuffer = NULL;

	//create depth stencil buffer
	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory(&textureDesc, sizeof(D3D11_TEXTURE2D_DESC));
	textureDesc.Width = (UINT)width;
	textureDesc.Height = (UINT)height;
	textureDesc.ArraySize = 1;
	textureDesc.MipLevels = 1;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Format = DXGI_FORMAT_D32_FLOAT;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;
	ID3D11Texture2D *pID3D11Texture2D_DepthBuffer;
	hr = gpID3D11Device->CreateTexture2D(&textureDesc, NULL, &pID3D11Texture2D_DepthBuffer);

	//create depth stencil view from above depth stencil buffer
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	depthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;

	hr = gpID3D11Device->CreateDepthStencilView(pID3D11Texture2D_DepthBuffer, &depthStencilViewDesc, &gpID3D11DepthStencilView);
	if (FAILED(hr))
	{
		fopen_s(&gpFileAta, gszLogFileNameAta, "a+");
		fprintf_s(gpFileAta, "D3D11Device::CreateDepthStencilView() failed. Exiting \n");
		fclose(gpFileAta);
	}
	else
	{
		fopen_s(&gpFileAta, gszLogFileNameAta, "a+");
		fprintf_s(gpFileAta, "D3D11Device::CreateDepthStencilView() Succeeded. \n");
		fclose(gpFileAta);
	}
	pID3D11Texture2D_DepthBuffer->Release();
	pID3D11Texture2D_DepthBuffer = NULL;
	gpID3D11DeviceContext->OMSetRenderTargets(1, &gpID3D11RenderTargetView, gpID3D11DepthStencilView);

	//similar to glViewPort, set D3D view port
	D3D11_VIEWPORT d3dViewPort;
	d3dViewPort.TopLeftX = 0;
	d3dViewPort.TopLeftY = 0;
	d3dViewPort.Width = (float)width;
	d3dViewPort.Height = (float)height;
	d3dViewPort.MinDepth = 0.0f;
	d3dViewPort.MaxDepth = 1.0f;

	gpID3D11DeviceContext->RSSetViewports(1, &d3dViewPort);

	PerspectiveProjectionMatrixAta = XMMatrixPerspectiveFovLH(XMConvertToRadians(45.0f), ((float)width / (float)height), 0.1f, 100.0f);

	return(hr);
}

void display()
{
	//clear render target view to a chosen color
	gpID3D11DeviceContext->ClearRenderTargetView(gpID3D11RenderTargetView, gClearColorAta);
	gpID3D11DeviceContext->ClearDepthStencilView(gpID3D11DepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
	//D3D drawing goes here
	// select which vertex buffer to display
	UINT stride = sizeof(float) * 3;
	UINT offset = 0;

	//draw triangle
	XMMATRIX worldMatrixAta = XMMatrixIdentity();
	XMMATRIX viewMatrixAta = XMMatrixIdentity();
	XMMATRIX scaleMatrix = XMMatrixIdentity();
	XMMATRIX rotationXMatrix = XMMatrixIdentity();
	XMMATRIX rotationYMatrix = XMMatrixIdentity();
	XMMATRIX rotationZMatrix = XMMatrixIdentity();
	XMMATRIX rotationMatrix = XMMatrixIdentity();
	XMMATRIX translationMatrix = XMMatrixTranslation(0.0f, 0.0f, 6.0f);

	//vertex
	gpID3D11DeviceContext->IASetVertexBuffers(0, 1, &gpID3D11Buffer_PyramidVertexBufferAta, &stride, &offset);
	//color
	gpID3D11DeviceContext->IASetVertexBuffers(1, 1, &gpID3D11Buffer_PyramidColorBufferAta, &stride, &offset);

	//select geometry primitive
	gpID3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//Model translation and viewing
	rotationXMatrix = XMMatrixRotationX(XMConvertToRadians(angleSquareAta));
	rotationYMatrix = XMMatrixRotationY(XMConvertToRadians(angleSquareAta));
	rotationZMatrix = XMMatrixRotationZ(XMConvertToRadians(angleSquareAta));
	rotationMatrix = rotationYMatrix;

	scaleMatrix = XMMatrixScaling(0.75f, 0.75f, 0.75f);

	worldMatrixAta = rotationMatrix * translationMatrix;

	// load the data into constant buffer
	CBUFFER constantBuffer;
	constantBuffer.WorldMatrixAta = worldMatrixAta;
	constantBuffer.ViewMatrixAta = viewMatrixAta;
	constantBuffer.ProjectionMatrixAta = PerspectiveProjectionMatrixAta;

	if (gbLight == true)
	{
		constantBuffer.laZero = XMVectorSet(light0Ambient[0], light0Ambient[1], light0Ambient[2], light0Ambient[3]);
		constantBuffer.ldZero = XMVectorSet(light0Diffuse[0], light0Diffuse[1], light0Diffuse[2], light0Diffuse[3]);
		constantBuffer.lsZero = XMVectorSet(light0Specular[0], light0Specular[1], light0Specular[2], light0Specular[3]);
		constantBuffer.laOne = XMVectorSet(light1Ambient[0], light1Ambient[1], light1Ambient[2], light1Ambient[3]);
		constantBuffer.ldOne = XMVectorSet(light1Diffuse[0], light1Diffuse[1], light1Diffuse[2], light1Diffuse[3]);
		constantBuffer.lsOne = XMVectorSet(light1Specular[0], light1Specular[1], light1Specular[2], light1Specular[3]);
		constantBuffer.ka = XMVectorSet(material_ambient[0], material_ambient[1], material_ambient[2], material_ambient[3]);
		constantBuffer.kd = XMVectorSet(material_diffuse[0], material_diffuse[1], material_diffuse[2], material_diffuse[3]);
		constantBuffer.ks = XMVectorSet(material_specular[0], material_specular[1], material_specular[2], material_specular[3]);
		constantBuffer.lightZeroPosition = XMVectorSet(light0Position[0], light0Position[1], light0Position[2], light0Position[3]);
		constantBuffer.lightOnePosition = XMVectorSet(light1Position[0], light1Position[1], light1Position[2], light1Position[3]);
		constantBuffer.lKeyPressed = 1;
		constantBuffer.materialShininess = material_shininess;
	}
	else
	{
		constantBuffer.lKeyPressed = 0;
	}
	gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_ConstantBuffer, 0, NULL, &constantBuffer, 0, 0);

	gpID3D11DeviceContext->Draw(12, 0);
	//Swap front and back buffers
	gpIDXGISwapChain->Present(0, 0);
}
void update()
{
	if (angleSquareAta > 360.0f)
	{
		angleSquareAta = 0.0f;
	}
	else
		angleSquareAta = angleSquareAta + 0.2f;
}
void uninitialize()
{
	if (gpID3D11Buffer_ConstantBuffer)
	{
		gpID3D11Buffer_ConstantBuffer->Release();
		gpID3D11Buffer_ConstantBuffer = NULL;
	}
	if (gpID3D11InputLayout)
	{
		gpID3D11InputLayout->Release();
		gpID3D11InputLayout = NULL;
	}
	if (gpID3D11Buffer_PyramidVertexBufferAta)
	{
		gpID3D11Buffer_PyramidVertexBufferAta->Release();
		gpID3D11Buffer_PyramidVertexBufferAta = NULL;
	}
	if (gpID3D11Buffer_PyramidColorBufferAta)
	{
		gpID3D11Buffer_PyramidColorBufferAta->Release();
		gpID3D11Buffer_PyramidColorBufferAta = NULL;
	}
	if (gpID3D11Buffer_CubeVertexBufferAta)
	{
		gpID3D11Buffer_CubeVertexBufferAta->Release();
		gpID3D11Buffer_CubeVertexBufferAta = NULL;
	}
	if (gpID3D11Buffer_CubeColorBufferAta)
	{
		gpID3D11Buffer_CubeColorBufferAta->Release();
		gpID3D11Buffer_CubeColorBufferAta = NULL;
	}
	if (gpID3D11PixelShader)
	{
		gpID3D11PixelShader->Release();
		gpID3D11PixelShader = NULL;
	}
	if (gpID3D11VertexShader)
	{
		gpID3D11VertexShader->Release();
		gpID3D11VertexShader = NULL;
	}
	if (gpID3D11DepthStencilView)
	{
		gpID3D11DepthStencilView->Release();
		gpID3D11DepthStencilView = NULL;
	}
	if (gpID3D11RenderTargetView)
	{
		gpID3D11RenderTargetView->Release();
		gpID3D11RenderTargetView = NULL;
	}

	if (gpIDXGISwapChain)
	{
		gpIDXGISwapChain->Release();
		gpIDXGISwapChain = NULL;
	}
	if (gpID3D11DeviceContext)
	{
		gpID3D11DeviceContext->Release();
		gpID3D11DeviceContext = NULL;
	}
	if (gpID3D11Device)
	{
		gpID3D11Device->Release();
		gpID3D11Device = NULL;
	}
	if (gpFileAta)
	{
		fopen_s(&gpFileAta, gszLogFileNameAta, "a+");
		fprintf_s(gpFileAta, "uninitialize() success. \n");
		fprintf_s(gpFileAta, "Log file is successfully closed. \n");
		fclose(gpFileAta);
	}
}
