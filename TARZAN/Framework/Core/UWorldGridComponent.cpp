#include "stdafx.h"
#include "UWorldGridComponent.h"
//#include "CGraphics.h"  
#include "./Framework/Core/CRenderer.h"
#include <d3d11.h>
#include <cstring>

#include "GuiController.h"
#include "ConfigManager.h"

// 생성자: 멤버 초기화
UWorldGridComponent::UWorldGridComponent()
{
    ObjType = EObjectType::Object;
    vertices = {};
    indices = {};
    CGraphics* graphics = CRenderer::Instance()->GetGraphics();
    _vertexBuffer = std::make_unique<CVertexBuffer<FVertexSimple>>(graphics->GetDevice());
    _indexBuffer = std::make_unique<CIndexBuffer>(graphics->GetDevice());

    lastPosX = std::floor(CRenderer::Instance()->GetMainCamera()->GetRelativeLocation().x);
    lastPosZ = std::floor(CRenderer::Instance()->GetMainCamera()->GetRelativeLocation().z);
}

// 소멸자: 할당한 리소스 해제
UWorldGridComponent::~UWorldGridComponent() {}

void UWorldGridComponent::GenerateGrid(float posX, float posZ, int gridCount, float unitSize)
{
    // 초기화
    vertices.clear();
    indices.clear();

    gridScale = unitSize;
    const FVector4 gridColor = { 1.f, 1.f, 1.f, 0.1f };

    // X축 선
    for (int i = -gridCount; i <= gridCount; i++)
    {
        float x = posX + i * unitSize;
        FVertexSimple v1, v2;
        v1.x = x; v1.z = 0.f; v1.y = posZ - gridCount * unitSize;
        v2.x = x; v2.z = 0.f; v2.y = posZ + gridCount * unitSize;
        v1.r = gridColor.x; v1.g = gridColor.y; v1.b = gridColor.z; v1.a = gridColor.w;
        v2.r = gridColor.x; v2.g = gridColor.y; v2.b = gridColor.z; v2.a = gridColor.w;
        vertices.push_back(v1);
        vertices.push_back(v2);
    }

    // Z축 선
    for (int i = -gridCount; i <= gridCount; i++)
    {
        float z = posZ + i * unitSize;
        FVertexSimple v1, v2;
        v1.x = posX - gridCount * unitSize; v1.z = 0.f; v1.y = z;
        v2.x = posX + gridCount * unitSize; v2.z = 0.f; v2.y = z;
        v1.r = gridColor.x; v1.g = gridColor.y; v1.b = gridColor.z; v1.a = gridColor.w;
        v2.r = gridColor.x; v2.g = gridColor.y; v2.b = gridColor.z; v2.a = gridColor.w;
        vertices.push_back(v1);
        vertices.push_back(v2);
    }

    for (UINT32 i = 0; i < vertices.size(); i++)
    {
        indices.push_back(i);
    }

    // 동적 버퍼를 사용하므로, 이미 버퍼가 생성되어 있다면 Map/Unmap으로 내용을 업데이트
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

    // 현재 gridScale의 배수로 새 원점을 계산합니다.
    float newOriginX = std::floor(camX / gridScale) * gridScale;
    float newOriginZ = std::floor(camZ / gridScale) * gridScale;

    // 만약 새 원점이 기존 원점과 다르다면, 그리드를 재생성합니다.
    if (newOriginX != lastPosX || newOriginZ != lastPosZ)
    {
        // 새 원점 업데이트
        lastPosX = newOriginX;
        lastPosZ = newOriginZ;

        // gridCount는 예시로 10, 필요에 따라 조정
        int gridCount = 10;
        // 새 원점과 gridScale을 사용해 grid를 재생성합니다.
        GenerateGrid(lastPosX, lastPosZ, gridCount, gridScale);

        return;
    }

    // 카메라가 아직 같은 grid cell 내에 있다면, 기존 로직으로 grid 위치를 미세하게 이동시킵니다.
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
    CTextureManager::GetInstance()->BindTextureToShader(ObjType);
    CGraphics* graphics = CRenderer::Instance()->GetGraphics();
    ID3D11DeviceContext* context = graphics->GetDeviceContext();
    //CRenderer::Instance()->SetRasterzierState(D3D11_FILL_SOLID);
    if (!context) return;

    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
    // 선(Line List) 토폴로지 설정
    UPrimitiveComponent::Render();

    graphics->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // DrawIndexed를 호출하여 grid를 렌더링합니다.
    // 이게 지금 문젠가 Start Base Index Location이 잘못된 거 같기도 하고
    //context->DrawIndexed(static_cast<UINT>(indices.size()), 0, 0);
}