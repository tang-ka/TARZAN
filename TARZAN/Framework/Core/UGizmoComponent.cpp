#include "stdafx.h"
#include "UGizmoComponent.h"
#include "UArrowComponent.h"
#include "UScaleComponent.h"
#include "UDiscHollowComponent.h"

#define GIZMO_SELECT_MOUSE_SPEED 10.0f;

UGizmoComponent::UGizmoComponent()
{
    GizmoType = EGizmoType::Location;

    ArrowX = new UArrowComponent(EPrimitiveColor::RED_X);
    ArrowY = new UArrowComponent(EPrimitiveColor::GREEN_Y);
    ArrowZ = new UArrowComponent(EPrimitiveColor::BLUE_Z);

    ArrowX->AttachToComponent(this);
    ArrowY->AttachToComponent(this);
    ArrowZ->AttachToComponent(this);

    ArrowX->IsOverrideScale3D = true;
    ArrowY->IsOverrideScale3D = true;
    ArrowZ->IsOverrideScale3D = true;
    ArrowX->OverrideScale3D = { 1, 1, 1 };
    ArrowY->OverrideScale3D = { 1, 1, 1 };
    ArrowZ->OverrideScale3D = { 1, 1, 1 };

    ArrowX->SetRelativeRotation({ 0, M_PI  ,0 });
    ArrowY->SetRelativeRotation({ 0 ,0, M_PI / 2 });
    ArrowZ->SetRelativeRotation({ 0, -M_PI / 2 ,0 });

    //ArrowX->SetRelativeRotation({ 0, 0  ,0 });
    //ArrowY->SetRelativeRotation({ 0 ,0, M_PI / 2 });
    //ArrowZ->SetRelativeRotation({ 0, -M_PI / 2 ,0 });

    RotationX = new UDiscHollowComponent(EPrimitiveColor::RED_X, 0.95f);
    RotationY = new UDiscHollowComponent(EPrimitiveColor::GREEN_Y, 0.95f);
    RotationZ = new UDiscHollowComponent(EPrimitiveColor::BLUE_Z, 0.95f);

    RotationX->AttachToComponent(this);
    RotationY->AttachToComponent(this);
    RotationZ->AttachToComponent(this);

    RotationX->IsOverrideScale3D = true;
    RotationY->IsOverrideScale3D = true;
    RotationZ->IsOverrideScale3D = true;
    RotationX->OverrideScale3D = { 2.5, 2.5, 2.5 };
    RotationY->OverrideScale3D = { 2.5, 2.5, 2.5 };
    RotationZ->OverrideScale3D = { 2.5, 2.5, 2.5 };

    RotationX->SetRelativeRotation({ FLT_EPSILON, 0.5 , -M_PI / 2 - FLT_EPSILON });
    RotationY->SetRelativeRotation({ FLT_EPSILON,FLT_EPSILON,FLT_EPSILON });
    RotationZ->SetRelativeRotation({ M_PI / 2 - FLT_EPSILON,FLT_EPSILON,FLT_EPSILON });

    ScaleX = new UScaleComponent(EPrimitiveColor::RED_X);
    ScaleY = new UScaleComponent(EPrimitiveColor::GREEN_Y);
    ScaleZ = new UScaleComponent(EPrimitiveColor::BLUE_Z);

    ScaleX->AttachToComponent(this);
    ScaleY->AttachToComponent(this);
    ScaleZ->AttachToComponent(this);

    ScaleX->IsOverrideScale3D = true;
    ScaleY->IsOverrideScale3D = true;
    ScaleZ->IsOverrideScale3D = true;
    ScaleX->OverrideScale3D = { 1, 1, 1 };
    ScaleY->OverrideScale3D = { 1, 1, 1 };
    ScaleZ->OverrideScale3D = { 1, 1, 1 };

    ScaleX->SetRelativeRotation({ 0, M_PI  ,0 });
    ScaleY->SetRelativeRotation({ 0 ,0, M_PI / 2 });
    ScaleZ->SetRelativeRotation({ 0, -M_PI / 2 ,0 });
}

UGizmoComponent::~UGizmoComponent()
{
    ArrowX->AttachToComponent(nullptr);
    ArrowY->AttachToComponent(nullptr);
    ArrowZ->AttachToComponent(nullptr);
    ArrowX = ArrowY = ArrowZ = nullptr;

    RotationX->AttachToComponent(nullptr);
    RotationY->AttachToComponent(nullptr);
    RotationZ->AttachToComponent(nullptr);
    RotationX = RotationY = RotationZ = nullptr;

    ScaleX->AttachToComponent(nullptr);
    ScaleY->AttachToComponent(nullptr);
    ScaleZ->AttachToComponent(nullptr);
    ScaleX = ScaleY = ScaleZ = nullptr;
}
void UGizmoComponent::Update()
{
    if (GetAttachParent() == nullptr)
        return;

    // ImGui로 기본 정보 표시
    ImGui::Begin("Gizmo");
    ImGui::Text("Gizmo Comp Pos: %f %f %f", GetComponentLocation().x, GetComponentLocation().y, GetComponentLocation().z);
    ImGui::Text("Parent Rel Pos: %f %f %f", GetAttachParent()->GetRelativeLocation().x, GetAttachParent()->GetRelativeLocation().y, GetAttachParent()->GetRelativeLocation().z);
    ImGui::Text("Parent Comp Pos: %f %f %f", GetAttachParent()->GetComponentLocation().x, GetAttachParent()->GetComponentLocation().y, GetAttachParent()->GetComponentLocation().z);
    ImGui::End();

    // 현재 선택된 컴포넌트(예: Arrow 또는 Scale)를 초기화
    UArrowComponent* selectedComp = nullptr;
    UDiscHollowComponent* selectedDisc = nullptr;

    // GizmoType에 따라 다른 컴포넌트를 선택하고 입력 로직을 분기
    switch (GizmoType)
    {
    case EGizmoType::Location:
    {
        if (selectedAxis == EPrimitiveColor::RED_X)
            selectedComp = ArrowX;
        else if (selectedAxis == EPrimitiveColor::GREEN_Y)
            selectedComp = ArrowY;
        else if (selectedAxis == EPrimitiveColor::BLUE_Z)
            selectedComp = ArrowZ;
        break;
    }
    case EGizmoType::Rotation:
    {
        // 회전용 로직: 예를 들어 Arrow 컴포넌트를 사용해 회전 각도를 결정
        if (selectedAxis == EPrimitiveColor::RED_X_ROT)
            selectedDisc = RotationX;
        else if (selectedAxis == EPrimitiveColor::GREEN_Y_ROT)
            selectedDisc = RotationY;
        else if (selectedAxis == EPrimitiveColor::BLUE_Z_ROT)
            selectedDisc = RotationZ;
        break;
    }
    case EGizmoType::Scaling:
    {
        if (selectedAxis == EPrimitiveColor::RED_X)
            selectedComp = ScaleX;
        else if (selectedAxis == EPrimitiveColor::GREEN_Y)
            selectedComp = ScaleY;
        else if (selectedAxis == EPrimitiveColor::BLUE_Z)
            selectedComp = ScaleZ;
        break;
    }
    default:
        if (temp)
        {
            temp->renderFlags &= ~PRIMITIVE_FLAG_SELECTED;
            temp = nullptr;
        }
        if (temp1)
        {
            temp1->renderFlags &= ~PRIMITIVE_FLAG_SELECTED;
            temp1 = nullptr;
        }
        return;
    }

    // 선택된 컴포넌트 하이라이팅 처리
    if (selectedComp)
    {
        temp = selectedComp;
        temp->renderFlags |= PRIMITIVE_FLAG_SELECTED;
    }
    else if (selectedDisc)
    {
        temp1 = selectedDisc;
        temp1->renderFlags |= PRIMITIVE_FLAG_SELECTED;
    }
    else
    {
        if (temp)
        {
            temp->renderFlags &= ~PRIMITIVE_FLAG_SELECTED;
            temp = nullptr;
        }
        if (temp1)
        {
            temp1->renderFlags &= ~PRIMITIVE_FLAG_SELECTED;
            temp1 = nullptr;
        }
        return;
    }

    int xInt, yInt;
    Input::Instance()->GetMouseLocation(xInt, yInt);
    float x = xInt / static_cast<float>(SCR_WIDTH);
    float y = -yInt / static_cast<float>(SCR_HEIGHT);

    // 마우스 입력 처리
    int dxInt, dyInt;
    Input::Instance()->GetMouseDelta(dxInt, dyInt);
    float dx = dxInt / static_cast<float>(SCR_WIDTH);
    float dy = -dyInt / static_cast<float>(SCR_HEIGHT);

    // 마우스 이동이 거의 없으면 업데이트 중단
    if (abs(dx) < FLT_EPSILON && abs(dy) < FLT_EPSILON)
        return;

    ImGui::Begin("MouseDelta");
    ImGui::Text("MouseDelta: %f %f", dx, dy);
    ImGui::End();

    // 각 타입별 처리 로직
    auto cam = CRenderer::Instance()->GetMainCamera();
    switch (GizmoType)
    {
    case EGizmoType::Location:
    {
        FVector mouseDirOnScreen = { dx, dy, 0 };
        FVector arrowDirOnScreen = (FVector4(selectedComp->Right(), 0) * cam->View() * cam->PerspectiveProjection()).xyz();
        float effectiveMovement = mouseDirOnScreen.Dot(arrowDirOnScreen) * GIZMO_SELECT_MOUSE_SPEED;
        auto newPos = selectedComp->Right() * effectiveMovement + GetAttachParent()->GetRelativeLocation();
        GetAttachParent()->SetRelativeLocation(newPos);
        ImGui::Begin("Gizmo Attached");
        auto l = GetAttachParent()->GetRelativeLocation();
        ImGui::Text("Gizmo Attached Rel Pos: %f %f %f", l.x, l.y, l.z);
        ImGui::End();
        break;
    }
    case EGizmoType::Rotation:
    {
        int xInt, yInt;
        Input::Instance()->GetMouseLocation(xInt, yInt);
        float x = xInt / (float)SCR_WIDTH;
        float y = -yInt / (float)SCR_HEIGHT;

        FVector cursorPosOnScreen = { x,y,0 };

        int dxInt, dyInt;
        Input::Instance()->GetMouseDelta(dxInt, dyInt);

        float dx = dxInt / (float)SCR_WIDTH;
        float dy = -dyInt / (float)SCR_HEIGHT;

        FVector cursorDeltaOnScreen = { dx,dy,0 };

        auto cam = CRenderer::Instance()->GetMainCamera();
        FVector gizmoCenterOnScreen = (FVector4(GetComponentLocation(), 1.f) * cam->View() * cam->PerspectiveProjection()).GetCoord();
        gizmoCenterOnScreen = FVector(gizmoCenterOnScreen.x, gizmoCenterOnScreen.y, 0);
        FVector cursorToGizmo = gizmoCenterOnScreen - cursorPosOnScreen;

        FVector effectiveMovementVector = cursorToGizmo.Cross(cursorDeltaOnScreen);
        float effectiveMovementClockwise = effectiveMovementVector.z;

        //FVector discClockWiseOnScreen = (FVector4(selectedDisc->Right() * -1, 0) * cam->View() * cam->PerspectiveProjection()).xyz();
        // normalize?

        //float effectiveMovement = mouseDirOnScreen.Dot(discClockWiseOnScreen);
        effectiveMovementClockwise *= -GIZMO_SELECT_MOUSE_SPEED_ROTATION;
        FVector rotDirection = { 0,0,0 };
        if (selectedAxis == EPrimitiveColor::RED_X_ROT)
        {
            rotDirection = FVector(1, 0, 0);
        }
        else if (selectedAxis == EPrimitiveColor::GREEN_Y_ROT)
        {
            rotDirection = FVector(0, 1, 0);
        }
        else if (selectedAxis == EPrimitiveColor::BLUE_Z_ROT)
        {
            rotDirection = FVector(0, 0, -1);
        }
        //FVector rotationDirection = { selectedDisc->GetRelativeRotation().x , selectedDisc->GetRelativeRotation().y, selectedDisc->GetRelativeRotation().z };
        auto newRot = rotDirection * effectiveMovementClockwise + GetAttachParent()->GetRelativeRotation();
        GetAttachParent()->SetRelativeRotation(newRot);
        break;
    }
    case EGizmoType::Scaling:
    {
        // 스케일 조정 비율
        float scaleSpeed = 1.0f;

        FVector currentScale = GetAttachParent()->GetRelativeScale3D();

        switch (selectedAxis)
        {
        case EPrimitiveColor::RED_X:
            // X 축은 마우스의 x 값 사용
            currentScale.x *= (1.0f + dx * scaleSpeed);
            break;
        case EPrimitiveColor::GREEN_Y:
            // Y 축은 마우스의 y 값 사용
            currentScale.y *= (1.0f + dy * scaleSpeed);
            break;
        case EPrimitiveColor::BLUE_Z:
            // Z 축은 기본적으로 마우스의 x 값 사용 (필요에 따라 수정 가능)
            currentScale.z *= (1.0f + dx * scaleSpeed);
            break;
        default:
            break;
        }

        GetAttachParent()->SetRelativeScale3D(currentScale);

        ImGui::Begin("Gizmo Scale");
        ImGui::Text("New Scale: %f %f %f", currentScale.x, currentScale.y, currentScale.z);
        ImGui::End();
        break;
    }
    default:
        break;
    }
}

void UGizmoComponent::Render()
{
    if (isGizmoActivated)
    {

        CTextureManager::GetInstance()->BindTextureToShader(EObjectType::Object);

        switch (GizmoType)
        {
        case Location:
            ArrowX->Render();
            ArrowY->Render();
            ArrowZ->Render();
            break;
        case Rotation:
            RotationY->Render();
            RotationZ->Render();
            RotationX->Render();
            break;
        case Scaling:
            ScaleX->Render();
            ScaleY->Render();
            ScaleZ->Render();
            break;
        default:
            break;
        }
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
    if (temp)
    {
        temp->renderFlags &= ~PRIMITIVE_FLAG_SELECTED;

        temp = nullptr;
    }
}

void UGizmoComponent::SwitchGizmoType()
{
    if (GizmoType == EGizmoType::Location)
    {
        GizmoType = EGizmoType::Rotation;
    }
    else if (GizmoType == EGizmoType::Rotation)
    {
        GizmoType = EGizmoType::Scaling;
    }
    else
    {
        GizmoType = EGizmoType::Location;
    }
}
