#include "stdafx.h"
#include "GuiController.h"
#include "Framework/DirectXWrapper/CGraphics.h"
#include "Framework/Core/CEngineStatics.h"
#include "Framework/Core/Time.h"
#include "Framework/Core/UCubeComponent.h"
#include "Framework/Core/UGizmoComponent.h"
#include "Framework/Core/Engine.h"
#include "UWorld.h"
#include "Framework/Core/SceneManager.h"
#include "ConfigManager.h"
#include "Framework/Core/UWorldGridComponent.h"


GuiController::~GuiController()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void GuiController::Init(HWND hWnd, CGraphics* graphics)
{
	IMGUI_CHECKVERSION();
	_context = ImGui::CreateContext();
	_io = &ImGui::GetIO();
	ImGui_ImplDX11_Init(graphics->GetDevice(), graphics->GetDeviceContext());
	ImGui_ImplWin32_Init(hWnd);
	_console = std::make_unique<GuiConsole>();
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

	if (Input::Instance()->IsMouseButtonPressed(0) && !_io->WantCaptureMouse)
	{
		float nearestActorDistance = FLT_MAX;
		float nearestGizmoDistance = FLT_MAX;
		UActorComponent* nearestActorComp = GetNearestActorComponents(nearestActorDistance);
		EPrimitiveColor nearestAxis = GetNearestGizmo(nearestGizmoDistance);

		// 아무 것도 선택되지 않았을 경우: 기존 선택 해제 및 Gizmo Detach
		if (nearestActorComp == nullptr && nearestAxis == EPrimitiveColor::NONE)
		{
			UEngine::GetInstance().GetGizmo()->Detach();
			UPrimitiveComponent* downcast = dynamic_cast<UPrimitiveComponent*>(_selected);
			if (downcast)
			{
				downcast->renderFlags &= ~PRIMITIVE_FLAG_SELECTED;
				downcast->HideBoundingBox();
			}
			_selected = nullptr;
		}
		else
		{
			// Actor와 Gizmo 중 어느 쪽이 더 가까운지 비교
			if (nearestActorComp != nullptr && nearestActorDistance < nearestGizmoDistance)
			{
				// Actor 선택 처리
				UPrimitiveComponent* downcast = dynamic_cast<UPrimitiveComponent*>(_selected);
				if (downcast)
				{
					downcast->renderFlags &= ~PRIMITIVE_FLAG_SELECTED;
					downcast->HideBoundingBox();
				}
				_selected = nearestActorComp;
				downcast = dynamic_cast<UPrimitiveComponent*>(_selected);
				if (downcast)
				{
					downcast->renderFlags |= PRIMITIVE_FLAG_SELECTED;
					downcast->ShowBoundingBox();
				}
				UEngine::GetInstance().GetGizmo()->AttachTo(dynamic_cast<UPrimitiveComponent*>(_selected));
				UEngine::GetInstance().GetGizmo()->selectedAxis = EPrimitiveColor::NONE;
			}
			else if (nearestAxis != EPrimitiveColor::NONE)
			{
				// Gizmo 축 선택 처리
				UEngine::GetInstance().GetGizmo()->selectedAxis = nearestAxis;
				// 기존에 선택된 Actor가 있다면 선택 해제 (Gizmo 선택 시 Actor 선택은 유지하지 않음)
				UPrimitiveComponent* downcast = dynamic_cast<UPrimitiveComponent*>(_selected);
				if (downcast)
				{
					downcast->renderFlags &= ~PRIMITIVE_FLAG_SELECTED;
					downcast->HideBoundingBox();
				}
			}
		}
	}

	if (Input::Instance()->IsMouseButtonReleased(0) && !_io->WantCaptureMouse)
	{
		UEngine::GetInstance().GetGizmo()->selectedAxis = EPrimitiveColor::NONE;
	}
}

UActorComponent* GuiController::GetNearestActorComponents(float& distance)
{
	int x, y;

	UWorld* World = UEngine::GetInstance().GetWorld();

	Input::Instance()->GetMouseLocation(x, y);
	UActorComponent* nearestActor = World->PickingByRay(x, y, distance);
	return nearestActor;
}

EPrimitiveColor GuiController::GetNearestGizmo(float& distance)
{
	if (!UEngine::GetInstance().GetGizmo()->isGizmoActivated)
	{
		distance = FLT_MAX;
		return EPrimitiveColor::NONE;
	}
	UWorld* World = UEngine::GetInstance().GetWorld();

	int x, y;
	Input::Instance()->GetMouseLocation(x, y);
	FMatrix viewMatrix = FMatrix::Identity;
	FVector pickPosition;
	World->ConvertNDC_VIEW(x, y, pickPosition, viewMatrix);
	float hitDistance[7]{ FLT_MAX,FLT_MAX,FLT_MAX,FLT_MAX,FLT_MAX,FLT_MAX ,FLT_MAX };
	float minDistance = FLT_MAX;
	EPrimitiveColor pickedAxis = EPrimitiveColor::NONE;

	if (UEngine::GetInstance().GetGizmo()->ArrowX->PickObjectByRayIntersection(pickPosition, viewMatrix, &hitDistance[EPrimitiveColor::RED_X]))
	{
		if (hitDistance[EPrimitiveColor::RED_X] < minDistance)
		{
			minDistance = hitDistance[EPrimitiveColor::RED_X];
			pickedAxis = EPrimitiveColor::RED_X;
		}
	}
	if (UEngine::GetInstance().GetGizmo()->ArrowY->PickObjectByRayIntersection(pickPosition, viewMatrix, &hitDistance[EPrimitiveColor::GREEN_Y]))
	{
		if (hitDistance[EPrimitiveColor::GREEN_Y] < minDistance)
		{
			minDistance = hitDistance[EPrimitiveColor::GREEN_Y];
			pickedAxis = EPrimitiveColor::GREEN_Y;
		}
	}
	if (UEngine::GetInstance().GetGizmo()->ArrowZ->PickObjectByRayIntersection(pickPosition, viewMatrix, &hitDistance[EPrimitiveColor::BLUE_Z]))
	{
		if (hitDistance[EPrimitiveColor::BLUE_Z] < minDistance)
		{
			minDistance = hitDistance[EPrimitiveColor::BLUE_Z];
			pickedAxis = EPrimitiveColor::BLUE_Z;
		}
	}

	if (UEngine::GetInstance().GetGizmo()->RotationX->PickObjectByRayIntersection(pickPosition, viewMatrix, &hitDistance[EPrimitiveColor::RED_X_ROT]))
	{
		if (hitDistance[EPrimitiveColor::RED_X_ROT] < minDistance)
		{
			minDistance = hitDistance[EPrimitiveColor::RED_X_ROT];
			pickedAxis = EPrimitiveColor::RED_X_ROT;
		}
	}
	if (UEngine::GetInstance().GetGizmo()->RotationY->PickObjectByRayIntersection(pickPosition, viewMatrix, &hitDistance[EPrimitiveColor::GREEN_Y_ROT]))
	{
		if (hitDistance[EPrimitiveColor::GREEN_Y_ROT] < minDistance)
		{
			minDistance = hitDistance[EPrimitiveColor::GREEN_Y_ROT];
			pickedAxis = EPrimitiveColor::GREEN_Y_ROT;
		}
	}
	if (UEngine::GetInstance().GetGizmo()->RotationZ->PickObjectByRayIntersection(pickPosition, viewMatrix, &hitDistance[EPrimitiveColor::BLUE_Z_ROT]))
	{
		if (hitDistance[EPrimitiveColor::BLUE_Z_ROT] < minDistance)
		{
			minDistance = hitDistance[EPrimitiveColor::BLUE_Z_ROT];
			pickedAxis = EPrimitiveColor::BLUE_Z_ROT;
		}
	}

	distance = hitDistance[pickedAxis];
	return pickedAxis;

}

void GuiController::SelectActor(UActorComponent* neareastActorComp)
{
	UPrimitiveComponent* downcast = dynamic_cast<UPrimitiveComponent*>(_selected);
	if (downcast)
		downcast->renderFlags &= ~PRIMITIVE_FLAG_SELECTED;

	if (downcast != nullptr)
		downcast->HideBoundingBox();
	_selected = neareastActorComp;
	dynamic_cast<UPrimitiveComponent*>(_selected)->ShowBoundingBox();

	downcast = dynamic_cast<UPrimitiveComponent*>(_selected);
	if (downcast)
		downcast->renderFlags |= PRIMITIVE_FLAG_SELECTED;

	UEngine::GetInstance().GetGizmo()->AttachTo(dynamic_cast<UPrimitiveComponent*>(_selected));
	UEngine::GetInstance().GetGizmo()->selectedAxis = EPrimitiveColor::NONE;
}

void GuiController::DeselectActor()
{
	UEngine::GetInstance().GetGizmo()->Detach();
	UPrimitiveComponent* downcast = dynamic_cast<UPrimitiveComponent*>(_selected);
	if (downcast)
	{
		downcast->renderFlags &= ~PRIMITIVE_FLAG_SELECTED;
		downcast->HideBoundingBox();
	}
	_selected = nullptr;
}

UPrimitiveComponent* GuiController::GetSelectedObject()
{
	return dynamic_cast<UPrimitiveComponent*>(_selected);
}

void GuiController::RenderFrame()
{
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void GuiController::RenderEditor()
{
	UWorld* World = UEngine::GetInstance().GetWorld();
	USceneManager* SceneManager = UEngine::GetInstance().GetSceneManager();

	CreateSceneManagerPanel();

#pragma region Control Panel
	float controllWindowWidth = static_cast<float>(SCR_WIDTH) * 0.3f;
	float controllWindowHeight = static_cast<float>(SCR_HEIGHT) * 0.25f;
	float controllWindowPosX = (static_cast<float>(SCR_WIDTH) - controllWindowWidth) * 0.f;
	float controllWindowPosY = (static_cast<float>(SCR_HEIGHT) - controllWindowHeight) * 0.f;
	ImGui::SetNextWindowPos(ImVec2(controllWindowPosX, controllWindowPosY));
	ImGui::SetNextWindowSize(ImVec2(controllWindowWidth, 0.0f), ImGuiCond_Once);

	ImGui::SetNextWindowSizeConstraints(ImVec2(300.0f, 0.0f), ImVec2(300.0f, FLT_MAX));

	const char* primitiveItems[] = { "Cube", "Sphere", "Plane","Text", "SpotLight"};
	const char* viewModes[] = { "Lit", "Unlit", "Wireframe" };

	D3D11_FILL_MODE currentFillMode = CRenderer::Instance()->GetGraphics()->GetFillMode();
	//int selectedMode = (currentFillMode == D3D11_FILL_SOLID) ? 0 : (currentFillMode == D3D11_FILL_WIREFRAME) ? 2 : 1;

	ImGui::Begin("Control Panel", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

	ImGui::Text("FPS: %.2f (%.2fms)", 1 / Time::GetDeltaTime(), 1000.f * Time::GetDeltaTime());
	ImGui::Text("UObject Count: %d", CEngineStatics::TotalAllocationCount);
	ImGui::Text("UObject Bytes: %d", CEngineStatics::TotalAllocationBytes);
	ImGui::Separator();
	/***********************************/

	ImGui::Text("UActorComponent Count: %d", World->GetActorCount());

	ImGui::Combo("Primitive", &_selectedPrimitive, primitiveItems, ARRAYSIZE(primitiveItems));
	if (ImGui::Button("Create"))
	{
		for (int i = 0; i < _spawnNumber; i++)
		{
			switch (_selectedPrimitive)
			{
			case 0:
				SceneManager->SpawnActor(EPrimitiveType::CUBE);
				break;
			case 1:
				SceneManager->SpawnActor(EPrimitiveType::SPHERE);
				break;
			case 2:
				SceneManager->SpawnActor(EPrimitiveType::PLANE);
				break;
			case 3:
				SceneManager->SpawnActor(EPrimitiveType::TEXT);
				break;
			case 4:
				SceneManager->SpawnActor(EPrimitiveType::SPOTLIGHT);
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
	if (ImGui::Button("New Scene"))
	{
		World->ClearWorld();
		_selected = nullptr;
		UPrimitiveComponent* downcast = dynamic_cast<UPrimitiveComponent*>(_selected);
		if (downcast)
			downcast->renderFlags &= ~PRIMITIVE_FLAG_SELECTED;
	}
	if (ImGui::Button("Save Scene"))
	{
		World->SaveWorld(_sceneNameBuffer);
	}
	if (ImGui::Button("Load Scene"))
	{
		World->LoadWorld(_sceneNameBuffer);
		_selected = nullptr;
		UPrimitiveComponent* downcast = dynamic_cast<UPrimitiveComponent*>(_selected);
		if (downcast)
			downcast->renderFlags &= ~PRIMITIVE_FLAG_SELECTED;
	}
	ImGui::Separator();
	/*********************************/

	if (ImGui::Checkbox("Show Flag", &showFlag))
	{
		World->SetShowFlag(showFlag);
	}

	ImGui::Text("Camera");
	UCameraComponent* mainCam = CRenderer::Instance()->GetMainCamera();
	if (mainCam)
	{
		mainCam->RenderUI();
	}
	ImGui::Separator();
	/***********************************/

	ImGui::Text("View Mode");
	ImGui::SameLine();

	if (ImGui::Combo("##ViewMode", &_selectedMode, viewModes, IM_ARRAYSIZE(viewModes)))
	{
		D3D11_FILL_MODE newMode = D3D11_FILL_SOLID;
		switch (_selectedMode)
		{
		case 0: newMode = D3D11_FILL_SOLID; break; // Lit
		case 1: newMode = D3D11_FILL_SOLID; break; // Unlit
		case 2: newMode = D3D11_FILL_WIREFRAME; break; // Wireframe
		}

		CRenderer::Instance()->SetRasterzierState(newMode);
	}

	ImGui::Separator();
	/***********************************/
	const char* gridScaleItems[] = { "0.1","1", "10" };
	const float gridScaleValues[3] = { 0.1f,1.0f, 10.0f };
	ImGui::Text("Grid Scale");
	ImGui::SameLine();
	if (ImGui::Combo("##GridScale", &_selectedGridScale, gridScaleItems, IM_ARRAYSIZE(gridScaleItems)))
	{
		float newScale = gridScaleValues[_selectedGridScale];

		UWorld* World = UEngine::GetInstance().GetWorld();
		if (World)
		{
			UCameraComponent* cam = CRenderer::Instance()->GetMainCamera();
			if (cam)
			{
				float camX = cam->GetRelativeLocation().x;
				float camZ = cam->GetRelativeLocation().z;
				int gridCount = 10;

				//UEngine::GetInstance().GetWorldGridComponent()->GenerateGrid(std::floor(camX), std::floor(camZ), gridCount, newScale);
				UEngine::GetInstance().GetWorldGridComponent()->GenerateGrid(camX, camZ, gridCount, newScale);
			}
		}
	}
	ImGui::Separator();
	/***********************************/
	const char* cameraSpeedItems[] = { "1","2", "5", "10" };
	const float cameraSpeedValues[4] = { 1.f,2.f, 5.f, 10.f };
	ImGui::Text("Camera Speed");
	ImGui::SameLine();
	if (ImGui::Combo("##CameraSpeed", &_selectedCameraSpeed, cameraSpeedItems, IM_ARRAYSIZE(cameraSpeedItems)))
	{
		UWorld* World = UEngine::GetInstance().GetWorld();
		if (World)
		{
			UCameraComponent* cam = CRenderer::Instance()->GetMainCamera();
			if (cam)
			{
				float newSpeed = cameraSpeedValues[_selectedCameraSpeed];
				cam->CameraSpeed = newSpeed;
			}
		}
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
	if (downcast != nullptr)
	{
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

		if (ImGui::Button("Delete"))
		{
			UEngine::GetInstance().GetGizmo()->Detach();
			SceneManager->DeleteActorFromMap(_selected);
			World->RemoveActor(_selected);
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
	return _console.get();
}

void GuiController::CreateSceneManagerPanel()
{
	ImGui::SetNextWindowSize(ImVec2(300.f, 300.0f));
	ImGui::Begin("SceneManager", 0);

	if (ImGui::TreeNode("Primitive"))
	{
		SceneView->Update();

		TMap<uint32, UObject*> Actors = SceneView->GetActors();

		for (const TPair<uint32, UObject*>& Pair : Actors)
		{
			FString ActorName = Pair.second->GetName().ToString();

			if (ImGui::Selectable(ActorName.c_str()))
			{
				if (_selected)
				{
					UEngine::GetInstance().GetGizmo()->Detach();
				}


				UEngine::GetInstance().GetGizmo()->AttachTo(dynamic_cast<UPrimitiveComponent*>(Pair.second));
				UEngine::GetInstance().GetGizmo()->selectedAxis = EPrimitiveColor::NONE;
				_selected = dynamic_cast<UActorComponent*>(Pair.second);
			}
		}

		ImGui::TreePop();
	}

	ImGui::End();
}
