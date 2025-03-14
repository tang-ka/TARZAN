#include "stdafx.h"
#include "USceneComponent.h"
const FVector USceneComponent::PseudoUp = { 0,1,0 };

void USceneComponent::Update()
{
	UActorComponent::Update();
}

FVector USceneComponent::Right() {
	FVector4 r = GetComponentTransform().r1(); // (1,0,0,0) * getcomponenttransform
	return FVector(r.x, r.y, r.z).Normalized();
}

FVector USceneComponent::Up() {
	FVector4 r = GetComponentTransform().r2();
	return FVector(r.x, r.y, r.z).Normalized();
}

FVector USceneComponent::Front() {
	FVector4 r = GetComponentTransform().r3();
	return FVector(r.x, r.y, r.z).Normalized();
}

FVector USceneComponent::GetRelativeLocation() const
{
	if (IsOverrideLocation) return OverrideLocation;
	return RelativeLocation;
}

FVector4 USceneComponent::GetRelativeLocation4() const
{
	if (IsOverrideLocation) return FVector4(OverrideLocation,1);
	return FVector4(RelativeLocation, 1);
}

FVector USceneComponent::GetRelativeRotation() const
{
	if (IsOverrideRotation) return OverrideRotation;
	return RelativeRotation;
}

FVector USceneComponent::GetRelativeScale3D() const
{
	if (IsOverrideScale3D) return OverrideScale3D;
	return RelativeScale3D;
}

FMatrix USceneComponent::GetRelativeTransform() const
{
	if (IsOverrideTransform) return OverrideTransform;

	FMatrix origin = FMatrix::Identity;
	FMatrix scale = FMatrix::Scale(RelativeScale3D);
	FMatrix rot = FMatrix::RotateX(RelativeRotation.x) * FMatrix::RotateY(RelativeRotation.y) * FMatrix::RotateZ(RelativeRotation.z);
	FMatrix trans = FMatrix::Translate(RelativeLocation);

	if (IsOverrideLocation) {
		trans = FMatrix::Translate(OverrideLocation);
	}
	if (IsOverrideRotation) {
		rot = FMatrix::RotateX(OverrideRotation.x) * FMatrix::RotateY(OverrideRotation.y) * FMatrix::RotateZ(OverrideRotation.z);
	}
	if (IsOverrideScale3D) {
		scale = FMatrix::Scale(OverrideScale3D);
	}

	return origin * scale * rot * trans;
}

FVector USceneComponent::GetComponentLocation() const
{
	if (IsOverrideLocation) {
		return OverrideLocation;
	}

	if (AttachParent != nullptr)
	{
		FVector4 componentVec = (FVector4(GetRelativeLocation(), 1) * AttachParent->GetComponentTransform());
		return FVector(componentVec.xyz() / componentVec.w);
	}
	else
	{
		return GetRelativeLocation();
	}
}

FMatrix USceneComponent::GetComponentTransform() const
{
	if (IsOverrideTransform)
	{
		return OverrideTransform;
	}
	if (IsOverrideLocation || IsOverrideRotation || IsOverrideScale3D)
	{
		return GetRelativeTransform() * AttachParent->GetComponentTransform();
		//FMatrix origin = FMatrix::Identity;

		//FMatrix scale = FMatrix::Scale(OverrideScale3D);
		//FMatrix rot = FMatrix::RotateX(OverrideRotation.x) * FMatrix::RotateY(OverrideRotation.y) * FMatrix::RotateZ(OverrideRotation.z);
		//FMatrix trans = FMatrix::Translate(OverrideLocation);
		//return origin * scale * rot * trans;
	}
	if (AttachParent != nullptr)
	{
		return GetRelativeTransform() * AttachParent->GetComponentTransform();
	}
	else
	{
		return GetRelativeTransform();
	}
}

FVector USceneComponent::GetComponentX() const
{
	return (FVector4(1, 0, 0, 0) * GetComponentTransform()).xyz();
}

FVector USceneComponent::GetComponentY() const
{
	return (FVector4(0, 1, 0, 0) * GetComponentTransform()).xyz();
}

FVector USceneComponent::GetComponentZ() const
{
	return (FVector4(0, 0, 1, 0) * GetComponentTransform()).xyz();
}

FVector USceneComponent::GetComponentInverseRotation() const
{
	
	FMatrix inverseRotaion = FMatrix::Identity;

	USceneComponent* ParentIterator = GetAttachParent();
	while (ParentIterator != nullptr)
	{
		FVector rot = ParentIterator->GetRelativeRotation();
		FMatrix matRotZ = FMatrix::RotateZ(rot.z);
		FMatrix matRotY = FMatrix::RotateZ(rot.y);
		FMatrix matRotX = FMatrix::RotateZ(rot.x);
		
		inverseRotaion = inverseRotaion * matRotZ * matRotY * matRotX;
		ParentIterator = ParentIterator->GetAttachParent();
	}

	FVector rotAngles;
	rotAngles.x = atan(-inverseRotaion[2][0] / sqrt(inverseRotaion[0][0] * inverseRotaion[0][0]
		+ inverseRotaion[1][0] * inverseRotaion[1][0]));
	rotAngles.y = atan(inverseRotaion[1][0] / inverseRotaion[0][0]);
	rotAngles.z = atan(inverseRotaion[2][1] / inverseRotaion[2][2]);
	return rotAngles;
}

void USceneComponent::SetRelativeLocation(const FVector NewLocation)
{
	RelativeLocation = NewLocation;
}

void USceneComponent::SetRelativeLocationX(const float NewLocX)
{
	RelativeLocation.x = NewLocX;
}

void USceneComponent::SetRelativeLocationY(const float NewLocY)
{
	RelativeLocation.y = NewLocY;
}

void USceneComponent::SetRelativeLocationZ(const float NewLocZ)
{
	RelativeLocation.z = NewLocZ;
}

void USceneComponent::SetRelativeRotation(const FVector NewRotation)
{
	RelativeRotation = NewRotation;
}

void USceneComponent::SetRelativeRotationX(const float NewRotX)
{
	RelativeRotation.x = NewRotX;
}

void USceneComponent::SetRelativeRotationY(const float NewRotY)
{
	RelativeRotation.y = NewRotY;
}

void USceneComponent::SetRelativeRotationZ(const float NewRotZ)
{
	RelativeRotation.z = NewRotZ;
}

void USceneComponent::SetRelativeScale3D(const FVector NewScale3D)
{
	RelativeScale3D = NewScale3D;
}

void USceneComponent::SetRelativeScale3DX(const float NewScaleX)
{
	RelativeScale3D.x = NewScaleX;
}

void USceneComponent::SetRelativeScale3DY(const float NewScaleY)
{
	RelativeScale3D.y = NewScaleY;
}

void USceneComponent::SetRelativeScale3DZ(const float NewScaleZ)
{
	RelativeScale3D.z = NewScaleZ;
}

void USceneComponent::SetWorldLocation(const FVector NewWorldLocation)
{
	if (GetAttachParent() != nullptr)
	{
		SetRelativeLocation((GetRelativeLocation4() * GetAttachParent()->GetComponentTransform().Inverse()).xyz());
	}
	else
	{
		SetRelativeLocation(NewWorldLocation);
	}
}

void USceneComponent::SetWorldRotation(const FVector NewWorldRotation)
{
	if (GetAttachParent() != nullptr)
	{
		SetRelativeRotation((FVector4(GetRelativeRotation(), 0) * GetAttachParent()->GetComponentTransform().Inverse()).xyz());
	}
	else
	{
		SetRelativeLocation(NewWorldRotation);
	}
}

void USceneComponent::SetWorldLocationAndRotation(const FVector NewWorldLocation)
{
	FMatrix invTrans = GetAttachParent()->GetComponentTransform().Inverse();
	SetRelativeLocation((GetRelativeLocation4() * invTrans).xyz());
	SetRelativeRotation((FVector4(GetRelativeRotation(), 0) * invTrans).xyz());
}

void USceneComponent::SetValuesFromMatrix(const FMatrix mat)
{
	// 1 for linear transform, nonlinear otherwise(not SRT transform)
	assert(mat.m[3][3] == 1);

	RelativeLocation = { mat.m[0][3] ,mat.m[1][3],mat.m[2][3] };
	RelativeScale3D = sqrt(mat.m[0][0] * mat.m[0][0] + mat.m[0][1] * mat.m[0][1] * mat.m[0][2]*mat.m[0][2]);

	// https://www.geometrictools.com/Documentation/EulerAngles.pdf
	FMatrix r = mat * (1.0 / RelativeScale3D.x);
	float x, y, z;
	if (r[0][2] < 1)
	{
		if (r[0][2] > -1)
		{
			y = asin(r[0][2]);
			x = atan2(-r[1][2], r[2][2]);
			z = atan2(-r[0][1], r[0][0]);
		}
		else // r02 = -1; not a unique solution : z - x = atan2(r[1][0], r[1][1])
		{
			y = -M_PI / 2;
			x = -atan2(r[1][0], r[1][1]);
			z = 0;
		}
	}
	else // r02 = +1; not a unique solution : z+x = atan2(r10,r11)
	{
		y = M_PI / 2;
		x = atan2(r[1][0], r[1][1]);
		z = 0;
	}
	RelativeRotation = { x,y,z };
}

const TArray<USceneComponent*>& USceneComponent::GetAttachChildren() const
{
	return AttachChildern;
}

void USceneComponent::GetChildrenComponents(TArray<USceneComponent*>& Children) const
{
	Children.clear();
	for (auto& child : Children)
	{
		TArray<USceneComponent*> childComponents;
		child->GetChildrenComponents(childComponents);
		Children.insert(Children.end(), childComponents.begin(), childComponents.end());
	}
}

USceneComponent* USceneComponent::GetAttachParent() const
{
	return AttachParent;
}

void USceneComponent::GetParentComponents(TArray<USceneComponent*>& Parents) const
{
	Parents.clear();

	USceneComponent* ParentIterator = GetAttachParent();
	while (ParentIterator != nullptr)
	{
		Parents.push_back(ParentIterator);
		ParentIterator = ParentIterator->GetAttachParent();
	}
}

// children ?¤ì •
void USceneComponent::SetupAttachment(TArray<USceneComponent*>& Children)
{
	AttachChildern.clear();

	for (auto& child : Children)
	{
		if (!child->AttachToComponent(this))
		{
			UE_LOG(L"USceneComponent::SetupAttachment::Children");
		}
	}
}


bool USceneComponent::AttachToComponent(USceneComponent* Parent)
{
	if (Parent == this)
	{
		UE_LOG(L"USceneComponent::AttachToComponent::");
		return false;
	}

	if (Parent != nullptr) {
		TArray<USceneComponent*> parentsOfInput;
		Parent->GetParentComponents(parentsOfInput);
		if (parentsOfInput.end() != std::find(parentsOfInput.begin(), parentsOfInput.end(), this))
		{
			UE_LOG(L"USceneComponent::AttachToComponent::Descendent");
			return false;
		}
	}

	if (GetAttachParent() != nullptr)
	{
		TArray<USceneComponent*> currentChildrenOfParent = GetAttachParent()->GetAttachChildren();

		auto it = std::find(currentChildrenOfParent.begin(), currentChildrenOfParent.end(), this);
		assert(it != currentChildrenOfParent.end());

		currentChildrenOfParent.erase(it);
		GetAttachParent()->SetupAttachment(currentChildrenOfParent);

	}

	if (Parent == nullptr)
	{
		AttachParent = nullptr;

		return true;
	}

	AttachParent = Parent;
	Parent->AttachChildern.push_back(this);
	return true;
}

void USceneComponent::PrintLoc(std::wstring msg) const
{
	FVector loc = GetRelativeLocation();
	loc = GetComponentLocation();
}

