#pragma once
#include "USceneComponent.h"
#include "./Framework/DirectXWrapper/CBuffer.h"
#include "./Framework/DirectXWrapper/CInputLayout.h"
#include "./Framework/Core/CRenderer.h"
#include "FBoundingBox.h"

class FBoundingBox;

class UPrimitiveComponent: public USceneComponent {
public:
	UPrimitiveComponent(): renderFlags(0) {}
	virtual ~UPrimitiveComponent();

public:
	uint32 renderFlags;
	TArray<FVertexSimple> vertices = {};
	TArray<uint32> indices = {};
	bool isShowBoundingBox = false;

public:
	virtual void Render();
	virtual bool IntersectsRay(const FVector& rayOrigin, const FVector& rayDir, float& dist) { return false; }
	virtual void GenerateRayForPicking(const FVector& pickPosition, const FMatrix& viewMatrix, FVector* pickRayOrigin, FVector* rayDirection) override;
	virtual int PickObjectByRayIntersection(const FVector& pickPosition, const FMatrix& viewMatrix, float* hitDistance) override;
	bool IntersectRayTriangle(const FVector& rayOrigin, const FVector& rayDirection, const FVector& v0, const FVector& v1, const FVector& v2, float& hitDistance);

	virtual int CheckRayIntersection(FVector& rayOrigin, FVector& rayDirection, float* pfNearHitDistance);

protected:
	virtual void CreateBoundingBox(EPrimitiveType type) {}

private :
	//void UpdateBoundingBox();
	void CreateBoundingBoxBuffer();
	void RenderBoundingBox();

private:
	CVertexBuffer<FVertexSimple>* _boundingBoxVertexBuffer = nullptr;
	CIndexBuffer* _boundingBoxIndexBuffer = nullptr;

protected:
	FBoundingBox* boundingBox;
	CVertexBuffer<FVertexSimple>* _vertexBuffer = nullptr;
	CIndexBuffer* _indexBuffer = nullptr;
};