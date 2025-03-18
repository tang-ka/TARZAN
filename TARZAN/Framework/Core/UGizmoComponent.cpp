#include "stdafx.h"
#include "UGizmoComponent.h"
#include "UArrowComponent.h"

#define GIZMO_SELECT_MOUSE_SPEED 10.0f;

UGizmoComponent::UGizmoComponent()
{
	ArrowX = std::make_unique<UArrowComponent>(EPrimitiveColor::RED_X);
	ArrowY = std::make_unique<UArrowComponent>(EPrimitiveColor::GREEN_Y);
	ArrowZ = std::make_unique<UArrowComponent>(EPrimitiveColor::BLUE_Z);

	ArrowX->AttachToComponent(this);
	ArrowY->AttachToComponent(this);
	ArrowZ->AttachToComponent(this);

	ArrowX->IsOverrideScale3D = true;
	ArrowY->IsOverrideScale3D = true;
	ArrowZ->IsOverrideScale3D = true;
	ArrowX->OverrideScale3D = { 1, 1, 1 };
	ArrowY->OverrideScale3D = { 1, 1, 1 };
	ArrowZ->OverrideScale3D = { 1, 1, 1 };

	ArrowX->SetRelativeRotation({ 0,-M_PI / 2,0 });
	ArrowY->SetRelativeRotation({ M_PI / 2 ,0,0 });
	ArrowZ->SetRelativeRotation({ 0,0,0 });

}

UGizmoComponent::~UGizmoComponent()
{
	ArrowX->AttachToComponent(nullptr);
	ArrowY->AttachToComponent(nullptr);
	ArrowZ->AttachToComponent(nullptr);
	ArrowX = nullptr;
	ArrowY = nullptr;
	ArrowZ = nullptr;

}

void UGizmoComponent::Update()
{
	//ArrowX->IsOverrideLocation = true;
	//ArrowY->IsOverrideLocation = true;
	//ArrowZ->IsOverrideLocation = true;

	//ArrowX->IsOverrideRotation = true;
	//ArrowY->IsOverrideRotation = true;
	//ArrowZ->IsOverrideRotation = true;

	//ArrowX->IsOverrideScale3D = true;
	//ArrowY->IsOverrideScale3D = true;
	//ArrowZ->IsOverrideScale3D = true;

 	if (GetAttachParent() == nullptr)
		return;

	ImGui::Begin("Gizmo");
	//ImGui::Text("Gizmo Rel Pos: %f %f %f", RelativeLocation.x, RelativeLocation.y, RelativeLocation.z);
	ImGui::Text("Gizmo Comp Pos: %f %f %f", GetComponentLocation().x, GetComponentLocation().y, GetComponentLocation().z);
	ImGui::Text("Parent Rel Pos: %f %f %f", GetAttachParent()->GetRelativeLocation().x, GetAttachParent()->GetRelativeLocation().y, GetAttachParent()->GetRelativeLocation().z);
	ImGui::Text("Parent Comp Pos: %f %f %f", GetAttachParent()->GetComponentLocation().x, GetAttachParent()->GetComponentLocation().y, GetAttachParent()->GetComponentLocation().z);
	ImGui::End();


	UArrowComponent* selectedArrow = nullptr;
	if (selectedAxis == EPrimitiveColor::RED_X) selectedArrow = ArrowX.get();
	else if (selectedAxis == EPrimitiveColor::GREEN_Y) selectedArrow = ArrowY.get();
	else if (selectedAxis == EPrimitiveColor::BLUE_Z) selectedArrow = ArrowZ.get();
	else {
		if (temp) {
			temp->renderFlags &= ~PRIMITIVE_FLAG_SELECTED;
			temp = nullptr;
		}
		return;
	}
	if (selectedArrow) {
		temp = selectedArrow;
		temp->renderFlags |= PRIMITIVE_FLAG_SELECTED;
	}

	int dxInt, dyInt;
	Input::Instance()->GetMouseDelta(dxInt, dyInt);
	float dx = dxInt / (float)SCR_WIDTH;
	float dy = -dyInt / (float)SCR_HEIGHT;
		
 	FVector mouseDirOnScreen = { dx,dy,0 };
	if (abs(dx) < FLT_EPSILON && abs(dy) < FLT_EPSILON) return;
	ImGui::Begin("MouseDelta");
	ImGui::Text("MouseDelta: %f %f", dx, dy);
	ImGui::End();

	auto cam = CRenderer::Instance()->GetMainCamera();
	FVector arrowDirOnScreen = (FVector4(selectedArrow->Front(), 0) * cam->View() * cam->PerspectiveProjection()).xyz();

	float effectiveMovement = mouseDirOnScreen.Dot(arrowDirOnScreen);
	effectiveMovement *= GIZMO_SELECT_MOUSE_SPEED;

 	auto newPos = selectedArrow->Front() * effectiveMovement + GetAttachParent()->GetRelativeLocation();
	GetAttachParent()->SetRelativeLocation(newPos);
	ImGui::Begin("Gizmo Attached");
	auto l =GetAttachParent()->GetRelativeLocation();
	ImGui::Text("Gizmo Attached Rel Pos: %f %f %f", l.x, l.y, l.z);
	ImGui::End();
}

void UGizmoComponent::Render()
{
	if (isGizmoActivated) {
	
		CTextureManager::GetInstance()->BindTextureToShader(EObjectType::Object);

		ArrowX->Render();
		ArrowY->Render();
		ArrowZ->Render();
	}
}

void UGizmoComponent::AttachTo(UPrimitiveComponent* Parent)
{
	isGizmoActivated = true;
	this->AttachToComponent(Parent);
}

void UGizmoComponent::Detach()
{
	isGizmoActivated = false;
	selectedAxis = EPrimitiveColor::NONE;
	//UE_LOG(L"Detach!!!!!!!\n");
	this->AttachToComponent(nullptr);
	if (temp) {
		temp->renderFlags &= ~PRIMITIVE_FLAG_SELECTED;
		
		temp = nullptr;
	}
}
