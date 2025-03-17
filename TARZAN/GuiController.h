#pragma once
//#include "stdafx.h"
#include "GuiConsole.h"

#include "Singleton.h"

enum EPrimitiveColor;
class CGraphics;
class UActorComponent;
class SceneManagerView;
class UWorld;
class UWorldGridComponent;
class UPrimitiveComponent;

class GuiController : public TSingleton<GuiController>
{
public:
	GuiController(){}
	~GuiController();

	void Init(HWND hWnd, CGraphics* graphics);

	void NewFrame();
	UActorComponent* GetNearestActorComponents(float& distance);
	EPrimitiveColor GetNearestGizmo(float& distance);
	void SelectActor(UActorComponent* neareastActorComp);
	void DeselectActor();
	UPrimitiveComponent* GetSelectedObject();

	void RenderFrame();
	void RenderEditor();
	void Resize();
	
public:
	GuiConsole* GetConcolWindow();
	
	UActorComponent* _selected;
	SceneManagerView* SceneView;

	int _selectedMode;
	int _selectedGridScale;
	int _selectedCameraSpeed;

private:
	void CreateSceneManagerPanel();

private:
	ImGuiContext* _context;
	ImGuiIO* _io;
	HWND hWnd;
	// �̰� ����ũ�� �δ°� �³�
	std::unique_ptr<GuiConsole> _console;

	char _sceneNameBuffer[256] = "";
	int _selectedPrimitive = 0, _spawnNumber = 1;
};

