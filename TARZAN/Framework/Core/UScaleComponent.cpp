#include "stdafx.h"
#include "UScaleComponent.h"

UScaleComponent::UScaleComponent(EPrimitiveColor Color) : color(Color)
{
	std::wstring filepath = L"Assets/ScaleArrow.obj";
	std::ifstream objFile(filepath.c_str());
	FVector color = AxisColors[Color];

	ObjType = EObjectType::Object;

	if (objFile)
	{

		std::string line;
		std::vector<FVector> positions;

		while (std::getline(objFile, line))
		{
			std::istringstream lineStream(line);
			std::string type;
			lineStream >> type;

			if (type == "v") // Vertex position
			{
				FVector vertex;
				lineStream >> vertex.x >> vertex.y >> vertex.z;
				FVertexSimple vertexSimple{ vertex.x, vertex.y, vertex.z, color.x,color.y,color.z, 1 };
				vertices.push_back(vertexSimple);
				positions.push_back(vertex);
			}
			else if (type == "f") // Face
			{
				std::vector<uint32_t> faceIndices;
				uint32_t index;

				while (lineStream >> index)
				{
					faceIndices.push_back(index - 1);
				}

				for (size_t i = 1; i + 1 < faceIndices.size(); ++i)
				{
					indices.push_back(faceIndices[0]);    // ù ��° ����
					indices.push_back(faceIndices[i]);    // �� ��° ����
					indices.push_back(faceIndices[i + 1]); // �� ��° ����
				}
			}

		}
	}
	CGraphics* graphics = CRenderer::Instance()->GetGraphics();
	_vertexBuffer = std::make_unique<CVertexBuffer<FVertexSimple>>(graphics->GetDevice());
	_vertexBuffer->Create(vertices);
	_indexBuffer = std::make_unique<CIndexBuffer>(graphics->GetDevice());
	_indexBuffer->Create(indices);

	DepthStencilState = new CDepthStencilState(graphics->GetDevice());
	DepthStencilState->SetDepthFlags(FALSE, D3D11_DEPTH_WRITE_MASK_ZERO, D3D11_COMPARISON_GREATER);
	DepthStencilState->SetStencilFlags(FALSE, 0xFF, 0x00);
	DepthStencilState->SetFrontFaceFlags(D3D11_COMPARISON_EQUAL, D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP);
	DepthStencilState->Create();

	ArrowBlendState = new CBlendState(graphics->GetDevice());
	ArrowBlendState->SetDescBlend(D3D11_BLEND_INV_SRC_ALPHA, D3D11_BLEND_ZERO);
	ArrowBlendState->SetSrcBlend(D3D11_BLEND_SRC_ALPHA, D3D11_BLEND_ONE);
	ArrowBlendState->SetOp(D3D11_BLEND_OP_ADD, D3D11_BLEND_OP_ADD);
	ArrowBlendState->Create();

}

UScaleComponent::~UScaleComponent()
{
}