﻿#pragma once

#include "Singleton.h"

class UGizmoComponent;
class UWorld;
class GuiController;
class UCoordArrowComponent;

enum class EScreenMode
{
    Windowed,  
    Fullscreen, 
    Borderless,
};

class UEngine : public TSingleton<UEngine>
{
public:
	static LRESULT CALLBACK WinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

    /**
     * Application을 초기화 합니다.
     * @param hInstance 윈도우 인스턴스
     * @param InWindowTitle 윈도우 창 이름
     * @param InWindowClassName 윈도우 클래스 이름
     * @param InScreenWidth 화면 너비
     * @param InScreenHeight 화면 높이
     * @param InScreenMode 창 모드
     * @return 초기화 여부
    */
    void Initialize(
        HINSTANCE hInstance, const WCHAR* InWindowTitle, const WCHAR* InWindowClassName, int InScreenWidth,
        int InScreenHeight, EScreenMode InScreenMode = EScreenMode::Windowed
    );

    void Run();

    void Shutdown();

public:
    /* retun: Main GUI Controller */
    GuiController* GetGUI();

    /* return: Main Scene */
    UWorld* GetWorld();

    /* return: Main Gizmo */
    UGizmoComponent* GetGizmo();

private:
    bool InitWindow(int32 InScreenWidth, int32 InScreenHeight);

    void InitGlobal();

    void InitWorld();

private:
    bool IsRunning = false;
    EScreenMode ScreenMode = EScreenMode::Windowed;

    const WCHAR* WindowTitle = nullptr;
    const WCHAR* WindowClassName = nullptr;
    HWND WindowHandle = nullptr;
    HINSTANCE WindowInstance = nullptr;

    int InitializedScreenWidth = 0;
    int InitializedScreenHeight = 0;

/* Maybe should refactoring.. */
private:
    UGizmoComponent* UGizmo = nullptr;
    UWorld* World = nullptr;
    GuiController* Controller = nullptr;
    UCoordArrowComponent* Arrow = nullptr;
};

