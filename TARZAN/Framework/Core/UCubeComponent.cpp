#include "stdafx.h"
#include "UCubeComponent.h"

UCubeComponent::UCubeComponent() {

	ObjType = EObjectType::Object;
	isBill = false;


	vertices = {
		//À­¸é
	   { 1.f,  1.f,  1.f, 1.f, 1.f, 1.f, 1.f, 0.5f, 0.0f }, // 0     
	   {1.f,  1.f,  -1.f, 1.f, 1.f, 1.f, 1.f, 0.5f, 0.5f },  // 1
	   {-1.f, 1.f,  -1.f, 1.f, 1.f, 1.f, 1.f, 0.f, .5f },  // 2
	   { -1.f, 1.f,  1.f, 1.f, 1.f, 1.f, 1.f, 0.0f, 0.f },  // 3

	   //¾Æ·§¸é
		{ 1.f,  -1.f,  1.f, 1.f, 1.f, 1.f, 1.f,0.5f, 0.0f }, //4
	   {1.f,  -1.f,  -1.f, 1.f, 1.f, 1.f, 1.f, 0.5f, 0.5f},//5
	   {-1.f, -1.f,  -1.f, 1.f, 1.f, 1.f, 1.f, 0.f, .5f }, //6
	   { -1.f, -1.f,  1.f, 1.f, 1.f, 1.f, 1.f, 0.0f, 0.f },//7


	   //¾Õ¸é
	   {1.f,  1.f,  -1.f, 1.f, 1.f, 1.f, 1.f,	1.f, 0.5f },  // 1			 8
	   {1.f,  -1.f,  -1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f },//5			9
		{-1.f, -1.f,  -1.f, 1.f, 1.f, 1.f, 1.f, 0.5f, 1.0f }, //6		10
		{-1.f, 1.f,  -1.f, 1.f, 1.f, 1.f, 1.f, .5f, 0.5f },  // 2		11


		//µÞ¸é

	   { 1.f,  1.f,  1.f, 1.f, 1.f, 1.f, 1.f, 0.5f, 0.5f }, // 0			12
		 { -1.f, 1.f,  1.f, 1.f, 1.f, 1.f, 1.f, 1.f, .5f },  // 3		13
		{ -1.f, -1.f,  1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f },//7			14
		{ 1.f,  -1.f,  1.f, 1.f, 1.f, 1.f, 1.f, 0.5f, 1.f }, //4			15


		//¿À¸¥ÂÊ
		 {1.f,  1.f,  -1.f, 1.f, 1.f, 1.f, 1.f, 0.5f, 0.f },  // 1		16
		{ 1.f,  1.f,  1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 0.f }, // 0			17
		{ 1.f,  -1.f,  1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 0.5f }, //4			18
		{1.f,  -1.f,  -1.f, 1.f, 1.f, 1.f, 1.f, 0.5f, 0.5f },//5			19


		//¿ÞÂÊ
		 { -1.f, 1.f,  1.f, 1.f, 1.f, 1.f, 1.f, 0.f, 0.5f },  // 3		20
		 {-1.f, 1.f,  -1.f, 1.f, 1.f, 1.f, 1.f, 0.5f,0.5f },  // 2		21
		   {-1.f, -1.f,  -1.f, 1.f, 1.f, 1.f, 1.f, 0.5f, 1.f }, //6		22
	   { -1.f, -1.f,  1.f, 1.f, 1.f, 1.f, 1.f, 0.f, 1.f },//7			23


	};

	indices =
	{
		2,3,1,1,3,0,

		5,7,6,5,4,7,

		11,9,10,11,8,9,

		12,13,14,12,14,15,

		16,18,19,16,17,18,

		20,22,23,20,21,22,


	};



	CGraphics* graphics = CRenderer::Instance()->GetGraphics();
	_vertexBuffer = std::make_unique<CVertexBuffer<FVertexSimple>>(graphics->GetDevice());
	_vertexBuffer->Create(vertices);
	_indexBuffer = std::make_unique<CIndexBuffer>(graphics->GetDevice());
	_indexBuffer->Create(indices);

	CreateBoundingBox(EPrimitiveType::CUBE);
}

void UCubeComponent::Update() {
	//RelativeRotation += FVector(1.f, 1.f, 1.f) * Time::GetDeltaTime();
	//RelativeLocation.x = sin(Time::GetElapsedTime());
}

bool UCubeComponent::IntersectsRay(const FVector& rayOrigin, const FVector& rayDir, float& Distance)
{
	FVector min = { -1.f, -1.f, -1.f };
	FVector max = { 1.f,  1.f,  1.f };

	double tMin = -FLT_MAX;
	double tMax = FLT_MAX;
	const double epsilon = FLT_EPSILON;

	if (fabs(rayDir.x) < epsilon) {
		if (rayOrigin.x < min.x || rayOrigin.x > max.x)
			return false;
	}
	else {
		double t1 = (min.x - rayOrigin.x) / rayDir.x;
		double t2 = (max.x - rayOrigin.x) / rayDir.x;
		if (t1 > t2)
			std::swap(t1, t2);
		tMin = (tMin < t1) ? t1 : tMin;
		tMax = (tMax < t2) ? tMax : t2;
	}

	if (fabs(rayDir.y) < epsilon) {
		if (rayOrigin.y < min.y || rayOrigin.y > max.y)
			return false;
	}
	else {
		double t1 = (min.y - rayOrigin.y) / rayDir.y;
		double t2 = (max.y - rayOrigin.y) / rayDir.y;
		if (t1 > t2) std::swap(t1, t2);
		tMin = (tMin < t1) ? t1 : tMin;
		tMax = (tMax < t2) ? tMax : t2;
	}

	if (fabs(rayDir.z) < epsilon) {
		if (rayOrigin.z < min.z || rayOrigin.z > max.z)
			return false;
	}
	else
	{
		double t1 = (min.z - rayOrigin.z) / rayDir.z;
		double t2 = (max.z - rayOrigin.z) / rayDir.z;
		if (t1 > t2) std::swap(t1, t2);
		tMin = (tMin < t1) ? t1 : tMin;
		tMax = (tMax < t2) ? tMax : t2;
	}

	if (tMax >= tMin && tMax >= 0) {
		FVector localIntersection = rayOrigin + rayDir * tMin;
		FVector worldIntersection = GetComponentTransform().TransformCoord(localIntersection);
		Distance = (worldIntersection).MagnitudeSquared();
		return true;
	}

	return false;
}

void UCubeComponent::CreateBoundingBox(EPrimitiveType type)
{
	boundingBox = new FBoundingBox();
	boundingBox->SetBoundaryPointsForPrimitive(type);
	boundingBox->GenerateVerticesByBasicBox();
	boundingBox->GenerateIndices();
	//isShowBoundingBox = true;
}
