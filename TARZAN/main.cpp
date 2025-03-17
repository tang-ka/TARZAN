#include "stdafx.h"
#include "Framework/Core/Engine.h"
#include <iostream>


void RedirectIOToConsole()
{
    // 콘솔 창 생성
    AllocConsole();

    // 표준 입출력 스트림을 콘솔에 연결
    FILE* fp;
    freopen_s(&fp, "CONOUT$", "w", stdout);  // 콘솔 출력
    freopen_s(&fp, "CONOUT$", "w", stderr); // 콘솔 에러 출력
    freopen_s(&fp, "CONIN$", "r", stdin);   // 콘솔 입력

    std::cout << "Debug Console Initialized!" << std::endl; // 디버깅 콘솔 메시지 출력
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
#ifdef _DEBUG
    RedirectIOToConsole(); // 디버깅 모드에서만 콘솔을 띄우도록 설정
#endif

    UEngine& Engine = UEngine::GetInstance();

    Engine.Initialize(hInstance, L"TARZAN ENGINE", L"TARZAN", 1200, 800);

    Engine.Run();

    Engine.Shutdown();

#ifdef _DEBUG
    FreeConsole();  // 콘솔 창 닫기
#endif


    return 0;
}