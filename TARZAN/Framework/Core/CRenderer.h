#pragma once
#include "Framework\DirectXWrapper\CShader.h"
#include "Framework\DirectXWrapper\CInputLayout.h"
#include "Framework\DirectXWrapper\CGraphics.h"
#include "Framework\DirectXWrapper\CState.h"
#include "Framework\DirectXWrapper\CBuffer.h"
#include "./UCameraComponent.h"
#include "CTextureManager.h"
#include <memory>

class CRenderer {
private:
    static CRenderer* _instance;
    CRenderer();
    CRenderer(const CRenderer& ref) = delete;
    CRenderer& operator=(const CRenderer& ref) = delete;
    // �Ҹ��ڿ����� std::unique_ptr�� �ڵ����� �����մϴ�.
    ~CRenderer() = default;

public:
    static CRenderer* Instance() {
        if (_instance == nullptr)
            _instance = new CRenderer();
        return _instance;
    }
    static void Release() {
        delete _instance;
        _instance = nullptr;
    }
    void Init(HWND hWnd);

    // ���� COM ��ü���� CGraphics ���ο��� ComPtr�� �����ϹǷ�, GetGraphics()�� raw pointer ��ȯ
    CGraphics* GetGraphics() { return _graphics.get(); }

    void SetVertexShader(const FWString filename, FString funcname, FString version);
    void ResetVertexShader();
    void SetPixelShader(const FWString filename, FString funcname, FString version);
    void ResetPixelShader();
    void SetRasterzierState(D3D11_FILL_MODE fillMode);
    void SetTransformToConstantBuffer(FMatrix matrix);
    void SetFlagsToConstantBuffer(FPrimitiveFlags flags);
    UCameraComponent* GetMainCamera() const;
    void SetMainCamera(UCameraComponent* camera);

	void DrawLine(TArray<FVertexSimple> vertices, TArray<uint32> indices, FVector4 color) const;

private:
    // CRenderer�� �����ϴ� ��ü�� std::unique_ptr�� ����
    std::unique_ptr<CGraphics> _graphics;
    std::unique_ptr<CVertexShader> _vertexShader;   
    std::unique_ptr<CPixelShader> _pixelShader;
    std::unique_ptr<CInputLayout> _inputLayout;
    std::unique_ptr<CRasterzierState> _rasterizerState;
    std::unique_ptr<CConstantBuffer<FMatrix>> _matrixBuffer;
    std::unique_ptr<CConstantBuffer<FPrimitiveFlags>> _flagsBuffer;
    UCameraComponent* _mainCamera = nullptr; // �������� �ٸ� ������ �����ϴ� ������ ����

    // CTextureManager�� �̱������� �����ǹǷ� raw pointer�� �����մϴ�.
    CTextureManager* textureManager;
};
