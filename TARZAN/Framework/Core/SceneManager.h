#pragma once
#include "stdafx.h"

class UWorld;
class UObject;

class USceneManager
{
public:
	USceneManager();
	~USceneManager();

	void Initialize();
	UWorld* GetWorld();

	void SpawnActor(EPrimitiveType PrimitiveType);

	/* For Load Scene */
	void SpawnActor(FString Name, EPrimitiveType PrimitiveType);

	/* Delete Actor From Scene */
	void DeleteActorFromMap(UObject* Object);

	/* Join Spawn Actor Map */
	const TMap<uint32, UObject*>& RetrieveActorMap() const;

	/* Dirty */
	void MarkDirty();
	void ClearDirty();
	bool IsDirty() { return bIsDirty; }

private:
	void SetNewName(UObject* Actor, EPrimitiveType PrimitiveType);
	void SetNewName(UObject* Actor, FString Name);

	void InsertSpawnActorMap(UObject* Object);

	FString GetPrimitiveTypeString(EPrimitiveType PrimitiveType);

private:
	/* Primitives의 Name 에 사용될 생성 번호 Ex) Cube_1 .. Cube_2*/
	TMap<EPrimitiveType, int> NextIndices;

	TMap<uint32, UObject*> SpawnActorMap;

	/* Active World */
	UWorld* PrimaryWorld;

	bool bIsDirty;
};

class SceneManagerView
{
public:
	SceneManagerView(USceneManager* _SceneManager)
	{
		SceneManager = _SceneManager;
	}
	~SceneManagerView() = default;

	/* 상태가 변하면 업데이트가 됩니다. */
	void Update()
	{
		if (!SceneManager->IsDirty()) return;

		SpawnActorMapCopy = SceneManager->RetrieveActorMap();

		SceneManager->ClearDirty();
	}

	const TMap<uint32, UObject*> GetActors() const { return SpawnActorMapCopy; };

private:
	USceneManager* SceneManager = nullptr;
	TMap<uint32, UObject*> SpawnActorMapCopy;
};