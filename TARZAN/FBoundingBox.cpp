#include "stdafx.h"
#include "FBoundingBox.h"
#include "./Framework/Core/CRenderer.h"

void FBoundingBox::ExpandToInclude(const FVector& point)
{

}

void FBoundingBox::Merge(const FBoundingBox& other)
{

}

TArray<FVertexSimple> FBoundingBox::GenerateVertices(FVector4 color)
{
	TArray<FVertexSimple> vertices =
	{
		{Min.x, Min.y, Min.z, color.x, color.y, color.z, color.w},
		{Min.x, Min.y, Max.z, color.x, color.y, color.z, color.w},
		{Max.x, Min.y, Min.z, color.x, color.y, color.z, color.w},
		{Min.x, Max.y, Min.z, color.x, color.y, color.z, color.w},
		{Min.x, Max.y, Max.z, color.x, color.y, color.z, color.w},
		{Max.x, Max.y, Min.z, color.x, color.y, color.z, color.w},
		{Max.x, Min.y, Max.z, color.x, color.y, color.z, color.w},
		{Max.x, Max.y, Max.z, color.x, color.y, color.z, color.w}
	};

	return vertices;
}

TArray<uint32> FBoundingBox::GenerateIndices()
{
	TArray<uint32> indices =
	{
		0, 1,	1, 6,	6, 2,	2, 0,	// 바닥
		3, 4,	4, 7,	7, 5,	5, 3,	// 천장
		0, 3,	1, 4,	2, 5,	6, 7	// 측면
	};

	return indices;
}
