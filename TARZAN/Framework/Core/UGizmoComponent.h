#pragma once
#include "USceneComponent.h"

class UPrimitiveComponent;
class UArrowComponent;

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

    EPrimitiveColor selectedAxis = EPrimitiveColor::NONE;
    bool isGizmoActivated = false;
    
    std::unique_ptr<UArrowComponent> ArrowX;
    std::unique_ptr<UArrowComponent> ArrowY;
    std::unique_ptr<UArrowComponent> ArrowZ;

    UArrowComponent* temp;
};

