#pragma once

#include "./Framework/Core/UPrimitiveComponent.h"

// 텍스트 아틀라스를 사용하는 문자 UV 좌표를 저장하는 구조체


// 텍스트 UI 컴포넌트 클래스
class UTextComponent : public UPrimitiveComponent
{
public:
    std::wstring Text;
    UTextComponent();
    void SetVerticesToPipeline();
    void CreateBufferForText(std::wstring text);
    void UpdateText(std::wstring newText);
private:

};
