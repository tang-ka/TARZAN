#pragma once
#include "Singleton.h"

class FNameEntryRegistry : public TSingleton<FNameEntryRegistry>
{
public:
	int32 FindOrAddNameEntry(const FString Name)
	{
		auto Iter = NamePool.find(Name);
		if (Iter != NamePool.end())
		{
			return Iter->second;
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
	uint32 NextId = 0;
};

