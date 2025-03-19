#pragma once

#include "./Framework/Core/UPrimitiveComponent.h"

// �ؽ�Ʈ ��Ʋ�󽺸� ����ϴ� ���� UV ��ǥ�� �����ϴ� ����ü


// �ؽ�Ʈ UI ������Ʈ Ŭ����
class UTextComponent : public UPrimitiveComponent
{
public:
    std::wstring Text;
    UTextComponent();
    void CreateBufferForText(std::wstring text);
    void SetVerticesToPipeline();
    void UpdateText(std::wstring newText);
    virtual void Update() override;
    virtual FMatrix GetComponentTransform() const override;

private:
    FVector myPos = FVector::Zero;
    FMatrix translate = FMatrix::Identity;
};
