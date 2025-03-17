#pragma once
#include "Framework/Core/UActorComponent.h"

class USceneComponent :public UActorComponent {
public:
	USceneComponent() {}
	virtual void Update();
	//virtual void Render();
	FVector RelativeLocation = FVector::Zero;
	FVector RelativeRotation = FVector::Zero;
	FVector RelativeScale3D = FVector::One;
private:
	

public:
	FVector Right();
	FVector Up();
	FVector Front();
	static const FVector PseudoUp;
	//FMatrix Transformation();

public:
	FVector GetRelativeLocation() const;
	FVector4 GetRelativeLocation4() const;
	FVector GetRelativeRotation() const;
	FVector GetRelativeScale3D() const;
	FMatrix GetRelativeTransform() const;

	FVector GetComponentLocation() const;
	//FVector GetComponentRotation() const;
	//FVector GetComponentScale() const;
	FMatrix GetComponentTransform() const;
	//FVector GetComponentVelocity() const;
	FVector GetComponentX() const;
	FVector GetComponentY() const;
	FVector GetComponentZ() const;
	FVector GetComponentInverseRotation() const;

	void SetRelativeLocation(const FVector NewLocation);
	void SetRelativeLocationX(const float NewLocX);
	void SetRelativeLocationY(const float NewLocY);
	void SetRelativeLocationZ(const float NewLocZ);
	void SetRelativeRotation(const FVector NewRotation);
	void SetRelativeRotationX(const float NewRotX);
	void SetRelativeRotationY(const float NewRotY);
	void SetRelativeRotationZ(const float NewRotZ);
	void SetRelativeScale3D(const FVector NewScale3D);
	void SetRelativeScale3DX(const float NewScaleX);
	void SetRelativeScale3DY(const float NewScaleY);
	void SetRelativeScale3DZ(const float NewScaleZ);
	//void SetComponentVelocity(const FVector NewVelocity);

	void SetWorldLocation(const FVector NewWorldLocation);
	void SetWorldRotation(const FVector NewWorldRotation);
	void SetWorldLocationAndRotation(const FVector NewWorldLocation);
	//void SetWorldScale3D(const FVector NewWorldScale3D);
	//void SetWorldVelocity(const FVector NewWorldVelocity);

public:
	// ì£¼ì˜!! hierarchy??ë°”ë€Œì? ?ŠìŒ 
	void SetValuesFromMatrix(const FMatrix mat);
	//void UpdateComponentVelocity();

// ï¿½ï¿½ï¿?ï¿½ï¿½ï¿½ï¿½
private:
	USceneComponent* AttachParent = nullptr;
	TArray<USceneComponent*> AttachChildern;

public:
	const TArray<USceneComponent*>& GetAttachChildren() const; // ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ childrenï¿½ï¿½ ï¿½ï¿½È¯
	void GetChildrenComponents(TArray<USceneComponent*>& Children) const; // ï¿½ï¿½Í·ï¿?ï¿½Æ·ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿?children ï¿½ï¿½È¯
	USceneComponent* GetAttachParent() const;
	void GetParentComponents(TArray<USceneComponent*>& Parents) const; // ï¿½ï¿½Í·ï¿?rootï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½È¯

	void SetupAttachment(TArray<USceneComponent*>& Children); // ï¿½Ú½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½

	// ï¿½Î¸ï¿½ï¿½Ú½ï¿½ ï¿½ï¿½ï¿½è¸¦ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½Ï´ï¿½ ï¿½ï¿½ ï¿½Ô¼ï¿½ ï¿½ï¿½ï¿½ï¿½
	bool AttachToComponent(USceneComponent* Parent); // ï¿½Î¸ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ false

// ï¿½ï¿½ï¿½ï¿½ï¿?
public:
	void PrintLoc(std::wstring msg) const;


public:
	//FMatrix DEBUG_TRANSFORMATION_OVERRIDE = FMatrix::Identity;
	// flag ?¤ì •??ê°•ì œë¡????íƒœë¡??Œë”
	FVector OverrideLocation = FVector::Zero;
	FVector OverrideRotation = FVector::Zero;
	FVector OverrideScale3D = FVector::One;
	FMatrix OverrideTransform = FMatrix::Identity;

	//bool ISDEBUG = false;

	bool IsOverrideTransform = false;
	bool IsOverrideLocation = false;
	bool IsOverrideRotation = false;
	bool IsOverrideScale3D = false;

};

