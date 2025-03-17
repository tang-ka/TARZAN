#pragma once
#include "stdafx.h"
#include "./Framework/Core/CRenderer.h"
//class FVertexSimple;

class FBoundingBox
{
public:
	FBoundingBox() :
		minPoint(FVector(FLT_MAX, FLT_MAX, FLT_MAX)),
		maxPoint(FVector(FLT_MIN, FLT_MIN, FLT_MIN))
	{
		BasicBoxInitialize();
	}

	FBoundingBox(const FVector& min, const FVector& max)
		: minPoint(min), maxPoint(max)
	{
		GenerateVertices();
		GenerateIndices();
	}
	 
public:
	void ExpandToInclude(const FVector& point);
	void Merge(const FBoundingBox& other);
	void SetBoundaryPointsForPrimitive(EPrimitiveType type);
	void SetBoundaryPoints(TArray<FVector4> points) { boundaryPoints = points; }
	void UpdateVerticesByBP(FMatrix mvpMatrix);

	void GenerateVertices();
	void GenerateVertices(FVector min, FVector max, FVector4 color = { 1.f, 1.f, 1.f, 1.f });
	void GenerateVerticesByBasicBox();
	void GenerateIndices();

	FVector4 SetColor(FVector4 color) { this->color = color; }
	TArray<FVertexSimple> GetVertices() { return vertices; }
	TArray<uint32> GetIndices() { return indices; }
	FVector GetCenter() const { return (minPoint + maxPoint) * 0.5f; }
	FVector GetExtent() const { return (maxPoint - minPoint) * 0.5f; }
	FVector GetSize() const { return maxPoint - minPoint; }
	bool Contains(const FVector& point) const;

private:
	void BasicBoxInitialize();

private:
	FVector minPoint;
	FVector maxPoint;
	FVector4 color = { 0.2235f, 1.f, 0.0784f, 1.f };

	TArray<FVertexSimple> vertices;
	TArray<uint32> indices;

	TArray<FVector4> boundaryPoints;

	FVector basicMinPoint;
	FVector basicMaxPoint;
	TArray<FVertexSimple> basicBoxVertices;
};

inline bool FBoundingBox::Contains(const FVector& point) const
{
	return (point.x >= minPoint.x && point.x <= maxPoint.x
		&& point.y >= minPoint.y && point.y <= maxPoint.y
		&& point.z >= minPoint.z && point.z <= maxPoint.z);
}
