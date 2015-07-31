#include "App.h"
HINSTANCE App::hInstance;
HWND App::hWnd;
LPDIRECT3D9 App::g_pD3D = NULL;		//�׷���ī�带 ����
LPDIRECT3DDEVICE9 App::g_pd3dDevice; //������ �׷���ī�� ������ �޾Ƽ� ����� �� �ְ� ���ִ� ����
LPD3DXSPRITE App::g_pSprite;			//�׷���ī��� �����ؼ� 2D�̹����� ����� �� �ְ� ����
D3DPRESENT_PARAMETERS App::d3dpp;	//ȭ������� ���λ����� �������
CString App::title;
App *instance = NULL;

App::App()
{
	
}

App::~App()
{

}

void App::Initialize(HINSTANCE inst, int nCmdShow, CString str)
{
	title = str;
	InitWindow();
	FloatWindow(nCmdShow);
	InitD3D();
}

void App::Destroy()
{
	g_pd3dDevice->Release();
	g_pSprite->Release();
	g_pD3D->Release();
}

void App::InitWindow()
{
	WNDCLASS WndClass;
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hInstance = hInstance;
	WndClass.lpfnWndProc = WndProc;
	WndClass.lpszClassName = title;
	WndClass.lpszMenuName = NULL;
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClass(&WndClass);
}

void App::FloatWindow(int nCmdShow)
{
	hWnd = CreateWindow(title, title, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, WIDTH, HEIGHT,
		NULL, (HMENU)NULL, hInstance, NULL);
	ShowWindow(hWnd, nCmdShow);
}

void App::InitD3D()
{
	g_pD3D = Direct3DCreate9(D3D_SDK_VERSION);
	ZeroMemory(&d3dpp, sizeof(d3dpp));

	
	d3dpp.Windowed = true;
	d3dpp.SwapEffect             = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat       = D3DFMT_UNKNOWN;
	d3dpp.BackBufferWidth	     = WIDTH;
	d3dpp.BackBufferHeight		 = HEIGHT;
	d3dpp.BackBufferCount        = 1;
	
	/*
	d3dpp.Windowed = false;
	d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
	d3dpp.FullScreen_RefreshRateInHz = 60;
	*/

	g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&d3dpp, &g_pd3dDevice);

	D3DXCreateSprite(g_pd3dDevice, &g_pSprite);
}

int App::DoLoop()
{
	MSG Message = { 0 };
	while (Message.message != WM_QUIT)
	{
		if (PeekMessage(&Message, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&Message);
			DispatchMessage(&Message);
		}
		else
		{
			if (FAILED(g_pd3dDevice->TestCooperativeLevel()))
				g_pd3dDevice->Reset(&d3dpp);

			g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET,
				D3DCOLOR_ARGB(255, 0, 0, 0), 1.0f, 0);
			//	������Ʈ ��ġ

			if (SUCCEEDED(g_pd3dDevice->BeginScene()))
			{
				//	���� ��ġ
				g_pd3dDevice->EndScene();
			}
			g_pd3dDevice->Present(NULL, NULL, NULL, NULL);
		}
	}
	return (int)Message.wParam;
}

LRESULT CALLBACK App::WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	switch (iMessage)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}
