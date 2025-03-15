#include "stdafx.h"
#include "Engine.h"

#include "Framework/Core/UGizmoComponent.h"
#include "Framework/Core/CRenderer.h"
#include "UWorld.h"
#include "UWorldGridComponent.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT UEngine::WinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// ImGui의 메시지를 처리
	if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
	{
		return true;
	}

	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_LBUTTONDOWN:
		if (UEngine::GetInstance().GetWorld())
		{
			auto cam = CRenderer::Instance()->GetMainCamera();
			Input::Instance()->SpawnMouseRay(cam->View(), cam->PerspectiveProjection());
		}
		break;
	case WM_MOUSEMOVE:
	{
		TRACKMOUSEEVENT tme = {};
		tme.cbSize = sizeof(TRACKMOUSEEVENT);
		tme.dwFlags = TME_LEAVE;
		tme.hwndTrack = hWnd;
		tme.dwHoverTime = 0;
		TrackMouseEvent(&tme);
		break;
	}
	case WM_MOUSELEAVE:
	{
		break;
	}

	case WM_SIZE:
	{
		if (CRenderer::Instance()->GetGraphics() && CRenderer::Instance()->GetGraphics()->GetDevice() && CRenderer::Instance()->GetGraphics()->GetDeviceContext())
		{
			SCR_WIDTH = LOWORD(lParam);
			SCR_HEIGHT = HIWORD(lParam);
			CRenderer::Instance()->GetGraphics()->ResizeBuffers(SCR_WIDTH, SCR_HEIGHT);
			UCameraComponent* camera = CRenderer::Instance()->GetMainCamera();

			if (camera)
			{
				float aspectRatio = static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT);
				camera->UpdateRatio(aspectRatio);
			}

			if (GuiController* GUI = UEngine::GetInstance().GetGUI())
			{
				GUI->Resize();
			}

			Input::Instance()->ResizeScreen(SCR_WIDTH, SCR_HEIGHT);

		}
		break;
	}
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void UEngine::Initialize(HINSTANCE hInstance, const WCHAR* InWindowTitle, const WCHAR* InWindowClassName, int InScreenWidth, int InScreenHeight, EScreenMode InScreenMode)
{
	WindowClassName = InWindowClassName;
	WindowTitle = InWindowTitle;
	WindowInstance = hInstance;
	ScreenMode = InScreenMode;


	if (InitWindow(SCR_WIDTH, SCR_HEIGHT))
	{
		InitGlobal();

		InitWorld();
	}
}

void UEngine::Run()
{
	IsRunning = true;

	while (IsRunning)
	{
		Time::Instance()->_query_frame_update_time();

		MSG msg;
		
		while( (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) )
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT)
			{
				IsRunning = false;
				break;
			}
		}

	


		Input::Instance()->Frame();

		Controller->NewFrame();
		Controller->world->Update();

		CRenderer::Instance()->GetMainCamera()->allowKeyboardInput = !Controller->GetConcolWindow()->OnFocusing();
		CRenderer::Instance()->GetMainCamera()->Update();
		CRenderer::Instance()->GetMainCamera()->Render();
		CRenderer::Instance()->GetGraphics()->RenderBegin();

		Controller->world->Render();
		UGizmo->Update();
		Controller->RenderEditor();
		UGizmo->Render();
		Arrow->Render();
		// 동적으로 변경하려면 여기에 따로 WorldGrid->Update를 추가해야 할 것 같은데
		WorldGrid->UpdateGrid();
		WorldGrid->Render();
		Controller->RenderFrame();

		CRenderer::Instance()->GetGraphics()->RenderEnd();
		Time::Instance()->_query_frame_end_time();
	}
}

void UEngine::Shutdown()
{
	Input::Instance()->Shutdown();
	CRenderer::Release();
}

GuiController* UEngine::GetGUI()
{
	return Controller;
}

UWorld* UEngine::GetWorld()
{
	return World;
}

UGizmoComponent* UEngine::GetGizmo()
{
	return UGizmo;
}

bool UEngine::InitWindow(int32 InScreenWidth, int32 InScreenHeight)
{
	// Register Window Class
	WNDCLASS winClass = {};
	winClass.hInstance = WindowInstance;
	winClass.lpszClassName = WindowClassName;
	winClass.lpfnWndProc = WinProc;
	RegisterClass(&winClass);

	// Window Handle 생성
	WindowHandle = CreateWindowExW(
		0, WindowClassName, WindowTitle,
		WS_POPUP | WS_VISIBLE | WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		InScreenWidth, InScreenHeight,
		nullptr, nullptr, WindowInstance, nullptr
	);

	// 윈도우 포커싱
	ShowWindow(WindowHandle, SW_SHOW);
	SetForegroundWindow(WindowHandle);
	SetFocus(WindowHandle);

	if (WindowHandle)
	{
		return true;
	}

	return false;
}

void UEngine::InitGlobal()
{
	CRenderer::Instance()->Init(WindowHandle);
	Time::Instance()->Init();
	Input::Instance()->Init(WindowInstance, WindowHandle, SCR_WIDTH, SCR_HEIGHT);
}

void UEngine::InitWorld()
{
	Controller = new GuiController(WindowHandle, CRenderer::Instance()->GetGraphics());
	Controller->world = new UWorld();

	UGizmo = new UGizmoComponent();

	Arrow = new UCoordArrowComponent();
	Arrow->SetRelativeScale3D({ 50000,50000,50000 });

	// 근데 이렇게 하는게 맞나 싶음. 이거 동적으로 어차피 사용해야 하는데, WireFrame 설정하는 곳에서 추가로 Grid사이즈 정할까
	WorldGrid = new UWorldGridComponent();
	
	WorldGrid->GenerateGrid(CRenderer::Instance()->GetMainCamera()->GetRelativeLocation().x, CRenderer::Instance()->GetMainCamera()->GetRelativeLocation().z, 10, 1.f);
}
