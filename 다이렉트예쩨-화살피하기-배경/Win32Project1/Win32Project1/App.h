#pragma once
#include "windows.h"
#include "atlstr.h"
#include <d3dx9.h>

#include "SceneManager.h"
#include "ResourceManager.h"
#include "InputManager.h"

class App
{
private:
	static LARGE_INTEGER LInterval, RInterval, Frequency;

	static App *instance;
	static void InitWindow();					// �����츦 �ʱ�ȭ
	static void FloatWindow(int nCmdShow);		// �����츦 ���
	static void InitD3D();						// DX ��ġ����
	static CString title;

	static const int WIDTH = 800;
	static const int HEIGHT = 600;
public:
	App();
	~App();

	static HINSTANCE hInstance;
	static HWND hWnd;
	static LPDIRECT3D9 g_pD3D;					// �׷���ī�带 ����
	static LPDIRECT3DDEVICE9 g_pd3dDevice;		// ������ �׷���ī�� ������ �޾Ƽ� ����� �� �ְ� ���ִ� ����
	static LPD3DXSPRITE g_pSprite;				// �׷���ī��� �����ؼ� 2D�̹����� ����� �� �ְ� ����
	static D3DPRESENT_PARAMETERS d3dpp;			// ȭ������� ���λ����� �������

	static CSceneManager *g_pSceneManager;
	static CResourceManager *g_pResourceManager;
	static CInputManager *g_pInputManager;

	static int DoLoop();						// �޽����� ó���ϸ� ���ѷ���
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam); // �޽��� ó�� ���ν���

	static void Initialize(HINSTANCE inst, int nCmdShow, CString str);
	static void Destroy();

	static App* GetInstance();

	static float GetElapsedTime();
};

