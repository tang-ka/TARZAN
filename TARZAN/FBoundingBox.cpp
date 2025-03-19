#include "stdafx.h"
#include "FBoundingBox.h"
//#include "./Framework/Core/CRenderer.h"

void FBoundingBox::ExpandToInclude(const FVector& point)
{

}

void FBoundingBox::Merge(const FBoundingBox& other)
{

}

void FBoundingBox::SetBoundaryPointsForPrimitive(EPrimitiveType type)
{
	switch (type)
	{
	case CUBE:
		boundaryPoints =
		{
			{ -1.f, -1.f, -1.f, 1.f},
			{ -1.f, -1.f, 1.f, 1.f},
			{ 1.f, -1.f, -1.f, 1.f},
			{ -1.f, 1.f, -1.f, 1.f},
			{ -1.f, 1.f, 1.f, 1.f},
			{ 1.f, 1.f, -1.f, 1.f},
			{ 1.f, -1.f, 1.f, 1.f},
			{ 1.f, 1.f, 1.f, 1.f},
		};
		break;
	case SPHERE:
		/*boundaryPoints =
		{
			{ 0.f, 0.f, -1.f, 1.f},
			{ 0.f, 0.f, 1.f, 1.f},
			{ 0.f, -1.f, 0.f, 1.f},
			{ 0.f, 1.f, 0.f, 1.f},
			{ -1.f, 0.f, 0.f, 1.f},
			{ 1.f, 0.f, 0.f, 1.f},
		};*/
		break;
	case PLANE:
		boundaryPoints =
		{
			{-1.0f, 0.0f, -1.0f, 1.f},
			{ 1.0f, 0.0f, -1.0f, 1.f},
			{ 1.0f, 0.0f,  1.0f, 1.f},
			{-1.0f, 0.0f,  1.0f, 1.f}
		};
		break;
	case SPOTLIGHT:
		// SpotLight는 Plane과 같은 bounding box를 가지도록 함
		boundaryPoints =
		{
			{-1.0f, 0.0f, -1.0f, 1.f},
			{ 1.0f, 0.0f, -1.0f, 1.f},
			{ 1.0f, 0.0f,  1.0f, 1.f},
			{-1.0f, 0.0f,  1.0f, 1.f}
		};
		break;
	default:
		boundaryPoints = {};
		break;
	}

	UpdateVerticesByBP();
}

void FBoundingBox::UpdateVerticesByBP(FMatrix modelToWorldTransformMatrix)
{
	TArray<FVector4> updatePoint = {};

	for (size_t i = 0; i < boundaryPoints.size(); i++)
	{
		updatePoint.push_back(boundaryPoints[i] * modelToWorldTransformMatrix);
	}

	minPoint = { FLT_MAX, FLT_MAX, FLT_MAX };
	maxPoint = { -FLT_MAX, -FLT_MAX, -FLT_MAX };

	for (int i = 0; i < updatePoint.size(); i++)
	{
		minPoint.x = min(minPoint.x, updatePoint[i].x);
		minPoint.y = min(minPoint.y, updatePoint[i].y);
		minPoint.z = min(minPoint.z, updatePoint[i].z);

		maxPoint.x = max(maxPoint.x, updatePoint[i].x);
		maxPoint.y = max(maxPoint.y, updatePoint[i].y);
		maxPoint.z = max(maxPoint.z, updatePoint[i].z);
	}

	GenerateVertices(minPoint, maxPoint, color);
}

void FBoundingBox::GenerateVertices()
{
	vertices =
	{
		{minPoint.x, minPoint.y, minPoint.z, color.x, color.y, color.z, color.w},
		{minPoint.x, minPoint.y, maxPoint.z, color.x, color.y, color.z, color.w},
		{maxPoint.x, minPoint.y, minPoint.z, color.x, color.y, color.z, color.w},
		{minPoint.x, maxPoint.y, minPoint.z, color.x, color.y, color.z, color.w},
		{minPoint.x, maxPoint.y, maxPoint.z, color.x, color.y, color.z, color.w},
		{maxPoint.x, maxPoint.y, minPoint.z, color.x, color.y, color.z, color.w},
		{maxPoint.x, minPoint.y, maxPoint.z, color.x, color.y, color.z, color.w},
		{maxPoint.x, maxPoint.y, maxPoint.z, color.x, color.y, color.z, color.w}
	};
}

void FBoundingBox::GenerateVertices(FVector min, FVector max, FVector4 color)
{
	this->minPoint = min;
	this->maxPoint = max;
	this->color = color;

	GenerateVertices();
}

void FBoundingBox::GenerateVerticesByBasicBox()
{
	GenerateVertices(basicMinPoint, basicMaxPoint, color);
}

void FBoundingBox::GenerateIndices()
{
	indices =
	{
		0, 1,	1, 6,	6, 2,	2, 0,	// 바닥
		3, 4,	4, 7,	7, 5,	5, 3,	// 천장
		0, 3,	1, 4,	2, 5,	6, 7	// 측면
	};
}

void FBoundingBox::BasicBoxInitialize()
{
	basicMinPoint = { -1.f, -1.f, -1.f };
	basicMaxPoint = { 1.f, 1.f, 1.f };

	basicBoxVertices =
	{
		{basicMinPoint.x, basicMinPoint.y, basicMinPoint.z, color.x, color.y, color.z, color.w},
		{basicMinPoint.x, basicMinPoint.y, basicMaxPoint.z, color.x, color.y, color.z, color.w},
		{basicMaxPoint.x, basicMinPoint.y, basicMinPoint.z, color.x, color.y, color.z, color.w},
		{basicMinPoint.x, basicMaxPoint.y, basicMinPoint.z, color.x, color.y, color.z, color.w},
		{basicMinPoint.x, basicMaxPoint.y, basicMaxPoint.z, color.x, color.y, color.z, color.w},
		{basicMaxPoint.x, basicMaxPoint.y, basicMinPoint.z, color.x, color.y, color.z, color.w},
		{basicMaxPoint.x, basicMinPoint.y, basicMaxPoint.z, color.x, color.y, color.z, color.w},
		{basicMaxPoint.x, basicMaxPoint.y, basicMaxPoint.z, color.x, color.y, color.z, color.w}
	};
}


