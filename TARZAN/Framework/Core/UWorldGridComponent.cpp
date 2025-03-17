#include "stdafx.h"
#include "UWorldGridComponent.h"
//#include "CGraphics.h"  
#include "./Framework/Core/CRenderer.h"
#include <d3d11.h>
#include <cstring>

#include "GuiController.h"
#include "ConfigManager.h"

// ������: ��� �ʱ�ȭ
UWorldGridComponent::UWorldGridComponent()
{
    vertices = {};
    indices = {};
    CGraphics* graphics = CRenderer::Instance()->GetGraphics();
    _vertexBuffer = new CVertexBuffer<FVertexSimple>(graphics->GetDevice());
    _indexBuffer = new CIndexBuffer(graphics->GetDevice());

    lastPosX = std::floor(CRenderer::Instance()->GetMainCamera()->GetRelativeLocation().x);
    lastPosZ = std::floor(CRenderer::Instance()->GetMainCamera()->GetRelativeLocation().z);
}

// �Ҹ���: �Ҵ��� ���ҽ� ����
UWorldGridComponent::~UWorldGridComponent()
{
    delete _vertexBuffer;
    delete _indexBuffer;
}

// GenerateGrid: -gridCount���� gridCount���� 1���� ������ grid �� ���� �� �ε��� ����
//void UWorldGridComponent::GenerateGrid(float posX, float posZ, int gridCount, float unitSize)
//{
//
//    if (_vertexBuffer->Get()!=nullptr)
//    {
//        vertices.clear();
//        _vertexBuffer->Get()->Release();
//
//    }
//    if (_indexBuffer->Get()!=nullptr)
//    {
//        indices.clear();
//        _indexBuffer->Get()->Release();
//    }
//
//    // ����
//    const FVector4 gridColor = { 0.f, 0.f, 0.f, 1.f };
//    gridScale = unitSize;
//
//    // X��(������) �׸���: X ��ǥ�� �����ϸ� Z�� -gridCount ~ gridCount���� ��ȭ
//    for (int i = -gridCount; i <= gridCount; i++)
//    {
//        // camera�� position�� �����ͼ� 
//        float x = posX+i * unitSize;
//
//        FVertexSimple v1, v2;
//
//        // FSimpleVertex ������ ������ �ʿ䰡 �־��
//        v1.x = x; v1.y = 0.0f; v1.z = posZ -gridCount * unitSize;
//        v2.x = x; v2.y = 0.0f; v2.z = posZ+gridCount * unitSize ;
//
//        v1.r = gridColor.x; v1.g = gridColor.y; v1.b = gridColor.z; v1.a = gridColor.w;
//        v2.r = gridColor.x; v2.g = gridColor.y; v2.b = gridColor.z; v2.a = gridColor.w;
//        vertices.push_back(v1);
//        vertices.push_back(v2);
//    }
//
//    // Z��(����) �׸���: Z ��ǥ�� �����ϸ� X�� -gridCount ~ gridCount���� ��ȭ
//    // ������ �ߴµ� �̷��� �ϸ� ī�޶� �����̵������� ������ ���� �� ������ �ƴѰ�? ��� ������
//    for (int i = -gridCount; i <= gridCount; i++)
//    {
//        float z = posZ+i * unitSize;
//        FVertexSimple v1, v2;
//        v1.x = posX -gridCount * unitSize; v1.y = 0.0f; v1.z = z;
//        v2.x = posX+gridCount * unitSize;  v2.y = 0.0f; v2.z = z;
//
//        // �⺻ ���� �Ҵ�
//        v1.r = gridColor.x; v1.g = gridColor.y; v1.b = gridColor.z; v1.a = gridColor.w;
//        v2.r = gridColor.x; v2.g = gridColor.y; v2.b = gridColor.z; v2.a = gridColor.w;
//        vertices.push_back(v1);
//        vertices.push_back(v2);
//    }
//
//    // �ε��� ����: �� ���� �� �������� �����ǹǷ�, ���������� �ε����� �Ҵ�
//    for (UINT32 i = 0; i < vertices.size(); i++)
//    {
//        indices.push_back(i);
//    }
//    _vertexBuffer->Create(vertices);
//    _indexBuffer->Create(indices);
//}

void UWorldGridComponent::GenerateGrid(float posX, float posZ, int gridCount, float unitSize)
{
    // �ʱ�ȭ
    vertices.clear();
    indices.clear();

    gridScale = unitSize;
    const FVector4 gridColor = { 0.f, 0.f, 0.f, 1.f };

    // X�� ��
    for (int i = -gridCount; i <= gridCount; i++)
    {
        float x = posX + i * unitSize;
        FVertexSimple v1, v2;
        v1.x = x; v1.y = 0.f; v1.z = posZ - gridCount * unitSize;
        v2.x = x; v2.y = 0.f; v2.z = posZ + gridCount * unitSize;
        v1.r = gridColor.x; v1.g = gridColor.y; v1.b = gridColor.z; v1.a = gridColor.w;
        v2.r = gridColor.x; v2.g = gridColor.y; v2.b = gridColor.z; v2.a = gridColor.w;
        vertices.push_back(v1);
        vertices.push_back(v2);
    }

    // Z�� ��
    for (int i = -gridCount; i <= gridCount; i++)
    {
        float z = posZ + i * unitSize;
        FVertexSimple v1, v2;
        v1.x = posX - gridCount * unitSize; v1.y = 0.f; v1.z = z;
        v2.x = posX + gridCount * unitSize; v2.y = 0.f; v2.z = z;
        v1.r = gridColor.x; v1.g = gridColor.y; v1.b = gridColor.z; v1.a = gridColor.w;
        v2.r = gridColor.x; v2.g = gridColor.y; v2.b = gridColor.z; v2.a = gridColor.w;
        vertices.push_back(v1);
        vertices.push_back(v2);
    }

    for (UINT32 i = 0; i < vertices.size(); i++)
    {
        indices.push_back(i);
    }

    // ���� ���۸� ����ϹǷ�, �̹� ���۰� �����Ǿ� �ִٸ� Map/Unmap���� ������ ������Ʈ
    CGraphics* graphics = CRenderer::Instance()->GetGraphics();
    ID3D11DeviceContext* context = graphics->GetDeviceContext();

    if (_vertexBuffer->Get() != nullptr)
    {
        D3D11_MAPPED_SUBRESOURCE mappedResource = {};
        HRESULT hr = context->Map(_vertexBuffer->Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
        if (SUCCEEDED(hr))
        {
            memcpy(mappedResource.pData, vertices.data(), vertices.size() * sizeof(FVertexSimple));
            context->Unmap(_vertexBuffer->Get(), 0);
        }
    }
    else
    {
        _vertexBuffer->Create(vertices);
    }

    if (_indexBuffer->Get() != nullptr)
    {
        D3D11_MAPPED_SUBRESOURCE mappedResource = {};
        HRESULT hr = context->Map(_indexBuffer->Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
        if (SUCCEEDED(hr))
        {
            memcpy(mappedResource.pData, indices.data(), indices.size() * sizeof(uint32));
            context->Unmap(_indexBuffer->Get(), 0);
        }
    }
    else
    {
        _indexBuffer->Create(indices);
    }
}


void UWorldGridComponent::UpdateGrid()
{
    UCameraComponent* mainCam = CRenderer::Instance()->GetMainCamera();
    float camX = mainCam->GetRelativeLocation().x;
    float camZ = mainCam->GetRelativeLocation().z;

    // ���� gridScale�� ����� �� ������ ����մϴ�.
    float newOriginX = std::floor(camX / gridScale) * gridScale;
    float newOriginZ = std::floor(camZ / gridScale) * gridScale;

    // ���� �� ������ ���� ������ �ٸ��ٸ�, �׸��带 ������մϴ�.
    if (newOriginX != lastPosX || newOriginZ != lastPosZ)
    {
        // �� ���� ������Ʈ
        lastPosX = newOriginX;
        lastPosZ = newOriginZ;

        // gridCount�� ���÷� 10, �ʿ信 ���� ����
        int gridCount = 10;
        // �� ������ gridScale�� ����� grid�� ������մϴ�.
        GenerateGrid(lastPosX, lastPosZ, gridCount, gridScale);

        return;
    }

    // ī�޶� ���� ���� grid cell ���� �ִٸ�, ���� �������� grid ��ġ�� �̼��ϰ� �̵���ŵ�ϴ�.
    UCameraComponent* MainCamera = CRenderer::Instance()->GetMainCamera();
    float cameraX = std::floor(MainCamera->GetRelativeLocation().x);
    float cameraZ = std::floor(MainCamera->GetRelativeLocation().z);

    FVector offset(0.f, 0.f, 0.f);
    if (cameraX >= lastPosX + gridScale)
    {
        offset.x = gridScale;
    }
    else if (cameraX <= lastPosX - gridScale)
    {
        offset.x = -gridScale;
    }

    if (cameraZ >= lastPosZ + gridScale)
    {
        offset.z = gridScale;
    }
    else if (cameraZ <= lastPosZ - gridScale)
    {
        offset.z = -gridScale;
    }

    if (offset != FVector())
    {
        SetRelativeLocation(GetRelativeLocation() + offset);
        lastPosX = cameraX;
        lastPosZ = cameraZ;
    }
}

void UWorldGridComponent::Render()
{
    CGraphics* graphics = CRenderer::Instance()->GetGraphics();
    ID3D11DeviceContext* context = graphics->GetDeviceContext();
    //CRenderer::Instance()->SetRasterzierState(D3D11_FILL_SOLID);
    if (!context) return;

    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
    // ��(Line List) �������� ����
    UPrimitiveComponent::Render();

    graphics->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // DrawIndexed�� ȣ���Ͽ� grid�� �������մϴ�.
    // �̰� ���� ������ Start Base Index Location�� �߸��� �� ���⵵ �ϰ�
    //context->DrawIndexed(static_cast<UINT>(indices.size()), 0, 0);
}