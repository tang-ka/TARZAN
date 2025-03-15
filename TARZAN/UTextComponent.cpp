#include "stdafx.h"
#include "UTextComponent.h"


// ������: �⺻ �ؽ�Ʈ�� "Default Text"�� ����
UTextComponent::UTextComponent()
   
{
    ObjType = EObjectType::Text; // �ؽ�Ʈ UI Ÿ�� ����
    
    Text = L"Hello";

	float u = 2.0f/16.0f;
	float v = 5.0f/16.0f;

	float gap = 1.0f / 16.0f;

    vertices = {
        {  1.f,  1.f, 0.f, 1.f, 0.f, 0.f, 1.f, u + gap, v - gap }, // ����
        {  1.f, -1.f, 0.f, 0.f, 1.f, 0.f, 1.f, u + gap, v },       // ���ϴ�
        { -1.f, -1.f, 0.f, 0.f, 0.f, 1.f, 1.f, u, v },             // ���ϴ�
        { -1.f,  1.f, 0.f, 1.f, 1.f, 0.f, 1.f, u, v - gap }        // �»��
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
