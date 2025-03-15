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
    vertices = {};
    indices = {};
    CGraphics* graphics = CRenderer::Instance()->GetGraphics();
    _vertexBuffer = new CVertexBuffer<FVertexSimple>(graphics->GetDevice());
    _indexBuffer = new CIndexBuffer(graphics->GetDevice());

    lastPosX = std::floor(CRenderer::Instance()->GetMainCamera()->GetRelativeLocation().x);
    lastPosZ = std::floor(CRenderer::Instance()->GetMainCamera()->GetRelativeLocation().z);
}

// 소멸자: 할당한 리소스 해제
UWorldGridComponent::~UWorldGridComponent()
{
    delete _vertexBuffer;
    delete _indexBuffer;
}

// GenerateGrid: -gridCount부터 gridCount까지 1단위 간격의 grid 선 정점 및 인덱스 생성
void UWorldGridComponent::GenerateGrid(float posX, float posZ, int gridCount, float unitSize)
{
    // 여기에서 하는게 맞을까
    vertices.clear();
    indices.clear();

    if (_vertexBuffer->Get())
        _vertexBuffer->Get()->Release();
    if (_indexBuffer->Get())
        _indexBuffer->Get()->Release();

    // 색상값
    const FVector4 gridColor = { 0.f, 0.f, 0.f, 1.f };
    gridScale = unitSize;

    // X축(수직선) 그리기: X 좌표가 일정하며 Z가 -gridCount ~ gridCount까지 변화
    for (int i = -gridCount; i <= gridCount; i++)
    {
        // camera의 position을 가져와서 
        float x = posX+i * unitSize;

        FVertexSimple v1, v2;

        // FSimpleVertex 구조를 변경할 필요가 있어보임
        v1.x = x; v1.y = 0.0f; v1.z = posZ -gridCount * unitSize;
        v2.x = x; v2.y = 0.0f; v2.z = posZ+gridCount * unitSize ;

        v1.r = gridColor.x; v1.g = gridColor.y; v1.b = gridColor.z; v1.a = gridColor.w;
        v2.r = gridColor.x; v2.g = gridColor.y; v2.b = gridColor.z; v2.a = gridColor.w;
        vertices.push_back(v1);
        vertices.push_back(v2);
    }

    // Z축(수평선) 그리기: Z 좌표가 일정하며 X가 -gridCount ~ gridCount까지 변화
    // 수정을 했는데 이렇게 하면 카메라 동적이동했을때 문제가 생길 것 같은데 아닌가? 상관 없을라나
    for (int i = -gridCount; i <= gridCount; i++)
    {
        float z = posZ+i * unitSize;
        FVertexSimple v1, v2;
        v1.x = posX -gridCount * unitSize; v1.y = 0.0f; v1.z = z;
        v2.x = posX+gridCount * unitSize;  v2.y = 0.0f; v2.z = z;

        // 기본 색상 할당
        v1.r = gridColor.x; v1.g = gridColor.y; v1.b = gridColor.z; v1.a = gridColor.w;
        v2.r = gridColor.x; v2.g = gridColor.y; v2.b = gridColor.z; v2.a = gridColor.w;
        vertices.push_back(v1);
        vertices.push_back(v2);
    }

    // 인덱스 생성: 각 선은 두 정점으로 구성되므로, 순차적으로 인덱스를 할당
    for (UINT32 i = 0; i < vertices.size(); i++)
    {
        indices.push_back(i);
    }
    _vertexBuffer->Create(vertices);
    _indexBuffer->Create(indices);
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
        //// 기존 grid 데이터 초기화
        //vertices.clear();
        //indices.clear();

        //// 기존 GPU 버퍼 해제
        //if (_vertexBuffer->Get())
        //    _vertexBuffer->Get()->Release();
        //if (_indexBuffer->Get())
        //    _indexBuffer->Get()->Release();

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