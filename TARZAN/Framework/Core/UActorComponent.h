#pragma once
#include "Framework/Core/UObject.h"

enum class EObjectType {
	Object,   // 일반 3D 오브젝트
	Text    // 텍스트(폰트) 오브젝트
};

class UActorComponent :public UObject {	
public:
	EObjectType ObjType;

	UActorComponent() {}
	virtual ~UActorComponent() {}

	virtual void Update() {};
	virtual void Render() {};
	//virtual void GenerateRayForPicking(FVector& pickPosition, FMatrix& viewMatrix, FVector* pickRayOrigin, FVector* rayDirection) {};
	virtual void GenerateRayForPicking(const FVector& pickPosition, const FMatrix& viewMatrix, FVector* pickRayOrigin, FVector* rayDirection) {}
	virtual int PickObjectByRayIntersection(const FVector& pickPosition, const FMatrix& viewMatrix, float* hitDistance) { return false; }
};

