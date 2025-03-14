#include "stdafx.h"
#include "Framework/Core/Engine.h"

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) 
{
	UEngine& Engine = UEngine::GetInstance();

	Engine.Initialize(hInstance, L"TARZAN ENGINE", L"TARZAN", 1200, 800);

	Engine.Run();

	Engine.Shutdown();

	return 0;
} 