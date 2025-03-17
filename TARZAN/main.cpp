#include "stdafx.h"
#include "Framework/Core/Engine.h"
#include <iostream>


void RedirectIOToConsole() {
    // �ܼ� â ����
    AllocConsole();

    // ǥ�� ����� ��Ʈ���� �ֿܼ� ����
    FILE* fp;
    freopen_s(&fp, "CONOUT$", "w", stdout);  // �ܼ� ���
    freopen_s(&fp, "CONOUT$", "w", stderr); // �ܼ� ���� ���
    freopen_s(&fp, "CONIN$", "r", stdin);   // �ܼ� �Է�

    std::cout << "Debug Console Initialized!" << std::endl; // ����� �ܼ� �޽��� ���
}


void RedirectIOToConsole()
{
#ifdef _DEBUG
    RedirectIOToConsole(); // ����� ��忡���� �ܼ��� ��쵵�� ����
#endif

	UEngine& Engine = UEngine::GetInstance();

    // ǥ�� ����� ��Ʈ���� �ֿܼ� ����
    FILE* fp;
    freopen_s(&fp, "CONOUT$", "w", stdout);  // �ܼ� ���
    freopen_s(&fp, "CONOUT$", "w", stderr); // �ܼ� ���� ���
    freopen_s(&fp, "CONIN$", "r", stdin);   // �ܼ� �Է�

    std::cout << "Debug Console Initialized!" << std::endl; // ����� �ܼ� �޽��� ���
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
#ifdef _DEBUG
    RedirectIOToConsole(); // ����� ��忡���� �ܼ��� ��쵵�� ����
#endif

#ifdef _DEBUG
    FreeConsole();  // �ܼ� â �ݱ�
#endif


	return 0;
}


