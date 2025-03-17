#pragma once
#include "./Framework/Core/UPrimitiveComponent.h"

class UPlaneComponent :public UPrimitiveComponent {
public:
	UPlaneComponent();

private:
	void CreateBoundingBox(EPrimitiveType type) override;
};

