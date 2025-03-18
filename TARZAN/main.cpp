#include "stdafx.h"
#include "Framework/Core/Engine.h"
#include <iostream>

/// CRT_DBG
#define CRTDBG_MAP_ALLOC
#define new new(_CLIENT_BLOCK, __FILE__, __LINE__)
#include <stdlib.h>
#include <crtdbg.h>


int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
#ifdef _DEBUG
    _CrtSetBreakAlloc(12083);
#endif
    UEngine& Engine = UEngine::GetInstance();
    
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    Engine.Initialize(hInstance, L"TARZAN ENGINE", L"TARZAN", 1200, 800);

    Engine.Run();

    Engine.Shutdown();
    return 0;
}