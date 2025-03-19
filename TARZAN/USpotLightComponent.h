#pragma once
#include "./Framework/Core/UPrimitiveComponent.h"

class USpotLightComponent :public UPrimitiveComponent {
public:
	USpotLightComponent();

private:
	void CreateBoundingBox(EPrimitiveType type) override;
};

