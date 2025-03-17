#pragma once
#include "USceneComponent.h"
#include "./Framework/DirectXWrapper/CBuffer.h"
#include "./Framework/DirectXWrapper/CInputLayout.h"
#include "./Framework/Core/CRenderer.h"
#include "FBoundingBox.h"

class FBoundingBox;

class UPrimitiveComponent: public USceneComponent {
public:
	UPrimitiveComponent(): renderFlags(0), StencilRefNum(1) {}
	virtual ~UPrimitiveComponent();

public:
	uint32 renderFlags;
	TArray<FVertexSimple> vertices = {};
	TArray<uint32> indices = {};

public:
	bool isBill;

	virtual void Render();
	virtual bool IntersectsRay(const FVector& rayOrigin, const FVector& rayDir, float& dist) { return false; }
	virtual void GenerateRayForPicking(const FVector& pickPosition, const FMatrix& viewMatrix, FVector* pickRayOrigin, FVector* rayDirection) override;
	virtual int PickObjectByRayIntersection(const FVector& pickPosition, const FMatrix& viewMatrix, float* hitDistance) override;
	bool IntersectRayTriangle(const FVector& rayOrigin, const FVector& rayDirection, const FVector& v0, const FVector& v1, const FVector& v2, float& hitDistance);

	virtual int CheckRayIntersection(FVector& rayOrigin, FVector& rayDirection, float* pfNearHitDistance);

	void ShowBoundingBox() { isShowBoundingBox = true; }
	void HideBoundingBox() { isShowBoundingBox = false; }

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
	bool isShowBoundingBox = false;
	FBoundingBox* boundingBox;
	CVertexBuffer<FVertexSimple>* _vertexBuffer = nullptr;
	CIndexBuffer* _indexBuffer = nullptr;

	uint32 renderFlags;

protected:
	std::unique_ptr<CVertexBuffer<FVertexSimple>> _vertexBuffer;
	std::unique_ptr<CIndexBuffer> _indexBuffer;
	CDepthStencilState* DepthStencilState = nullptr;
	CBlendState* BlendState = nullptr;
};