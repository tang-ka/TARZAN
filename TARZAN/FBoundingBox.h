#pragma once
class FVertexSimple;

class FBoundingBox
{
public:
	FVector Min;
	FVector Max;

public:
	FBoundingBox() :
		Min(FVector(FLT_MAX, FLT_MAX, FLT_MAX)),
		Max(FVector(FLT_MIN, FLT_MIN, FLT_MIN)) {}

	FBoundingBox(const FVector& min, const FVector& max)
		: Min(min), Max(max) {}

public:
	void ExpandToInclude(const FVector& point);
	void Merge(const FBoundingBox& other);
	TArray<FVertexSimple> GenerateVertices(FVector4 color = { 1.f, 1.f, 1.f, 1.f });
	TArray<uint32> GenerateIndices();

	FVector GetCenter() const { return (Min + Max) * 0.5f; }
	FVector GetExtent() const { return (Max - Min) * 0.5f; }
	FVector GetSize() const { return Max - Min; }
	bool Contains(const FVector& point) const;
};

inline bool FBoundingBox::Contains(const FVector& point) const
{
	return (point.x >= Min.x && point.x <= Max.x
		&& point.y >= Min.y && point.y <= Max.y
		&& point.z >= Min.z && point.z <= Max.z);
}

