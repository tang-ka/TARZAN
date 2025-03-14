#include "stdafx.h"
#include "UWorldGridComponent.h"
//#include "CGraphics.h"  
#include "./Framework/Core/CRenderer.h"
#include <d3d11.h>
#include <cstring>

// ������: ��� �ʱ�ȭ
UWorldGridComponent::UWorldGridComponent()
{
    vertices = {};
    indices = {};
    CGraphics* graphics = CRenderer::Instance()->GetGraphics();
    _vertexBuffer = new CVertexBuffer<FVertexSimple>(graphics->GetDevice());
    _indexBuffer = new CIndexBuffer(graphics->GetDevice());
    
}

// �Ҹ���: �Ҵ��� ���ҽ� ����
UWorldGridComponent::~UWorldGridComponent()
{
    delete _vertexBuffer;
    delete _indexBuffer;
}

// GenerateGrid: -gridCount���� gridCount���� 1���� ������ grid �� ���� �� �ε��� ����
void UWorldGridComponent::GenerateGrid(int gridCount, float unitSize)
{
    // ����
    const FVector4 gridColor = { 0.f, 0.f, 0.f, 1.f };

    // X��(������) �׸���: X ��ǥ�� �����ϸ� Z�� -gridCount ~ gridCount���� ��ȭ
    for (int i = -gridCount; i <= gridCount; i++)
    {
        // camera�� position�� �����ͼ� 
        float x = i * unitSize;

        FVertexSimple v1, v2;

        // FSimpleVertex ������ ������ �ʿ䰡 �־��
        v1.x = x; v1.y = 0.0f; v1.z = -gridCount * unitSize;
        v2.x = x; v2.y = 0.0f; v2.z = i ? gridCount * unitSize : 0;

        v1.r = gridColor.x; v1.g = gridColor.y; v1.b = gridColor.z; v1.a = gridColor.w;
        v2.r = gridColor.x; v2.g = gridColor.y; v2.b = gridColor.z; v2.a = gridColor.w;
        vertices.push_back(v1);
        vertices.push_back(v2);
    }

    // Z��(����) �׸���: Z ��ǥ�� �����ϸ� X�� -gridCount ~ gridCount���� ��ȭ
    // ������ �ߴµ� �̷��� �ϸ� ī�޶� �����̵������� ������ ���� �� ������ �ƴѰ�? ��� ������
    for (int i = -gridCount; i <= gridCount; i++)
    {
        float z = i * unitSize;
        FVertexSimple v1, v2;
        v1.x = -gridCount * unitSize; v1.y = 0.0f; v1.z = z;
        v2.x = i ? gridCount * unitSize : 0;  v2.y = 0.0f; v2.z = z;

        // �⺻ ���� �Ҵ�
        v1.r = gridColor.x; v1.g = gridColor.y; v1.b = gridColor.z; v1.a = gridColor.w;
        v2.r = gridColor.x; v2.g = gridColor.y; v2.b = gridColor.z; v2.a = gridColor.w;
        vertices.push_back(v1);
        vertices.push_back(v2);
    }

    // �ε��� ����: �� ���� �� �������� �����ǹǷ�, ���������� �ε����� �Ҵ�
    for (UINT32 i = 0; i < vertices.size(); i++)
    {
        indices.push_back(i);
    }

    _vertexBuffer->Create(vertices);
    _indexBuffer->Create(indices);
}

void UWorldGridComponent::UpdateGrid()
{
    UCameraComponent* MainCamera = CRenderer::Instance()->GetMainCamera();

    float PosX = MainCamera->GetRelativeLocation().x;
    float PosZ = MainCamera->GetRelativeLocation().z;
}

void UWorldGridComponent::Render()
{
    CGraphics* graphics = CRenderer::Instance()->GetGraphics();
    ID3D11DeviceContext* context = graphics->GetDeviceContext();
    CRenderer::Instance()->SetRasterzierState();
    if (!context) return;

    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
    // ��(Line List) �������� ����
    UPrimitiveComponent::Render();

    //graphics->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // DrawIndexed�� ȣ���Ͽ� grid�� �������մϴ�.
    // �̰� ���� ������ Start Base Index Location�� �߸��� �� ���⵵ �ϰ�
    //context->DrawIndexed(static_cast<UINT>(indices.size()), 0, 0);
}