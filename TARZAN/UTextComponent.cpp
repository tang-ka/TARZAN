#include "stdafx.h"
#include "UTextComponent.h"


// 생성자: 기본 텍스트는 "Default Text"로 설정
UTextComponent::UTextComponent()
   
{   
    ObjType = EObjectType::Text; // 텍스트 UI 타입 설정
    
    SetRelativeLocationZ(10.0f);
    SetRelativeLocationX(-2.f);

    Text = L"Jungle";

    isBill = true;

    float gap = 1.0f / 16.0f;
    size_t baseIndex = 0;
    vertices.clear();  // 기존 vertices 초기화
    indices.clear();   // 기존 indices 초기화

    float xOffset = Text.size() * gap; // 텍스트의 x축 위치를 조정하기 위한 오프셋
    float yOffset = 0.0f; // 텍스트의 y축 위치를 고정

    for (size_t i = 0; i < Text.size(); ++i) {
        // 각 문자의 아스키 코드 값 구하기
        wchar_t character = Text[i];
        int charCode = (int)character;  // 아스키 코드 값

        // 해당 문자의 UV 좌표 계산
        float u = (charCode % 16) * (1.0f / 16.0f); // u 좌표 (16x16 그리드)
        float v = (charCode / 16) * (1.0f / 16.0f); // v 좌표 (16x16 그리드)

        // 각 문자에 대한 버텍스 계산 (사각형을 그리기 위한 4개 버텍스)
        vertices.push_back({ xOffset + 1.f,  1.f + yOffset, 0.f, 1.f, 0.f, 0.f, 1.f, u + gap, v }); // 우상단
        vertices.push_back({ xOffset + 1.f, -1.f + yOffset, 0.f, 0.f, 1.f, 0.f, 1.f, u + gap,v + gap }); // 우하단
        vertices.push_back({ xOffset - 1.f, -1.f + yOffset, 0.f, 0.f, 0.f, 1.f, 1.f, u, v + gap });     // 좌하단
        vertices.push_back({ xOffset - 1.f,  1.f + yOffset, 0.f, 1.f, 1.f, 0.f, 1.f, u, v });     // 좌상단

        // 인덱스 계산 (사각형을 그리기 위한 인덱스)
        indices.push_back(baseIndex + 3);
        indices.push_back(baseIndex + 0);
        indices.push_back(baseIndex + 1);
        indices.push_back(baseIndex + 3);
        indices.push_back(baseIndex + 1);
        indices.push_back(baseIndex + 2);

        // xOffset을 증가시켜서 각 문자 간격을 조정
        xOffset += 1.0f;

        // 다음 문자의 시작 인덱스를 갱신
        baseIndex += 4;
    }

    CGraphics* graphics = CRenderer::Instance()->GetGraphics();
    _vertexBuffer = new CVertexBuffer<FVertexSimple>(graphics->GetDevice());
    _vertexBuffer->Create(vertices);
    _indexBuffer = new CIndexBuffer(graphics->GetDevice());
    _indexBuffer->Create(indices);

}
