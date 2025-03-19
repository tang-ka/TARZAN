#pragma once
#include "Framework/Core/UArrowComponent.h"

class UScaleComponent : public UArrowComponent
{
public:
	UScaleComponent(EPrimitiveColor Color);
	~UScaleComponent();

protected:
	//for boundBox
	FVector minPos;
	FVector maxPos;
	// picking
	bool picked = false;
	EPrimitiveColor color;

	CBlendState* ArrowBlendState;

	bool bIsRender = false;
};

