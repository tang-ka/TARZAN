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
    // 소멸자에서는 std::unique_ptr가 자동으로 정리합니다.
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

    // 내부 COM 객체들은 CGraphics 내부에서 ComPtr로 관리하므로, GetGraphics()는 raw pointer 반환
    CGraphics* GetGraphics() { return _graphics.get(); }
	void SetVertexShader(const FWString filename, FString funcname, FString version);
	void ResetVertexShader();
	void SetPixelShader(const FWString filename, FString funcname, FString version);
	void ResetPixelShader();
	void SetRasterzierState(D3D11_FILL_MODE fillMode);
	void SetTransformToConstantBuffer(FMatrix matrix, bool isBill=false);
	void SetFlagsToConstantBuffer(FPrimitiveFlags flags);
	UCameraComponent* GetMainCamera() const;
	void SetMainCamera(UCameraComponent* camera);

	void SetDepthStencil(ID3D11DepthStencilState* pDSState);

	void DrawLine(TArray<FVertexSimple> vertices, TArray<uint32> indices, FVector4 color) const;

private:
    // CRenderer가 소유하는 객체는 std::unique_ptr로 관리
    std::unique_ptr<CGraphics> _graphics;
    std::unique_ptr<CVertexShader> _vertexShader;   
    std::unique_ptr<CPixelShader> _pixelShader;
    std::unique_ptr<CInputLayout> _inputLayout;
    std::unique_ptr<CRasterzierState> _rasterizerState;
    std::unique_ptr<CConstantBuffer<FMatrix>> _matrixBuffer;
    std::unique_ptr<CConstantBuffer<FPrimitiveFlags>> _flagsBuffer;
    UCameraComponent* _mainCamera = nullptr; // �������� �ٸ� ������ �����ϴ� ������ ����

};
