// UWorldGridComponent.h
#pragma once
#include "UPrimitiveComponent.h"
// �̰� ���丮 ���� �ַ������ �ٲ����
#include <vector>
#include "./Math/FVector.h"

class UWorldGridComponent : public UPrimitiveComponent
{
public:
    UWorldGridComponent();
    ~UWorldGridComponent();

    // grid ���� (-gridCount ~ +gridCount) �� ���� ũ��(unitSize)�� �����Ͽ� grid�� ����
    void GenerateGrid(float posX, float posZ, int gridCount, float unitSize);

    void UpdateGrid();

    // UPrimitiveComponent���� ��ӹ��� Render �Լ� �������̵�
    virtual void Render() override;

private:

    int lastPosX;
    int lastPosZ;
    int newPosX;
    int newPosZ;
};