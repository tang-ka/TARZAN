#include "stdafx.h"
#include "UTextComponent.h"


// 생성자: 기본 텍스트는 "Default Text"로 설정
UTextComponent::UTextComponent()
   
{
    ObjType = EObjectType::Text; // 텍스트 UI 타입 설정
    
    Text = L"Hello";

	float u = 2.0f/16.0f;
	float v = 5.0f/16.0f;

	float gap = 1.0f / 16.0f;

    vertices = {
        {  1.f,  1.f, 0.f, 1.f, 0.f, 0.f, 1.f, u + gap, v - gap }, // 우상단
        {  1.f, -1.f, 0.f, 0.f, 1.f, 0.f, 1.f, u + gap, v },       // 우하단
        { -1.f, -1.f, 0.f, 0.f, 0.f, 1.f, 1.f, u, v },             // 좌하단
        { -1.f,  1.f, 0.f, 1.f, 1.f, 0.f, 1.f, u, v - gap }        // 좌상단
    };




	indices =
	{
		3,1,2,
		3,0,1
	};


    CGraphics* graphics = CRenderer::Instance()->GetGraphics();
    _vertexBuffer = new CVertexBuffer<FVertexSimple>(graphics->GetDevice());
    _vertexBuffer->Create(vertices);
    _indexBuffer = new CIndexBuffer(graphics->GetDevice());
    _indexBuffer->Create(indices);

}
