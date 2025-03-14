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

#include <vector>
#include <list>
#include <string>
#include <sstream>
#include <unordered_map>
#include <algorithm>

template <typename T> using TArray = std::vector<T>;
template <typename T> using TLinkedList = std::list<T>;

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
	NONE
};

struct FPrimitiveFlags {
	UINT flags;
};

enum EPrimitiveFlag {
	PRIMITIVE_FLAG_DISABLE = 1 << 0,
	PRIMITIVE_FLAG_SELECTED = 1 << 1,
};

#include "Framework/Core/FNameEntryRegistry.h"

/* FName */
#ifndef WITH_CASE_PRESERVING_NAME
#define WITH_CASE_PRESERVING_NAME 1
#endif

enum { NAME_SIZE = 1024 };
/* WITH_CASE_PRESERVING_NAME 이 활성화되어 있으면 DisplayName으로 비교합니다.
* 
* DisplayName은 원래의 이름 문자열을 대소문자 그대로 유지하여 저장합니다.
* 
* 기본적으로 현재 프로젝트는 WITH_CASE_PRESERVING_NAME 1입니다.
*/
struct FName
{
	FName()
	{
		*this = FName("UObject");
	}

	FName(char* pStr)
	{
		FString tmpStr(pStr);
		*this = FName(tmpStr);
	}

	FName(FString str)
	{
		DisplayIndex =  FNameEntryRegistry::Get().FindOrAddNameEntry(str);

		FString LowerString = str;
		
		TranslateLower(LowerString);

		ComparisonIndex = FNameEntryRegistry::Get().FindOrAddNameEntry(LowerString);
	}

	int32 DisplayIndex;
	int32 ComparisonIndex;

	bool operator==(const FName& Other) const
	{
		return !Compare(Other);
	}

	FName& operator=(const FString& Other)
	{
		DisplayIndex = FNameEntryRegistry::Get().FindOrAddNameEntry(Other);

		FString LowerString = Other;
		TranslateLower(LowerString);
		ComparisonIndex = FNameEntryRegistry::Get().FindOrAddNameEntry(LowerString);

		return *this;
	}

	FName& operator=(const char* Other)
	{
		return *this = FString(Other);
	}

	/* 같다면 0, 작으면 -1, 크다면 1 */
	inline int32 Compare(const FName& Other) const
	{
		const int32 rhs = GetDisplayIndexFast();
		const int32 lhs = Other.GetDisplayIndexFast();

		if (rhs < lhs)
		{
			return -1;
		}
		else if (rhs > lhs)
		{
			return 1;
		}
		return 0;
	}

	inline int32 GetComparisonIndex() const
	{
		return ComparisonIndex;
	}

	inline int32 GetDisplayIndex() const
	{
		const int32 Index = GetDisplayIndexFast();
		return Index;
	}

	FString ToString() const
	{
		return FNameEntryRegistry::Get().GetNameString(DisplayIndex);
	}

	void ToString(FString& Out) const
	{
		Out = FNameEntryRegistry::Get().GetNameString(DisplayIndex);
	}

	/**
	 * Buffer size required for any null-terminated FName string, i.e. [name] '_' [digits] '\0'
	 */
	static constexpr inline uint32 StringBufferSize = NAME_SIZE + 1 + 10; // NAME_SIZE includes null-terminator

	void AppendString(FString& Out)
	{
		uint32 ExistIndex = FNameEntryRegistry::Get().FindOrAddNameEntry(Out);

		FString FrontStr = FNameEntryRegistry::Get().GetNameString(DisplayIndex);
		FString BackStr = FNameEntryRegistry::Get().GetNameString(ExistIndex);

		FString MergedStr = FrontStr + BackStr;
		
		DisplayIndex = FNameEntryRegistry::Get().FindOrAddNameEntry(MergedStr);
		
		// to lower-case
		TranslateLower(MergedStr);
		ComparisonIndex = FNameEntryRegistry::Get().FindOrAddNameEntry(MergedStr);
	}

private:
	inline int32 GetDisplayIndexFast() const
	{
#if WITH_CASE_PRESERVING_NAME
		return DisplayIndex;
#else
		return ComparisonIndex;
#endif
	}

	void TranslateLower(FString& InString)
	{
		for (char& c : InString)
		{
			c = tolower(c);
		}
	}
};