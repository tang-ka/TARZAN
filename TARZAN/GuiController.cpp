#include "stdafx.h"
#include "GuiController.h"
#include "Framework/DirectXWrapper/CGraphics.h"
#include "Framework/Core/CEngineStatics.h"
#include "Framework/Core/Time.h"
#include "Framework/Core/UCubeComponent.h"
#include "Framework/Core/UGizmoComponent.h"
#include "Framework/Core/Engine.h"
#include "UWorld.h"


GuiController::GuiController(HWND hWnd, CGraphics* graphics): hWnd(hWnd) {
	IMGUI_CHECKVERSION();
	_context = ImGui::CreateContext();
	_io = &ImGui::GetIO();
	ImGui_ImplDX11_Init(graphics->GetDevice(), graphics->GetDeviceContext());
	ImGui_ImplWin32_Init(hWnd);
	_console = new GuiConsole(this);
}

GuiController::~GuiController() {
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void GuiController::NewFrame()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	ImGui::Begin("MousePos");
	POINT cursorPos;
	GetCursorPos(&cursorPos);
	ScreenToClient(hWnd, &cursorPos);
	ImGui::Text("X : %d, Y : %d", cursorPos.x, cursorPos.y);
	static RECT rect;
	GetClientRect(hWnd, &rect);
	ImGui::Text("Width : %d, Height : %d", rect.right - rect.left, rect.bottom - rect.top);
	ImGui::End();
	int x, y, w;
	Input::Instance()->GetMouseLocation(x, y);
	Input::Instance()->GetMouseWheel(w);
	_io->MousePos.x = static_cast<float>(x);
	_io->MousePos.y = static_cast<float>(y);
	_io->MouseWheel = static_cast<float>(w);
	_io->MouseDown[0] = Input::Instance()->IsMouseButtonDown(0);
	_io->MouseDown[1] = Input::Instance()->IsMouseButtonDown(1);

	if (Input::Instance()->IsMouseButtonPressed(0) && !_io->WantCaptureMouse) {
		float nearestActorDistance;
		float nearestGizmoDistance;
		UActorComponent* neareastActorComp = GetNearestActorComponents(nearestActorDistance);
		EPrimitiveColor neareastAxis = GetNearestGizmo(nearestGizmoDistance);

		if (neareastActorComp == nullptr && neareastAxis == EPrimitiveColor::NONE) {
			UEngine::GetInstance().GetGizmo()->Detach();
			UPrimitiveComponent* downcast = dynamic_cast<UPrimitiveComponent*>(_selected);
			if (downcast)
				downcast->renderFlags &= ~PRIMITIVE_FLAG_SELECTED;
			_selected = nullptr;
		}
		else {
			if (nearestActorDistance < nearestGizmoDistance) {

				UPrimitiveComponent* downcast = dynamic_cast<UPrimitiveComponent*>(_selected);
				if (downcast)
					downcast->renderFlags &= ~PRIMITIVE_FLAG_SELECTED;

				_selected = neareastActorComp;
				downcast = dynamic_cast<UPrimitiveComponent*>(_selected);
				if (downcast)
					downcast->renderFlags |= PRIMITIVE_FLAG_SELECTED;

				UEngine::GetInstance().GetGizmo()->AttachTo(dynamic_cast<UPrimitiveComponent*>(_selected));
				UEngine::GetInstance().GetGizmo()->selectedAxis = EPrimitiveColor::NONE;
			}
			else  {
				UEngine::GetInstance().GetGizmo()->selectedAxis = neareastAxis;
			}
		}
	}
	if (Input::Instance()->IsMouseButtonReleased(0) && !_io->WantCaptureMouse) {
		UEngine::GetInstance().GetGizmo()->selectedAxis = EPrimitiveColor::NONE;
	}
}

UActorComponent* GuiController::GetNearestActorComponents(float& distance) {
	int x, y;
	Input::Instance()->GetMouseLocation(x, y);
	//_selected = 
	UActorComponent* nearestActor = world->PickingByRay(x, y, distance); //_selected
	return nearestActor;
}

EPrimitiveColor GuiController::GetNearestGizmo(float& distance)
{
	if (!UEngine::GetInstance().GetGizmo()->isGizmoActivated)
	{
		distance = FLT_MAX;
		return EPrimitiveColor::NONE;
	}
	int x, y;
	Input::Instance()->GetMouseLocation(x, y);
	FMatrix viewMatrix = FMatrix::Identity;
	FVector pickPosition;
	world->ConvertNDC_VIEW(x, y, pickPosition, viewMatrix);
	float hitDistance[4]{ FLT_MAX,FLT_MAX,FLT_MAX,FLT_MAX };
	float minDistance = FLT_MAX;
	EPrimitiveColor pickedAxis = EPrimitiveColor::NONE;

	if (UEngine::GetInstance().GetGizmo()->ArrowX->PickObjectByRayIntersection(pickPosition, viewMatrix, &hitDistance[EPrimitiveColor::RED_X])) {
		if (hitDistance[EPrimitiveColor::RED_X] < minDistance) {
			minDistance = hitDistance[EPrimitiveColor::RED_X];
			pickedAxis = EPrimitiveColor::RED_X;
		}
	}
	if (UEngine::GetInstance().GetGizmo()->ArrowY->PickObjectByRayIntersection(pickPosition, viewMatrix, &hitDistance[EPrimitiveColor::GREEN_Y])) {
		if (hitDistance[EPrimitiveColor::GREEN_Y] < minDistance) {
			minDistance = hitDistance[EPrimitiveColor::GREEN_Y];
			pickedAxis = EPrimitiveColor::GREEN_Y;
		}
	}
	if (UEngine::GetInstance().GetGizmo()->ArrowZ->PickObjectByRayIntersection(pickPosition, viewMatrix, &hitDistance[EPrimitiveColor::BLUE_Z])) {
		if (hitDistance[EPrimitiveColor::BLUE_Z] < minDistance) {
			minDistance = hitDistance[EPrimitiveColor::BLUE_Z];
			pickedAxis = EPrimitiveColor::BLUE_Z;
		}
	}

	distance = hitDistance[pickedAxis];
	return pickedAxis;

}

void GuiController::RenderFrame()
{
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void GuiController::RenderEditor() {
#pragma region Control Panel
	float controllWindowWidth = static_cast<float>(SCR_WIDTH) * 0.3f;
	float controllWindowHeight = static_cast<float>(SCR_HEIGHT) * 0.25f;
	float controllWindowPosX = (static_cast<float>(SCR_WIDTH) - controllWindowWidth) * 0.f;
	float controllWindowPosY = (static_cast<float>(SCR_HEIGHT) - controllWindowHeight) * 0.f;
	ImGui::SetNextWindowPos(ImVec2(controllWindowPosX, controllWindowPosY));
	ImGui::SetNextWindowSize(ImVec2(controllWindowWidth, 0.0f), ImGuiCond_Once);

	ImGui::SetNextWindowSizeConstraints(ImVec2(300.0f, 0.0f), ImVec2(300.0f, FLT_MAX));

	const char* primitiveItems[] = { "Cube", "Sphere", "Plane","Text"};
	const char* viewModes[] = { "Lit", "Unlit", "Wireframe" };

	D3D11_FILL_MODE currentFillMode = CRenderer::Instance()->GetGraphics()->GetFillMode();
	//int selectedMode = (currentFillMode == D3D11_FILL_SOLID) ? 0 : (currentFillMode == D3D11_FILL_WIREFRAME) ? 2 : 1;

	ImGui::Begin("Control Panel", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

	ImGui::Text("FPS: %.2f (%.2fms)", 1 / Time::GetDeltaTime(), 1000.f * Time::GetDeltaTime());
	ImGui::Text("UObject Count: %d", CEngineStatics::TotalAllocationCount);
	ImGui::Text("UObject Bytes: %d", CEngineStatics::TotalAllocationBytes);
	ImGui::Separator();
	/***********************************/

	ImGui::Text("UActorComponent Count: %d", world->GetActorCount());

	ImGui::Combo("Primitive", &_selectedPrimitive, primitiveItems, ARRAYSIZE(primitiveItems));
	if (ImGui::Button("Create"))
	{
		for (int i = 0; i < _spawnNumber; i++) {
			switch (_selectedPrimitive) {
			case 0:
				world->SpawnCubeActor();
				break;
			case 1:
				world->SpawnSphereActor();
				break;
			case 2:
				world->SpawnPlaneActor();
				break;
			case 3:
				world->SpawnTextActor();
				break;
			}
		}
	}
	ImGui::SameLine(0.f, 5.f);
	ImGui::InputInt("Number of Spawn", &_spawnNumber, 1, 50);
	ImGui::Separator();
	/***********************************/

	if (_spawnNumber < 0)
		_spawnNumber = 0;

	ImGui::InputText("Scene Name", _sceneNameBuffer, ARRAYSIZE(_sceneNameBuffer));
	if (ImGui::Button("New Scene")) {
		world->ClearWorld();
		_selected = nullptr;
		UPrimitiveComponent* downcast = dynamic_cast<UPrimitiveComponent*>(_selected);
		if (downcast)
			downcast->renderFlags &= ~PRIMITIVE_FLAG_SELECTED;
	}
	if (ImGui::Button("Save Scene")) {
		world->SaveWorld(_sceneNameBuffer);
	}
	if (ImGui::Button("Load Scene")) {
		world->LoadWorld(_sceneNameBuffer);
		_selected = nullptr;
		UPrimitiveComponent* downcast = dynamic_cast<UPrimitiveComponent*>(_selected);
		if (downcast)
			downcast->renderFlags &= ~PRIMITIVE_FLAG_SELECTED;
	}
	ImGui::Separator();
	/*********************************/

	ImGui::Text("Camera");
	UCameraComponent* mainCam = CRenderer::Instance()->GetMainCamera();
	if (mainCam) {
		mainCam->RenderUI();
	}
	ImGui::Separator();
	/***********************************/

	ImGui::Text("ViewMode");
	ImGui::SameLine();

	if (ImGui::Combo("##", &_selectedMode, viewModes, IM_ARRAYSIZE(viewModes)))
	{
		// 선택 변경 시 실행할 코드
		D3D11_FILL_MODE newMode = D3D11_FILL_SOLID;
		switch (_selectedMode)
		{
		case 0: newMode = D3D11_FILL_SOLID; break; // Lit 모드 처리
		case 1: newMode = D3D11_FILL_SOLID; break; // Unlit 모드 처리
		case 2: newMode = D3D11_FILL_WIREFRAME; break; // Wireframe 모드 처리
		}

		CRenderer::Instance()->SetRasterzierState(newMode);
	}

	ImGui::End();
#pragma endregion

#pragma region Property window
	float propertyWindowWidth = static_cast<float>(SCR_WIDTH) * 0.3f;
	float propertyWindowHeight = static_cast<float>(SCR_HEIGHT) * 0.25f;
	float propertyWindowPosX = (static_cast<float>(SCR_WIDTH) - propertyWindowWidth) * 1.f;
	float propertyWindowPosY = (static_cast<float>(SCR_HEIGHT) - propertyWindowHeight) * 0.f;
	ImGui::SetNextWindowPos(ImVec2(propertyWindowPosX, propertyWindowPosY));
	ImGui::SetNextWindowSize(ImVec2(propertyWindowWidth, 0.0f));
	ImGui::Begin("Property", 0);
	USceneComponent* downcast = nullptr;
	if (_selected)
		downcast = dynamic_cast<USceneComponent*>(_selected);
	if (downcast != nullptr) {
		ImGui::Text("UUID: %d", _selected->GetUUID());


		FVector vec = downcast->GetRelativeLocation();
		float downcastLocation[3] = { vec.x, vec.y, vec.z };
		ImGui::DragFloat3("position", downcastLocation, 0.1f);
		downcast->SetRelativeLocation(FVector(downcastLocation[0], downcastLocation[1], downcastLocation[2]));

		vec = downcast->GetRelativeRotation();
		float downcastRotation[3] = { vec.x, vec.y, vec.z };
		ImGui::DragFloat3("rotation", downcastRotation, 0.1f);
		downcast->SetRelativeRotation(FVector(downcastRotation[0], downcastRotation[1], downcastRotation[2]));

		vec = downcast->GetRelativeScale3D();
		float downcastScale[3] = { vec.x, vec.y, vec.z };
		ImGui::DragFloat3("scale", downcastScale, 0.1f);
		downcast->SetRelativeScale3D(FVector(downcastScale[0], downcastScale[1], downcastScale[2]));

		if ( ImGui::Button("Delete") ) {
			UEngine::GetInstance().GetGizmo()->Detach();
			world->RemoveActor(_selected);
			_selected = nullptr;
		}
	}
	ImGui::End();
#pragma endregion

	_console->Render();
}

void GuiController::Resize()
{
	_io->DisplaySize = ImVec2(static_cast<float>(SCR_WIDTH), static_cast<float>(SCR_HEIGHT));
}

GuiConsole* GuiController::GetConcolWindow()
{
	return _console;
}
