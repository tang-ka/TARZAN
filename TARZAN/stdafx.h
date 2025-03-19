#pragma once

#pragma comment(lib, "user32")
#pragma comment(lib, "d3d11")
#pragma comment(lib, "d3dcompiler")

#include <d3d11.h>
#include <d3dcompiler.h>
#include <comdef.h>

#include <fstream>
#include <assert.h>
#include <Windows.h>
#include <iostream>

#include <vector>
#include <list>
#include <string>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>

// 임시로 여기에 옮겨놨음.
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

template <typename T> using TArray = std::vector<T>;
template <typename T> using TLinkedList = std::list<T>;

// 정책 클래스를 사용하여 std::set 또는 std::unordered_set 선택
template <typename T, typename Policy = std::unordered_set<T>>
using TSet = Policy;

template <typename Key, typename Value, typename Policy = std::unordered_map<Key, Value>>
using TMap = Policy;

template <typename T1, typename T2>
using TPair = std::pair<T1, T2>;

using int32 = int;
using uint32 = unsigned int;
using FString = std::string;
using FWString = std::wstring;

#include "Math/FVector.h"
#include "Math/FMatrix.h"
#define M_PI           3.14159265358979323846

//#define SCR_WIDTH 1200
//#define SCR_HEIGHT 800

extern int SCR_WIDTH;
extern int SCR_HEIGHT;

#include "Framework/Core/Time.h"
#include "Framework/Core/Input.h"
#include "DataManager.h"

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui_impl_win32.h"
#include "GuiController.h"
#include "Framework/Core/FName.h"

template <typename T> void SafeRelease(T** ppT) {
	if (*ppT) {
		(*ppT)->Release();
		*ppT = nullptr;
	}
}


#define UE_LOG OutputDebugString

inline float degToRad(float deg) { return deg * M_PI / 180.f; }
inline float radToDeg(float rad) { return rad * 180.f / M_PI; }

enum EPrimitiveColor
{
	RED_X,
	GREEN_Y,
	BLUE_Z,
	NONE,
	RED_X_ROT,
	GREEN_Y_ROT,
	BLUE_Z_ROT
};

struct FPrimitiveFlags {
	UINT flags;
};

struct FTextCheck
{
	UINT isText;
};

enum EPrimitiveFlag {
	PRIMITIVE_FLAG_DISABLE = 1 << 0,
	PRIMITIVE_FLAG_SELECTED = 1 << 1,
};

enum EPrimitiveType
{
	CUBE,
	SPHERE,
	PLANE,
	TEXT
};

enum EGizmoType
{
	Location,
	Rotation,
	Scaling
};