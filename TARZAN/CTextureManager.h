#pragma once

#include <d3d11.h>
#include <DirectXTex.h>
#include <wrl/client.h>
#include <string>
#include <map>
#include <EObjectType.h>

using namespace DirectX;
using Microsoft::WRL::ComPtr;

class CTextureManager {
public:
    // 예시 텍스처 경로 배열
    std::wstring TexturePaths[2] = {
        L"Texture/TestTexure.dds",
        L"Texture/FontTexture.dds"
    };

    int slotCount = 0;
    const int NumberOfTexture = sizeof(TexturePaths) / sizeof(TexturePaths[0]);

    // 싱글턴 인스턴스 얻기
    static CTextureManager* GetInstance();
    static void SetDeviceAndContext(ID3D11Device* device, ID3D11DeviceContext* context);

    // 텍스처 로드 함수 (DDS 포맷)
    void LoadSetTexture();

    // 텍스처를 셰이더에 바인딩하는 함수 (EObjectType에 따라)
    void BindTextureToShader(EObjectType type);

    // 샘플러 생성 함수
    void CreateSamplerState();

    // 텍스처의 셰이더 리소스 뷰를 반환하는 함수
    ID3D11ShaderResourceView* GetTextureView(const std::wstring& filePath);

private:
    // 생성자와 복사 생성자, 대입 연산자 금지
    CTextureManager(ID3D11Device* device, ID3D11DeviceContext* context);
    CTextureManager(const CTextureManager&) = delete;
    CTextureManager& operator=(const CTextureManager&) = delete;

    // 소멸자
    ~CTextureManager();

    // SamplerState를 ComPtr로 관리
    ComPtr<ID3D11SamplerState> SamplerState;
    // 외부에서 제공하는 Device, Context는 소유하지 않으므로 raw pointer 사용
    ID3D11Device* Device;
    ID3D11DeviceContext* Context;
    // 텍스처 파일 경로와 뷰를 매핑 (ComPtr로 관리)
    std::map<std::wstring, ComPtr<ID3D11ShaderResourceView>> m_textureMap;

    // 싱글턴 인스턴스
    static CTextureManager* s_instance;
};
