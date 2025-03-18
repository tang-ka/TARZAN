#include "stdafx.h"
#include "UObject.h"
#include "SceneManager.h"
#include "Framework/Core/Engine.h"
#include "UWorld.h"

USceneManager::USceneManager()
{
	PrimaryWorld = nullptr;
	bIsDirty = false;
}

USceneManager::~USceneManager()
{
}

void USceneManager::Initialize()
{
	if (nullptr == PrimaryWorld)
	{
		//PrimaryWorld = new UWorld();
		PrimaryWorld = std::make_unique<UWorld>();
	}
}

void USceneManager::SpawnActor(EPrimitiveType PrimitiveType)
{
	UPrimitiveComponent* SpawnActor = nullptr;

	switch (PrimitiveType)
	{
		case CUBE:
		{
			SpawnActor = PrimaryWorld->SpawnCubeActor();
			break;
		}
		case SPHERE:
		{
			SpawnActor = PrimaryWorld->SpawnSphereActor();
			break;
		}
		case PLANE:
		{
			SpawnActor = PrimaryWorld->SpawnPlaneActor();
			break;
		}
		case TEXT:
		{
			SpawnActor = PrimaryWorld->SpawnTextActor();
			break;
		}
		default:
		{
			SpawnActor = nullptr;
			break;
		}
	}

	if (SpawnActor)
	{
		SetNewName(SpawnActor, PrimitiveType);
		InsertSpawnActorMap(SpawnActor);
		//GuiController::GetInstance().SelectActor(SpawnActor);
	}
}

void USceneManager::SpawnActor(FString Name, EPrimitiveType PrimitiveType)
{
	UPrimitiveComponent* SpawnActor = nullptr;

	switch (PrimitiveType)
	{
		case CUBE:
		{
			SpawnActor = PrimaryWorld->SpawnCubeActor();
			break;
		}
		case SPHERE:
		{
			SpawnActor = PrimaryWorld->SpawnSphereActor();
			break;
		}
		case PLANE:
		{
			SpawnActor = PrimaryWorld->SpawnPlaneActor();
			break;
		}
		case TEXT:
		{
			SpawnActor = PrimaryWorld->SpawnTextActor();
			break;
		}
		default:
		{
			SpawnActor = nullptr;
			break;
		}
	}

	if (SpawnActor)
	{
		SetNewName(SpawnActor, Name);
		InsertSpawnActorMap(SpawnActor);
	}
}

void USceneManager::DeleteActorFromMap(UObject* Object)
{
	SpawnActorMap.erase(Object->GetUUID());
	MarkDirty();
}

const TMap<uint32, UObject*>& USceneManager::RetrieveActorMap() const
{
	return SpawnActorMap;
}

void USceneManager::MarkDirty()
{
	bIsDirty = true;
}

void USceneManager::ClearDirty()
{
	bIsDirty = false;
}

void USceneManager::Shutdown()
{
	// 먼저, SpawnActorMap에 저장된 모든 UObject*를 삭제합니다.
	for (auto& pair : SpawnActorMap)
	{
		UObject* actor = pair.second;
		if (actor)
		{
			delete actor;
		}
	}
	SpawnActorMap.clear();

	// 필요하다면, PrimaryWorld에 속한 액터들도 정리합니다.
	if (PrimaryWorld)
	{
		PrimaryWorld->ClearWorld();
		PrimaryWorld = nullptr;
	}
}


void USceneManager::SetNewName(UObject* Actor, EPrimitiveType PrimitiveType)
{
	int& Count = NextIndices[PrimitiveType];

	FString NewName = GetPrimitiveTypeString(PrimitiveType) + "_" + std::to_string(Count);
	Count++;
	Actor->SetName(NewName);
}

void USceneManager::SetNewName(UObject* Actor, FString Name)
{
	Actor->SetName(Name);
}

void USceneManager::InsertSpawnActorMap(UObject* Object)
{
	SpawnActorMap[Object->GetUUID()] = Object;
	MarkDirty();
}

FString USceneManager::GetPrimitiveTypeString(EPrimitiveType PrimitiveType)
{
	switch (PrimitiveType)
	{
		case CUBE:
			return FString("Cube");
		case SPHERE:
			return FString("Sphere");
		case PLANE:
			return FString("Plane");
		case TEXT:
			return FString("Text");
		default:
			return FString("NAME_NONE");
	}
}

UWorld* USceneManager::GetWorld()
{
	return PrimaryWorld.get();
}