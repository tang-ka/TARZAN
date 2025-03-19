//#include "UCoordArrowComponent.h"
//
//
//UCoordArrowComponent::UCoordArrowComponent()
//{
//	static const vertices =
//	{
//		{0,0,0,1,0,0,1},
//		{1,0,0,1,0,0,1},
//		{0,1,0,0,1,0,1},
//		{0,1,0,0,1,0,1},
//		{0,0,1,0,0,1,1},
//		{0,0,1,0,0,1,1},
//	}
//}
//
//UCoordArrowComponent::~UCoordArrowComponent()
//{
//}
//
//void UCoordArrowComponent::Render()
//{
//
//}
//
//void UCoordArrowComponent::Update()
//{
//}
//

#include "stdafx.h"
#include "UCoordArrowComponent.h"
#include "./Framework/Core/CRenderer.h"

UCoordArrowComponent::UCoordArrowComponent()
{


	vertices =
	{
		{ 0.f, 0.f, 0.f, 5.f, 0.f, 0.f, 5.f},
		{ 5.f, 0.f, 0.f, 5.f, 0.f, 0.f, 5.f},
		{ 0.f, 0.f, 0.f, 0.f, 5.f, 0.f, 5.f},
		{ 0.f, 5.f, 0.f, 0.f, 5.f, 0.f, 5.f},
		{ 0.f, 0.f, 0.f, 0.f, 0.f, 5.f, 5.f},
		{ 0.f, 0.f, 5.f, 0.f, 0.f, 5.f, 5.f},
	};

	indices = { 0,1,2,3,4,5 };

	CGraphics* graphics = CRenderer::Instance()->GetGraphics();
	_vertexBuffer = std::make_unique<CVertexBuffer<FVertexSimple>>(graphics->GetDevice());
	_vertexBuffer->Create(vertices);
	_indexBuffer = std::make_unique<CIndexBuffer>(graphics->GetDevice());
	_indexBuffer->Create(indices);

	// 100 conflicts
	DepthStencilState = std::make_unique<CDepthStencilState>(graphics->GetDevice());
	DepthStencilState->SetDepthFlags(FALSE, D3D11_DEPTH_WRITE_MASK_ZERO, D3D11_COMPARISON_GREATER);
	DepthStencilState->SetStencilFlags(FALSE, 0xFF, 0x00);
	DepthStencilState->SetFrontFaceFlags(D3D11_COMPARISON_EQUAL, D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP);
	DepthStencilState->Create();

	CoordBlendState = std::make_unique<CBlendState>(graphics->GetDevice());
	CoordBlendState->SetDescBlend(D3D11_BLEND_INV_SRC_ALPHA, D3D11_BLEND_ZERO);
	CoordBlendState->SetSrcBlend(D3D11_BLEND_SRC_ALPHA, D3D11_BLEND_ONE);
	CoordBlendState->SetOp(D3D11_BLEND_OP_ADD, D3D11_BLEND_OP_ADD);
	CoordBlendState->Create();
}

UCoordArrowComponent::~UCoordArrowComponent(){}

void UCoordArrowComponent::Update()
{
}

void UCoordArrowComponent::Set(FVector origin, FVector direction)
{
	if (OriginWhenSpawned == FVector(0, 0, 0) && DirectionWhenSpawned == FVector(0, 0, 0))
	{
		OriginWhenSpawned = origin;
		DirectionWhenSpawned = direction;
	}
	else
	{
		// 여기 오면 안됨
	}
}

FVector UCoordArrowComponent::GetOrigin()
{
	return OriginWhenSpawned;
}

FVector UCoordArrowComponent::GetDirection()
{
	return DirectionWhenSpawned;
}

void UCoordArrowComponent::Render()
{

	CGraphics* graphics = CRenderer::Instance()->GetGraphics();
	graphics->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	CTextureManager::GetInstance()->BindTextureToShader(EObjectType::Object);
	UPrimitiveComponent::Render();
	//CRenderer::Instance()->SetRasterzierState();
	//ID3D11Buffer* vertexBuffer = _vertexBuffer->Get();
	//uint32 stride = _vertexBuffer->GetStride();
	//uint32 offset = _vertexBuffer->GetOffset();
	//graphics->GetDeviceContext()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	//graphics->GetDeviceContext()->IASetIndexBuffer(_indexBuffer->Get(), DXGI_FORMAT_R32_UINT, 0);
	//FMatrix m = GetComponentTransform();
	//CRenderer::Instance()->SetConstantBuffer(m);
	//graphics->GetDeviceContext()->DrawIndexed(indices.size(), 0, 0);
	graphics->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}
