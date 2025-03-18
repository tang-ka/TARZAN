#include "stdafx.h"
#include "UTextComponent.h"


// ������: �⺻ �ؽ�Ʈ�� "Default Text"�� ����
UTextComponent::UTextComponent()
   
{   
    ObjType = EObjectType::Text; // �ؽ�Ʈ UI Ÿ�� ����
    CreateBufferForText(L"Welcome to jungle");
    SetVerticesToPipeline();
}


void UTextComponent::CreateBufferForText(std::wstring text)
{
    SetRelativeLocationY(3.f);
    isBill = true;

    float gap = 1.0f / 16.0f;
    size_t baseIndex = 0;
    vertices.clear();  // ���� vertices �ʱ�ȭ
    indices.clear();   // ���� indices �ʱ�ȭ

    float xOffset = text.size() * gap; // �ؽ�Ʈ�� x�� ��ġ�� �����ϱ� ���� ������
    float yOffset = 0.0f; // �ؽ�Ʈ�� y�� ��ġ�� ����

    for (size_t i = 0; i < text.size(); ++i) {
        // �� ������ �ƽ�Ű �ڵ� �� ���ϱ�
        wchar_t character = text[i];
        int charCode = (int)character;  // �ƽ�Ű �ڵ� ��

        // �ش� ������ UV ��ǥ ���
        float u = (charCode % 16) * (1.0f / 16.0f); // u ��ǥ (16x16 �׸���)
        float v = (charCode / 16) * (1.0f / 16.0f); // v ��ǥ (16x16 �׸���)

        // �� ���ڿ� ���� ���ؽ� ��� (�簢���� �׸��� ���� 4�� ���ؽ�)
        vertices.push_back({ xOffset + 1.f,  1.f + yOffset, 0.f, 1.f, 0.f, 0.f, 1.f, u + gap, v }); // ����
        vertices.push_back({ xOffset + 1.f, -1.f + yOffset, 0.f, 0.f, 1.f, 0.f, 1.f, u + gap,v + gap }); // ���ϴ�
        vertices.push_back({ xOffset - 1.f, -1.f + yOffset, 0.f, 0.f, 0.f, 1.f, 1.f, u, v + gap });     // ���ϴ�
        vertices.push_back({ xOffset - 1.f,  1.f + yOffset, 0.f, 1.f, 1.f, 0.f, 1.f, u, v });     // �»��

        // �ε��� ��� (�簢���� �׸��� ���� �ε���)
        indices.push_back(baseIndex + 3);
        indices.push_back(baseIndex + 0);
        indices.push_back(baseIndex + 1);
        indices.push_back(baseIndex + 3);
        indices.push_back(baseIndex + 1);
        indices.push_back(baseIndex + 2);

        // xOffset�� �������Ѽ� �� ���� ������ ����
        xOffset += 1.0f;

        // ���� ������ ���� �ε����� ����
        baseIndex += 4;
    }


}

void UTextComponent::SetVerticesToPipeline()
{

    CGraphics* graphics = CRenderer::Instance()->GetGraphics();
    _vertexBuffer = std::make_unique<CVertexBuffer<FVertexSimple>>(graphics->GetDevice());
    _vertexBuffer->Create(vertices);
    _indexBuffer = std::make_unique<CIndexBuffer>(graphics->GetDevice());
    _indexBuffer->Create(indices);

}

void UTextComponent::UpdateText(std::wstring newText)
{
    // ���� ���� ���� (���ҽ� ����)
    _vertexBuffer.reset();
    _indexBuffer.reset();

    // ���ο� �ؽ�Ʈ�� ���ؽ��� �ε��� �ٽ� ����
    CreateBufferForText(newText);

    // ���ο� ���ؽ��� �ε����� �ٽ� GPU�� ���ε�
    SetVerticesToPipeline();
}