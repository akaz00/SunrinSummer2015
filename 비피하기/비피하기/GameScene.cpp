#include "GameScene.h"
#include <iostream>
#include <Windows.h>
using namespace std;

CGameScene::CGameScene()
{
	system("cls");
	cout << "���Ӿ�";
}
CGameScene::~CGameScene()
{
}

void CGameScene::Input()
{
	cout << "���Ӿ� ��ǲ" << endl;
}
void CGameScene::Update()
{
	cout << "���Ӿ� ����" << endl;
}
void CGameScene::Render()
{
	cout << "���Ӿ� ����" << endl;
}
void CGameScene::Clear()
{
	cout << "���Ӿ� Ŭ��" << endl;
}