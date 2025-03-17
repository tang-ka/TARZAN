#pragma once
#include "Framework/Core/UActorComponent.h"
class USceneComponent :public UActorComponent {
public:
	USceneComponent() {}
	virtual ~USceneComponent() = default;
	virtual void Update();
	//virtual void Render();
private:
	FVector RelativeLocation = FVector::Zero;
	FVector RelativeRotation = FVector::Zero;
	FVector RelativeScale3D = FVector::One;

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
	void SetValuesFromMatrix(const FMatrix mat);
	//void UpdateComponentVelocity();

private:
	USceneComponent* AttachParent = nullptr;
	TArray<USceneComponent*> AttachChildern;

public:
	const TArray<USceneComponent*>& GetAttachChildren() const;
	void GetChildrenComponents(TArray<USceneComponent*>& Children) const;
	USceneComponent* GetAttachParent() const;
	void GetParentComponents(TArray<USceneComponent*>& Parents) const;

	void SetupAttachment(TArray<USceneComponent*>& Children);

	bool AttachToComponent(USceneComponent* Parent);

public:
	void PrintLoc(std::wstring msg) const;


public:
	//FMatrix DEBUG_TRANSFORMATION_OVERRIDE = FMatrix::Identity;
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

