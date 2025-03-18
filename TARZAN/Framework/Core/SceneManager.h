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

	void Shutdown();

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
	std::unique_ptr<UWorld> PrimaryWorld;
	bool bIsDirty;
};

class SceneManagerView
{
public:
	// USceneManager 객체는 외부에서 std::shared_ptr로 관리되며,
	// SceneManagerView는 소유권을 갖지 않고 약한 참조(weak_ptr)로 보관합니다.
	SceneManagerView(std::shared_ptr<USceneManager> _SceneManager)
		: SceneManager(_SceneManager){}
	~SceneManagerView() = default;

	// 상태가 변화하면, 현재 SpawnActorMap을 복사하여 업데이트합니다.
	void Update()
	{
		if (auto sceneManagerShared = SceneManager.lock())
		{
			if (!sceneManagerShared->IsDirty())
				return;

			SpawnActorMapCopy = sceneManagerShared->RetrieveActorMap();
			sceneManagerShared->ClearDirty();
		}
	}
	const TMap<uint32, UObject*> GetActors() const { return SpawnActorMapCopy; };

private:
	std::weak_ptr<USceneManager> SceneManager;
	TMap<uint32, UObject*> SpawnActorMapCopy;

};