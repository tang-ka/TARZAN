#include "stdafx.h"
#include "UTextComponent.h"


// 생성자: 기본 텍스트는 "Default Text"로 설정
UTextComponent::UTextComponent()
   
{   
    ObjType = EObjectType::Text; // 텍스트 UI 타입 설정
    
   
    CreateBufferForText(L"Deafult");
    SetVerticesToPipeline();
}

void UTextComponent::CreateBufferForText(std::wstring text)
{
    SetRelativeLocationY(3.f);
    //isBill = true;

    float gap = 1.0f / 16.0f;
    size_t baseIndex = 0;
    vertices.clear();  // 기존 vertices 초기화
    indices.clear();   // 기존 indices 초기화

    float xOffset = text.size() * gap; // 텍스트의 x축 위치를 조정하기 위한 오프셋
    float yOffset = 0.0f; // 텍스트의 y축 위치를 고정

    for (size_t i = 0; i < text.size(); ++i) {
        // 각 문자의 아스키 코드 값 구하기
        wchar_t character = text[i];
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
    // 기존 버퍼 삭제 (리소스 해제)
    _vertexBuffer.reset();
    _indexBuffer.reset();

    // 새로운 텍스트로 버텍스와 인덱스 다시 생성
    CreateBufferForText(newText);

    // 새로운 버텍스와 인덱스를 다시 GPU에 업로드
    SetVerticesToPipeline();
}

void UTextComponent::Update()
{
    UPrimitiveComponent* Selected = GuiController::GetInstance().GetSelectedObject();

    if (Selected)
    {
        FString uid = Selected->GetName().ToString();

        std::wstring wstr(uid.begin(), uid.end());

        UpdateText(wstr);

        translate = FMatrix::Translate
        (Selected->GetRelativeLocation().x,
            Selected->GetRelativeLocation().y,
            Selected->GetRelativeLocation().z + 3.f);
        myPos = Selected->GetRelativeLocation();
    }
    else
    {
        std::wstring wss = L"Welcome to Jungle";
        UpdateText(wss);

        myPos = RelativeLocation;
        translate = FMatrix::Translate(RelativeLocation);
    }
}

FMatrix UTextComponent::GetComponentTransform() const
{

    FMatrix viewMatrix = CRenderer::Instance()->GetMainCamera()->View();
   
    FVector cameraPos = CRenderer::Instance()->GetMainCamera()->GetRelativeLocation();
   
    // ✅ 카메라 위치벡터 - 오브젝트 위치벡터를 통한 forward 벡터 생성
    FVector forward = cameraPos - myPos;
    forward = forward.Normalized();

    // ✅ 카메라 Up 벡터를 기반으로 right 벡터 계산
    FVector cameraUp = FVector(viewMatrix.c2().x, viewMatrix.c2().y, viewMatrix.c2().z);

    // ✅ forward 벡터, 카메라 Up벡터 외적으로 right계산 
    FVector right = forward.Cross(cameraUp);
    right = right.Normalized(); 

    // ✅ right 벡터,  forward벡터 외적으로 up계산 
    FVector up = right.Cross(forward);
    up = up.Normalized(); 

    // ✅ 카메라 바라보는 행렬 생성
    FMatrix rotMat = FMatrix{
        right.x, right.y, right.z, 0,  
        up.x, up.y, up.z, 0,       
        forward.x, forward.y, forward.z, 0, 
        translate.m[3][0], translate.m[3][1], translate.m[3][2], 1
    };

    return rotMat;
}

