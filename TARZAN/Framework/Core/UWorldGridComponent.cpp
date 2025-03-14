#include "stdafx.h"
#include "UWorldGridComponent.h"
//#include "CGraphics.h"  
#include "./Framework/Core/CRenderer.h"
#include <d3d11.h>
#include <cstring>

// 생성자: 멤버 초기화
UWorldGridComponent::UWorldGridComponent()
{
    vertices = {};
    indices = {};
    CGraphics* graphics = CRenderer::Instance()->GetGraphics();
    _vertexBuffer = new CVertexBuffer<FVertexSimple>(graphics->GetDevice());
    _indexBuffer = new CIndexBuffer(graphics->GetDevice());
}

// 소멸자: 할당한 리소스 해제
UWorldGridComponent::~UWorldGridComponent()
{
    delete _vertexBuffer;
    delete _indexBuffer;
}

// GenerateGrid: -gridCount부터 gridCount까지 1단위 간격의 grid 선 정점 및 인덱스 생성
void UWorldGridComponent::GenerateGrid(int gridCount, float unitSize)
{
    // 색상값
    const FVector4 gridColor = { 0.f, 0.f, 0.f, 1.f };

    // X축(수직선) 그리기: X 좌표가 일정하며 Z가 -gridCount ~ gridCount까지 변화
    for (int i = -gridCount; i <= gridCount; i++)
    {
        float x = i * unitSize;
        FVertexSimple v1, v2;
        v1.x = x; v1.y = 0.0f; v1.z = -gridCount * unitSize;
        v2.x = x; v2.y = 0.0f; v2.z = gridCount * unitSize;

        v1.r = gridColor.x; v1.g = gridColor.y; v1.b = gridColor.z; v1.a = gridColor.w;
        v2.r = gridColor.x; v2.g = gridColor.y; v2.b = gridColor.z; v2.a = gridColor.w;
        vertices.push_back(v1);
        vertices.push_back(v2);
    }

    // Z축(수평선) 그리기: Z 좌표가 일정하며 X가 -gridCount ~ gridCount까지 변화
    for (int i = -gridCount; i <= gridCount; i++)
    {
        float z = i * unitSize;
        FVertexSimple v1, v2;
        v1.x = -gridCount * unitSize; v1.y = 0.0f; v1.z = z;
        v2.x = gridCount * unitSize;  v2.y = 0.0f; v2.z = z;
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

void UWorldGridComponent::Render()
{
    CGraphics* graphics = CRenderer::Instance()->GetGraphics();
    ID3D11DeviceContext* context = graphics->GetDeviceContext();
    if (!context) return;

    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
    // 선(Line List) 토폴로지 설정
    UPrimitiveComponent::Render();

    //graphics->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // DrawIndexed를 호출하여 grid를 렌더링합니다.
    // 이게 지금 문젠가 Start Base Index Location이 잘못된 거 같기도 하고
    //context->DrawIndexed(static_cast<UINT>(indices.size()), 0, 0);
}