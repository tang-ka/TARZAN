// UWorldGridComponent.h
#pragma once
#include "UPrimitiveComponent.h"
// 이거 디렉토리 구조 솔루션으로 바꿔야함
#include <vector>
#include "./Math/FVector.h"

class UWorldGridComponent : public UPrimitiveComponent
{
public:
    UWorldGridComponent();
    ~UWorldGridComponent();

    // grid 범위 (-gridCount ~ +gridCount) 및 단위 크기(unitSize)를 설정하여 grid를 생성
    void GenerateGrid(float posX, float posZ, int gridCount, float unitSize);

    void UpdateGrid();

    // UPrimitiveComponent에서 상속받은 Render 함수 오버라이드
    virtual void Render() override;

private:

    int lastPosX;
    int lastPosZ;
    int newPosX;
    int newPosZ;
};