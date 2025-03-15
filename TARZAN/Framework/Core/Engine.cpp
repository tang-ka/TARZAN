#include "stdafx.h"
#include "Engine.h"

#include "Framework/Core/UGizmoComponent.h"
#include "Framework/Core/CRenderer.h"
#include "UWorld.h"
#include "UWorldGridComponent.h"
#include "ConfigManager.h"
#include "GuiController.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT UEngine::WinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// ImGui�� �޽����� ó��
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
		InitEditor();
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

	ConfigManager::GetInstance().Shutdown();

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

	// Window Handle ����
	WindowHandle = CreateWindowExW(
		0, WindowClassName, WindowTitle,
		WS_POPUP | WS_VISIBLE | WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		InScreenWidth, InScreenHeight,
		nullptr, nullptr, WindowInstance, nullptr
	);

	// ������ ��Ŀ��
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

void UEngine::InitEditor()
{
	// editor.ini ���Ϸκ��� �������� �ε�
	ConfigManager::GetInstance().LoadConfig();
	if (ConfigManager::GetInstance().GetEditorConfig().WorldGridScale == 0.1f)
		GuiController::GetInstance()._selectedGridScale = 0;
	else if (ConfigManager::GetInstance().GetEditorConfig().WorldGridScale == 1.0f)
		GuiController::GetInstance()._selectedGridScale = 1;
	else if (ConfigManager::GetInstance().GetEditorConfig().WorldGridScale == 10.0f)
		GuiController::GetInstance()._selectedGridScale = 2;


	// ImGui�� �ݿ��� grid scale ���� ������Ʈ
	// ���⼭ �� ���� �ȹ�����
	//GuiController::GetInstance().SetSelectedGridScale(ConfigManager::GetInstance().GetEditorConfig().WorldGridScale);

	// UWorldGridComponent ������Ʈ: ���� ī�޶� ��ġ�� �������� grid�� �����
	UWorldGridComponent* grid = GetWorldGridComponent(); // getter ���� ����
	UCameraComponent* mainCam = CRenderer::Instance()->GetMainCamera();
	if (grid && mainCam)
	{
		float camX = mainCam->GetRelativeLocation().x;
		float camZ = mainCam->GetRelativeLocation().z;
		int gridCount = 10; // ���� ��, �ʿ信 ���� ����
		grid->GenerateGrid(camX, camZ, gridCount, ConfigManager::GetInstance().GetEditorConfig().WorldGridScale);
	}
	
}
//void UEngine::InitEditor()
//{
//	// editor.ini ���Ϸκ��� �������� �ε��մϴ�.
//	ConfigManager::GetInstance().LoadConfig();
//	FEditorConfig config = ConfigManager::GetInstance().GetEditorConfig();
//
//	// ImGui�� �ݿ��� Grid Scale ���� ������Ʈ�մϴ�.
//	// (0.1, 1, 10 �� �ϳ����� �մϴ�. �ٸ� ���� ��� �⺻�� 1�� ����մϴ�.)
//	if (config.WorldGridScale == 0.1f)
//		GuiController::GetInstance()._selectedGridScale = 0;
//	else if (config.WorldGridScale == 1.0f)
//		GuiController::GetInstance()._selectedGridScale = 1;
//	else if (config.WorldGridScale == 10.0f)
//		GuiController::GetInstance()._selectedGridScale = 2;
//	else
//		GuiController::GetInstance()._selectedGridScale = 1;
//
//	// UWorldGridComponent ������Ʈ: ���� ī�޶� ��ġ�� �������� grid�� ������մϴ�.
//	UWorldGridComponent* grid = GetWorldGridComponent();  // UEngine�� private ����� �����ϴ� getter
//	UCameraComponent* mainCam = CRenderer::Instance()->GetMainCamera();
//	if (grid && mainCam)
//	{
//		float camX = mainCam->GetRelativeLocation().x;
//		float camZ = mainCam->GetRelativeLocation().z;
//		int gridCount = 10; // gridCount�� ���� ���Դϴ�. �ʿ信 ���� �����ϼ���.
//
//		// ���� ���Ͽ��� �о�� WorldGridScale ���� ����Ͽ� grid�� �����մϴ�.
//		grid->GenerateGrid(camX, camZ, gridCount, config.WorldGridScale);
//	}
//
//	// (�ɼ�) ī�޶��� �ΰ��� �� �ٸ� ������ ������Ʈ �� �� �ֽ��ϴ�.
//	// if (mainCam)
//	// {
//	//     mainCam->SetCameraSensitivity(config.CameraSens);
//	// }
//}


void UEngine::InitWorld()
{
	Controller = new GuiController(WindowHandle, CRenderer::Instance()->GetGraphics());
	Controller->world = new UWorld();

	UGizmo = new UGizmoComponent();

	Arrow = new UCoordArrowComponent();
	Arrow->SetRelativeScale3D({ 50000,50000,50000 });

	// �ٵ� �̷��� �ϴ°� �³� ����. �̰� �������� ������ ����ؾ� �ϴµ�, WireFrame �����ϴ� ������ �߰��� Grid������ ���ұ�
	WorldGrid = new UWorldGridComponent();
	
	//WorldGrid->GenerateGrid(
	//	CRenderer::Instance()->GetMainCamera()->GetRelativeLocation().x, 
	//	CRenderer::Instance()->GetMainCamera()->GetRelativeLocation().z, 
	//	10, 
	//	ConfigManager::GetInstance().GetEditorConfig().WorldGridScale
	//);
}
