#pragma once

class VertexProvider
{

};

enum class VertexType : uint8_t
{
	EPT_None,
	EPT_Triangle,
	EPT_Cube,
	EPT_Sphere,
	EPT_Line,
	EPT_Cylinder,
	EPT_Cone,
	EPT_Max,
};

class PrimitiveVertexProvider : public VertexProvider
{

};

class GizmoVertexProvider : public VertexProvider
{

};

