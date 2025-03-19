#pragma once
#include "Framework/Core/UPrimitiveComponent.h"
static const FVector AxisColors[] = {
	FVector(1.0f, 0.0f, 0.0f),
	FVector(0.0f, 1.0f, 0.0f),
	FVector(0.0f, 0.0f, 1.0f)
};

class UArrowComponent:public UPrimitiveComponent
{
public:
	UArrowComponent() = default;
	UArrowComponent(EPrimitiveColor axis);
	~UArrowComponent();
	virtual void Render();
	bool IsPicked() { return picked; }
	void SetPicked(bool pik) { picked = pik; }
private:
	//for boundBox
	FVector minPos;
	FVector maxPos;
	// picking
	bool picked = false;
	EPrimitiveColor color;
	
	std::unique_ptr<CBlendState> ArrowBlendState;
};

