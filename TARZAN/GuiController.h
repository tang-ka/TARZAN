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

class GuiController : public TSingleton<GuiController>
{
public:
	GuiController(){}
	~GuiController();

	void Init(HWND hWnd, CGraphics* graphics);

	void NewFrame();
	UActorComponent* GetNearestActorComponents(float& distance);
	EPrimitiveColor GetNearestGizmo(float& distance);

	void RenderFrame();
	void RenderEditor();
	void Resize();
	
	GuiConsole* GetConcolWindow();
	
	UActorComponent* _selected;
	SceneManagerView* SceneView;

	int _selectedGridScale;

private:
	void CreateSceneManagerPanel();

	int GetSelectedGridScale() const { return _selectedGridScale; }
	// 이게 1값으로 디폴트가 들어가서 그런가
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

