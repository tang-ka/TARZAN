#pragma once
#include "stdafx.h"
#include "Singleton.h"


enum { NAME_SIZE = 1024 };

enum EFindName
{
	/* Find a name; return 0/NAME_NONE/FNAME() if dosen't exist */
	FNAME_Find,

	/* Find a name or add it if it dosen't exist. */
	FNAME_Add
};


static const uint32 NAME_NONE = 0;

class FNameEntryRegistry : public TSingleton<FNameEntryRegistry>
{
	friend class TSingleton<FNameEntryRegistry>;

public:
	FNameEntryRegistry() : NextId(0)
	{
		NamePool["NAME_NONE"] = 0;
	}

public:
	int32 FindOrAddNameEntry(const FString Name, EFindName FindType = EFindName::FNAME_Add)
	{
		auto Iter = NamePool.find(Name);
		if (Iter != NamePool.end())
		{
			return Iter->second;
		}

		if (FindType == EFindName::FNAME_Find)
		{
			return NAME_NONE;
		}

		NamePool.emplace(Name, ++NextId);
		return NextId;
	}

	FString GetNameString(const int32 Index)
	{
		auto findIt = std::find_if(std::begin(NamePool), std::end(NamePool), [&](const TPair<FString, int32>& pair)
			{
				return pair.second == Index;
			});

		if (findIt != std::end(NamePool))
		{
			// 인덱스에 해당하는 이름 문자열을 반환
			return findIt->first;
		}

		return FString();
	}

private:
	TMap<FString, int32> NamePool;
	uint32 NextId;
};


/* FName */
#ifndef WITH_CASE_PRESERVING_NAME
#define WITH_CASE_PRESERVING_NAME 1
#endif


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
		*this = FName("NAME_NONE");
	}

	FName(char* pStr, EFindName FindType = EFindName::FNAME_Add)
	{
		FString tmpStr(pStr);
		*this = FName(tmpStr);
	}

	FName(FString str, EFindName FindType = EFindName::FNAME_Add)
	{
		DisplayIndex = FNameEntryRegistry::GetInstance().FindOrAddNameEntry(str);

		FString LowerString = str;

		TranslateLower(LowerString);

		ComparisonIndex = FNameEntryRegistry::GetInstance().FindOrAddNameEntry(LowerString);
	}

	int32 DisplayIndex;
	int32 ComparisonIndex;

	bool operator==(const FName& Other) const
	{
		return !Compare(Other);
	}

	FName& operator=(const FString& Other)
	{
		DisplayIndex = FNameEntryRegistry::GetInstance().FindOrAddNameEntry(Other);

		FString LowerString = Other;
		TranslateLower(LowerString);
		ComparisonIndex = FNameEntryRegistry::GetInstance().FindOrAddNameEntry(LowerString);

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
		return FNameEntryRegistry::GetInstance().GetNameString(DisplayIndex);
	}

	void ToString(FString& Out) const
	{
		Out = FNameEntryRegistry::GetInstance().GetNameString(DisplayIndex);
	}

	/**
	 * Buffer size required for any null-terminated FName string, i.e. [name] '_' [digits] '\0'
	 */
	static constexpr inline uint32 StringBufferSize = NAME_SIZE + 1 + 10; // NAME_SIZE includes null-terminator

	void AppendString(FString& Out)
	{
		uint32 ExistIndex = FNameEntryRegistry::GetInstance().FindOrAddNameEntry(Out);

		FString FrontStr = FNameEntryRegistry::GetInstance().GetNameString(DisplayIndex);
		FString BackStr = FNameEntryRegistry::GetInstance().GetNameString(ExistIndex);

		FString MergedStr = FrontStr + BackStr;

		DisplayIndex = FNameEntryRegistry::GetInstance().FindOrAddNameEntry(MergedStr);

		// to lower-case
		TranslateLower(MergedStr);
		ComparisonIndex = FNameEntryRegistry::GetInstance().FindOrAddNameEntry(MergedStr);
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