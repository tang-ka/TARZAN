#pragma once
#define GIZMO_SELECT_MOUSE_SPEED_ROTATION 30.0f;

#include "USceneComponent.h"

class UPrimitiveComponent;
class UArrowComponent;
class UScaleComponent;
class UDiscHollowComponent;

class UGizmoComponent :
    public USceneComponent
{
public:
    UGizmoComponent();
    //UGizmoComponent();
    ~UGizmoComponent();
    virtual void Update();
    virtual void Render() override;
    void AttachTo(UPrimitiveComponent* Parent);
    void Detach();

    void SwitchGizmoType();

    EPrimitiveColor selectedAxis = EPrimitiveColor::NONE;
    bool isGizmoActivated = false;
    
    UArrowComponent* ArrowX;
    UArrowComponent* ArrowY;
    UArrowComponent* ArrowZ;
    UArrowComponent* temp;

    /* Rotation */
    UDiscHollowComponent* RotationX;
    UDiscHollowComponent* RotationY;
    UDiscHollowComponent* RotationZ;
    UDiscHollowComponent* temp1;

    /* Scaling */
    UScaleComponent* ScaleX;
    UScaleComponent* ScaleY;
    UScaleComponent* ScaleZ;

    EGizmoType GizmoType;
};

