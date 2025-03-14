#pragma once
//#include "stdafx.h"
#include "GuiConsole.h"
enum EPrimitiveColor;
class CGraphics;
class UActorComponent;
class SceneManagerView;

class GuiController {
public:
	GuiController(HWND hWnd, CGraphics* graphics);
	~GuiController();

	void NewFrame();
	UActorComponent* GetNearestActorComponents(float& distance);
	EPrimitiveColor GetNearestGizmo(float& distance);

	
	void RenderFrame();
	void RenderEditor();
	void Resize();
	
	GuiConsole* GetConcolWindow();
	
	UActorComponent* _selected;
	SceneManagerView* SceneView;

private:
	void CreateSceneManagerPanel();

private:
	ImGuiContext* _context;
	ImGuiIO* _io;
	HWND hWnd;
	GuiConsole* _console;

	char _sceneNameBuffer[256] = "";
	int _selectedPrimitive = 0, _spawnNumber = 1;
	int _selectedMode = 0;
};

