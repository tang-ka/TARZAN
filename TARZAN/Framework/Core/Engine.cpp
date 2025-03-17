#include "stdafx.h"
#include "Engine.h"

#include "Framework/Core/UGizmoComponent.h"
#include "Framework/Core/CRenderer.h"
#include "UWorld.h"
#include "UWorldGridComponent.h"
#include "Framework/Core/SceneManager.h"
#include "ConfigManager.h"
#include "GuiController.h"

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
		InitEditor();
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

		Controller->NewFrame(); // GUI 새 프레임 생성
		SceneManager->GetWorld()->Update(); // 월드 액터 업데이트

		/* 카메라 관련 업데이트 */
		CRenderer::Instance()->GetMainCamera()->allowKeyboardInput = !Controller->GetConcolWindow()->OnFocusing();
		CRenderer::Instance()->GetMainCamera()->Update();
		CRenderer::Instance()->GetMainCamera()->Render();

		/* 렌더링 준비 */
		CRenderer::Instance()->GetGraphics()->RenderBegin();

		/* 월드 렌더링 */
		SceneManager->GetWorld()->Render();

		/* GUI 렌더링*/
		Controller->RenderEditor();

		/* 기즈모 업데이트*/
		UGizmo->Update();
		UGizmo->Render();

		// 동적으로 변경하려면 여기에 따로 WorldGrid->Update를 추가해야 할 것 같은데
		WorldGrid->UpdateGrid();
		WorldGrid->Render();

		/* 월드 Axis 렌더링*/
		Arrow->Render();
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
	return SceneManager->GetWorld();
}

UGizmoComponent* UEngine::GetGizmo()
{
	return UGizmo;
}

USceneManager* UEngine::GetSceneManager()
{
	return SceneManager;
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

void UEngine::InitEditor()
{
	// editor.ini 파일로부터 설정값을 로드
	ConfigManager::GetInstance().LoadConfig();

	// Camera Orthgonal/Perspective
	//GuiController::GetInstance()._selectedMode = ConfigManager::GetInstance().GetEditorConfig().bIsOrthogonal ? 1 : 0;

	if (ConfigManager::GetInstance().GetEditorConfig().WorldGridScale == 0.1f)
		GuiController::GetInstance()._selectedGridScale = 0;
	else if (ConfigManager::GetInstance().GetEditorConfig().WorldGridScale == 1.0f)
		GuiController::GetInstance()._selectedGridScale = 1;
	else if (ConfigManager::GetInstance().GetEditorConfig().WorldGridScale == 10.0f)
		GuiController::GetInstance()._selectedGridScale = 2;

	// Camera Speed
	if (ConfigManager::GetInstance().GetEditorConfig().CameraSpeed == 1.f) GuiController::GetInstance()._selectedCameraSpeed = 0;
	else if (ConfigManager::GetInstance().GetEditorConfig().CameraSpeed == 2.f) GuiController::GetInstance()._selectedCameraSpeed = 1;
	else if (ConfigManager::GetInstance().GetEditorConfig().CameraSpeed == 5.f) GuiController::GetInstance()._selectedCameraSpeed = 2;
	else if (ConfigManager::GetInstance().GetEditorConfig().CameraSpeed == 10.f) GuiController::GetInstance()._selectedCameraSpeed = 3;

	// ImGui에 반영될 grid scale 값을 업데이트
	// 여기서 왜 값이 안박히지
	//GuiController::GetInstance().SetSelectedGridScale(ConfigManager::GetInstance().GetEditorConfig().WorldGridScale);

	// UWorldGridComponent 업데이트: 현재 카메라 위치를 기준으로 grid를 재생성
	UWorldGridComponent* grid = UEngine::GetInstance().GetWorldGridComponent();
	UCameraComponent* mainCam = CRenderer::Instance()->GetMainCamera();

	// TO-DO: fix
	if (grid && mainCam)
	{
		float camX = mainCam->GetRelativeLocation().x;
		float camZ = mainCam->GetRelativeLocation().z;
		mainCam->CameraSpeed = ConfigManager::GetInstance().GetEditorConfig().CameraSpeed;
		int gridCount = 10; // 예시 값, 필요에 따라 조정
		grid->GenerateGrid(0, 0, gridCount, ConfigManager::GetInstance().GetEditorConfig().WorldGridScale);
	}
	// TO-DO: fix
}
void UEngine::InitWorld()
{
	SceneManager = new USceneManager();
	SceneManager->Initialize();

	//Controller = new GuiController(WindowHandle, CRenderer::Instance()->GetGraphics());
	// UEngine::InitWorld() 내 수정 예시
	Controller = &GuiController::GetInstance();
	Controller->Init(WindowHandle, CRenderer::Instance()->GetGraphics());


	SceneManagerView* SceneView = new SceneManagerView(SceneManager);
	Controller->SceneView = SceneView;

	UGizmo = new UGizmoComponent();

	Arrow = new UCoordArrowComponent();
	Arrow->SetRelativeScale3D({ 50000,50000,50000 });

	// 근데 이렇게 하는게 맞나 싶음. 이거 동적으로 어차피 사용해야 하는데, WireFrame 설정하는 곳에서 추가로 Grid사이즈 정할까
	WorldGrid = new UWorldGridComponent();
}
