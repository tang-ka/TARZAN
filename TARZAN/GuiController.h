#pragma once
//#include "stdafx.h"
#include "GuiConsole.h"

#include "Singleton.h"

enum EPrimitiveColor;
class CGraphics;
class UActorComponent;
class UWorld;
class UWorldGridComponent;

class GuiController : public TSingleton<GuiController>
{
public:
	GuiController(){}
	GuiController(HWND hWnd, CGraphics* graphics);
	~GuiController();

	void NewFrame();
	UActorComponent* GetNearestActorComponents(float& distance);
	EPrimitiveColor GetNearestGizmo(float& distance);

	void RenderFrame();
	void RenderEditor();
	void Resize();
	UWorld* world;
	UActorComponent* _selected;
	GuiConsole* GetConcolWindow();

	int GetSelectedGridScale() const { return _selectedGridScale; }
	int _selectedGridScale = 1;
	void SetSelectedGridScale(float scale);

private:
	ImGuiContext* _context;
	ImGuiIO* _io;
	HWND hWnd;
	GuiConsole* _console;


	char _sceneNameBuffer[256] = "";
	int _selectedPrimitive = 0, _spawnNumber = 1;
	int _selectedMode = 0;
};

