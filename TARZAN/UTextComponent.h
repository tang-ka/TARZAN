#pragma once

#include "./Framework/Core/UPrimitiveComponent.h"

// �ؽ�Ʈ ��Ʋ�󽺸� ����ϴ� ���� UV ��ǥ�� �����ϴ� ����ü


// �ؽ�Ʈ UI ������Ʈ Ŭ����
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
